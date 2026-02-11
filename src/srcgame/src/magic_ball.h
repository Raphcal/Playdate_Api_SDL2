//
//  magic_ball.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 13/01/2024.
//

#ifndef magic_ball_h
#define magic_ball_h

#include "common.h"
#include "../lib/melice.h"

#include "board.h"

LCDSprite * _Nonnull MagicBallConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);
LCDSprite * _Nonnull MagicBallConstructorAttack(Side side, int chips, MELPoint origin);
LCDSprite * _Nonnull MagicBallConstructorDefend(Side side, int chips, MELPoint origin);
LCDSprite * _Nonnull MagicBallConstructorCounter(Side side, int chips);

MELSprite * _Nullable MagicBallLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* magic_ball_h */
