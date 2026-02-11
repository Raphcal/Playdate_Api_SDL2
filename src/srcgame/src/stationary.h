//
//  stationary.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/01/2023.
//

#ifndef stationary_h
#define stationary_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull StationaryConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable StationaryLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* stationary_h */
