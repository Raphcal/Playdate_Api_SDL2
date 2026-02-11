//
//  spritepalettecache.h
//  Roll
//
//  Created by Raphaël Calabro on 01/09/2022.
//

#ifndef spritepalettecache_h
#define spritepalettecache_h

#include "common.h"
#include "references.h"
#include "../lib/melice.h"
#include "../gen/spritenames.h"

MELListDefine(SpriteName);
MELListDefineIndexOf(SpriteName);

MELKeyValueTableDefine(SpriteName, LCDBitmapTableRef);

typedef struct spritepalettecache {
    SpriteNameList order;
    SpriteNameLCDBitmapTableRefTable palettes;
} SpritePaletteCache;

/**
 * Creates an empty palette cache.
 * @return An empty palette cache.
 */
SpritePaletteCache SpritePaletteCacheMake(void);

/**
 * Free the resources and palette loaded by the cache.
 *
 * @param self Cache to deinitialize.
 */
void SpritePaletteCacheDeinit(SpritePaletteCache * _Nonnull self);

/**
 * Get the given palette from the cache or load it from the disk.
 *
 * @param self Cache to load from.
 * @param spriteName Name of the sprite to get or load.
 * @return The palette.
 */
LCDBitmapTable * _Nullable SpritePaletteCacheGet(SpritePaletteCache * _Nonnull self, SpriteName spriteName);


#endif /* spritepalettecache_h */
