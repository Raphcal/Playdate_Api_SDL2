//
//  palettecache.c
//  Roll
//
//  Created by Raphaël Calabro on 24/08/2022.
//

#include "palettecache.h"

static const int kCacheSize = 4;
static const int kPalettesToMove = kCacheSize - 1;
static const int kLastEntryIndex = kCacheSize - 1;

MELListImplement(PaletteName);
MELListImplementIndexOf(PaletteName);

MELKeyValueTableImplement(PaletteName, LCDBitmapTableRef);

PaletteCache PaletteCacheMake(void) {
    return (PaletteCache) {
        .order = PaletteNameListMakeWithInitialCapacity(kCacheSize),
        .palettes = PaletteNameLCDBitmapTableRefTableEmpty
    };
}

void PaletteCacheDeinit(PaletteCache * _Nonnull self) {
    PaletteNameListDeinit(&self->order);
    PaletteNameLCDBitmapTableRefTableEntryList entries = PaletteNameLCDBitmapTableRefTableEntries(&self->palettes);
    for (unsigned int index = 0; index < entries.count; index++) {
        playdate->graphics->freeBitmapTable(entries.memory[index].value);
    }
    PaletteNameLCDBitmapTableRefTableEntryListDeinit(&entries);
    PaletteNameLCDBitmapTableRefTableDeinit(&self->palettes);
}

LCDBitmapTable * _Nullable PaletteCacheGet(PaletteCache * _Nonnull cache, PaletteName paletteName) {
    PaletteCache self = *cache;
    int index;
    LCDBitmapTable *result = NULL;
    if (!PaletteNameLCDBitmapTableRefTableGet(self.palettes, paletteName, &result)) {
#if SHOW_LOADED_PALETTES
        playdate->system->logToConsole("Loading map palette %d", paletteName);
#endif
        result = PaletteNameLoadBitmapTable(paletteName);
        PaletteNameLCDBitmapTableRefTablePut(&self.palettes, paletteName, result);

        if (self.order.count < kCacheSize) {
            PaletteNameListPush(&self.order, paletteName);
        } else {
            LCDBitmapTable *paletteToFree;
            if (PaletteNameLCDBitmapTableRefTableRemoveAndGetOldValue(&self.palettes, self.order.memory[0], &paletteToFree)) {
    #if SHOW_LOADED_PALETTES
                playdate->system->logToConsole("Freeing map palette %d", self.order.memory[0]);
    #endif
                playdate->graphics->freeBitmapTable(paletteToFree);
            } else {
                playdate->system->error("Unable to find map palette %d to free from PaletteCache#palettes.", self.order.memory[0]);
            }

            memmove(self.order.memory, self.order.memory + 1, sizeof(PaletteName) * kPalettesToMove);
            self.order.memory[kLastEntryIndex] = paletteName;
        }
    } else if ((index = PaletteNameListIndexOf(self.order, paletteName)) != self.order.count - 1) {
        memmove(self.order.memory + index, self.order.memory + index + 1, sizeof(PaletteName) * (self.order.count - index - 1));
        self.order.memory[self.order.count - 1] = paletteName;
    }
    *cache = self;
    return result;
}
