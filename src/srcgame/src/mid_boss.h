//
//  midboss.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 08/04/2023.
//

#ifndef midboss_h
#define midboss_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull MidBossConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable MidBossLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* midboss_h */
