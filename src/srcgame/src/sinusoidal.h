//
//  sinusoidal.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 13/02/2023.
//

#ifndef sinusoidal_h
#define sinusoidal_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull SinusoidalConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable SinusoidalLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* sinusoidal_h */
