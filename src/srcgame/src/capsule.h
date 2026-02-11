//
//  capsule.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/02/2023.
//

#ifndef capsule_h
#define capsule_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull CapsuleConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable CapsuleLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* capsule_h */
