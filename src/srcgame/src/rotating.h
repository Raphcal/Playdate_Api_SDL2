//
//  rotating.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/02/2023.
//

#ifndef rotating_h
#define rotating_h

#include "common.h"
#include "../lib/melice.h"

/**
 * @deprecated Inutilisé.
 */
__attribute__((deprecated)) LCDSprite * _Nonnull RotatingConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

/**
 * @deprecated Inutilisé.
 */
__attribute__((deprecated)) MELSprite * _Nullable RotatingLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* rotating_h */
