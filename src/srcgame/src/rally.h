//
//  rally.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 20/07/2025.
//

#ifndef rally_h
#define rally_h

#include "story.h"
#include "card.h"

typedef struct {
    uint32_t score;
    float time;
    uint32_t deathCount;
    uint32_t continueCount;
    float speed;
    uint8_t stage;
    uint8_t bestWinStreak;
    uint8_t winStreak;
    uint8_t loseStreak;
} ArcadeGame;

typedef struct {
    ArcadeGame game;
    char * _Nullable leftBoard;
    char * _Nullable rightBoard;
    char * _Nullable deck;
    uint32_t leftDeckIndex;
    uint32_t rightDeckIndex;
} ArcadeSaveState;

extern const EncounterList kRallyEncounters;

MELBoolean ArcadeGameIsEnding(ArcadeGame game);

#endif /* rally_h */
