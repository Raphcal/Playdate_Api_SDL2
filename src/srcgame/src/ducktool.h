//
//  ducktool.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 21/05/2023.
//

#ifndef ducktool_h
#define ducktool_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull DucktoolConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable DucktoolLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);


#endif /* ducktool_h */
