//
//  scoreboard.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 10/11/2024.
//

#ifndef scoreboard_h
#define scoreboard_h

#include "common.h"
#include "../lib/melice.h"

#include "scores.h"

LCDSprite * _Nullable ScoreboardConstructor(ScoreList * _Nonnull scores, float top);
void ScoreboardRepaint(LCDSprite * _Nonnull sprite);

float ScoreboardGetTop(LCDSprite * _Nonnull sprite);
void ScoreboardSetTop(LCDSprite * _Nonnull sprite, float top);

#endif /* scoreboard_h */
