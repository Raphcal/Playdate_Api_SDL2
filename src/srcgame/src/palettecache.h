//
//  palettecache.h
//  Roll
//
//  Created by Raphaël Calabro on 24/08/2022.
//

#ifndef palettecache_h
#define palettecache_h

#include "common.h"
#include "references.h"
#include "../lib/melice.h"
#include "../gen/palettenames.h"

MELListDefine(PaletteName);
MELListDefineIndexOf(PaletteName);

MELKeyValueTableDefine(PaletteName, LCDBitmapTableRef);

typedef struct {
    PaletteNameList order;
    PaletteNameLCDBitmapTableRefTable palettes;
} PaletteCache;

/**
 * Creates an empty palette cache.
 * @return An empty palette cache.
 */
PaletteCache PaletteCacheMake(void);

/**
 * Free the resources and palette loaded by the cache.
 *
 * @param self Cache to deinitialize.
 */
void PaletteCacheDeinit(PaletteCache * _Nonnull self);

/**
 * Get the given palette from the cache or load it from the disk.
 *
 * @param self Cache to load from.
 * @param paletteName Name of the palette to get or load.
 * @return The palette.
 */
LCDBitmapTable * _Nullable PaletteCacheGet(PaletteCache * _Nonnull self, PaletteName paletteName);

#endif /* palettecache_h */
