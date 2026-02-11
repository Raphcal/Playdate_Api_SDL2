//
//  shark.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 06/03/2023.
//

#ifndef shark_h
#define shark_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull SharkConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable SharkLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* shark_h */
