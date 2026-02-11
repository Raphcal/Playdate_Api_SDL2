//
//  scoreattackscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 07/11/2024.
//

#ifndef scoreattackscene_h
#define scoreattackscene_h

#include "common.h"
#include "../lib/melice.h"

#include "gameflow.h"
#include "story.h"
#include "../gen/maps.h"

typedef enum {
    ScoreAttackDifficultyEasy,
    ScoreAttackDifficultyNormal,
    ScoreAttackDifficultyHard,
    ScoreAttackDifficultyCount
} ScoreAttackDifficulty;

typedef struct {
    MELScene super;
    MELMap * _Nullable boardMap;
    MELMap * _Nullable background;
    MapName backgroundName;
    MELCamera backgroundCamera;
    LCDSprite * _Nullable caroline;
    LCDSprite * _Nullable nextTargetLabel;
    LCDSprite * _Nullable nextTargetCounter;
    LCDSprite * _Nullable levelCounter;
    float speedY;
    float timeBeforeJump;
    uint32_t filePlayerOffset;
    CardList cards;
    GameFlow gameFlow;
    uint32_t score;
    uint32_t nextTarget;
    int removedCards;
    uint8_t cheerCombo;
    float time;
    uint8_t level;
    int8_t nextTargetIndex;
} ScoreAttackScene;

ScoreAttackScene * _Nonnull ScoreAttackSceneAlloc(ScoreAttackDifficulty difficulty);

#endif /* scoreattackscene_h */
