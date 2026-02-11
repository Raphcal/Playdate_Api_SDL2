//
//  spritepalettecache.c
//  Roll
//
//  Created by Raphaël Calabro on 01/09/2022.
//

#include "spritepalettecache.h"

static const int kCacheSize = 20;
static const int kPalettesToMove = kCacheSize - 1;
static const int kLastEntryIndex = kCacheSize - 1;

MELListImplement(SpriteName);
MELListImplementIndexOf(SpriteName);

MELKeyValueTableImplement(SpriteName, LCDBitmapTableRef);

SpritePaletteCache SpritePaletteCacheMake(void) {
    return (SpritePaletteCache) {
        .order = SpriteNameListMakeWithInitialCapacity(kCacheSize),
        .palettes = SpriteNameLCDBitmapTableRefTableEmpty
    };
}

void SpritePaletteCacheDeinit(SpritePaletteCache * _Nonnull self) {
    SpriteNameListDeinit(&self->order);
    SpriteNameLCDBitmapTableRefTableEntryList entries = SpriteNameLCDBitmapTableRefTableEntries(&self->palettes);
    for (unsigned int index = 0; index < entries.count; index++) {
        playdate->graphics->freeBitmapTable(entries.memory[index].value);
    }
    SpriteNameLCDBitmapTableRefTableEntryListDeinit(&entries);
    SpriteNameLCDBitmapTableRefTableDeinit(&self->palettes);
}

LCDBitmapTable * _Nullable SpritePaletteCacheGet(SpritePaletteCache * _Nonnull cache, SpriteName spriteName) {
    SpritePaletteCache self = *cache;
    int index;
    LCDBitmapTable *result = NULL;
    if (!SpriteNameLCDBitmapTableRefTableGet(self.palettes, spriteName, &result)) {
#if SHOW_LOADED_PALETTES
        playdate->system->logToConsole("Loading sprite palette %d", spriteName);
#endif
        result = SpriteNameLoadBitmapTable(spriteName);
        SpriteNameLCDBitmapTableRefTablePut(&self.palettes, spriteName, result);

        if (self.order.count < kCacheSize) {
            SpriteNameListPush(&self.order, spriteName);
        } else {
            // Free old palettes.
            LCDBitmapTable *paletteToFree;
            if (SpriteNameLCDBitmapTableRefTableRemoveAndGetOldValue(&self.palettes, self.order.memory[0], &paletteToFree)) {
    #if SHOW_LOADED_PALETTES
                playdate->system->logToConsole("Freeing sprite palette %d", self.order.memory[0]);
    #endif
                playdate->graphics->freeBitmapTable(paletteToFree);
            } else {
                playdate->system->error("Unable to find sprite palette %d to free from SpritePaletteCache#palettes.", self.order.memory[0]);
            }

            memmove(self.order.memory, self.order.memory + 1, sizeof(SpriteName) * kPalettesToMove);
            self.order.memory[kLastEntryIndex] = spriteName;
        }
    } else if ((index = SpriteNameListIndexOf(self.order, spriteName)) != self.order.count - 1) {
        memmove(self.order.memory + index, self.order.memory + index + 1, sizeof(SpriteName) * (self.order.count - index - 1));
        self.order.memory[self.order.count - 1] = spriteName;
    }
    *cache = self;
    return result;
}
