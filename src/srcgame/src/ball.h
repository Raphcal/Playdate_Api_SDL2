//
//  ball.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 06/03/2023.
//

#ifndef ball_h
#define ball_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull BallConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable BallLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* ball_h */
