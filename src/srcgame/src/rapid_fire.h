//
//  quickshoot.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 14/02/2023.
//

#ifndef rapid_fire_h
#define rapid_fire_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull RapidFireConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable RapidFireLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* rapid_fire_h */
