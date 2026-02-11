//
//  scoreentryscene.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 04/07/2023.
//

#ifndef scoreentryscene_h
#define scoreentryscene_h

#include "common.h"
#include "../lib/melice.h"
#include "rankingitem.h"
#include "player.h"

typedef struct {
    MELScene super;
    MELKeyboard * _Nullable keyboard;
    Score score;
    Score elevenScore;
    RankingItem * _Nullable rankingItem;
    MELSprite * _Nullable rankLabel;

    LCDBitmapTable * _Nullable faces;

    FilePlayer * _Nullable filePlayer;
    MELBoolean showTransition;
} ScoreEntryScene;

ScoreEntryScene * _Nullable ScoreEntryAlloc(PlayerCharacter player, unsigned int score, Ranks ranks);

#endif /* scoreentryscene_h */
