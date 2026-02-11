//
//  rallyresultsscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 12/08/2025.
//

#ifndef rallyresultsscene_h
#define rallyresultsscene_h

#include "common.h"
#include "../lib/melice.h"

#include "rally.h"

typedef enum {
    RallyResultsCounterTotalTime,
    RallyResultsCounterStage,
    RallyResultsCounterWinStreak,
    RallyResultsCounterDeathCount,
    RallyResultsCounterScore,
    RallyResultsCounterCount,
} RallyResultsCounter;

typedef struct {
    MELScene super;
    MELMap * _Nullable map;
    MELSprite * _Nullable title;
    ArcadeGame game;
    ArcadeGame counts;
    float time;
    MELBoolean success;
    LCDSpriteRefList labels;
    LCDSpriteRefList counters;
} RallyResultsScene;

RallyResultsScene * _Nonnull RallyResultsSceneAlloc(ArcadeGame game, MELBoolean success);

#endif /* rallyresultsscene_h */
