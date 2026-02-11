//
//  gameflow.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#ifndef gameflow_h
#define gameflow_h

#include "common.h"

#include "side.h"
#include "card.h"
#include "board.h"
#include "actor.h"

typedef enum {
    GameFlowStateInitial,
    GameFlowStateNewHand,
    GameFlowStatePlay,
    GameFlowStateLockDelay,
    GameFlowStateChain,
    GameFlowStateResolve,
    GameFlowStatePause,
    GameFlowStateCommit,
    GameFlowStateDefend,
    GameFlowStateChipFall,
    GameFlowStateLost,
    GameFlowStateWon,
} GameFlowState;

typedef struct gameflow GameFlow;

typedef struct gameflow {
    void (* _Nonnull update)(GameFlow * _Nonnull self);
    Side side;
    Board * _Nullable board;
    CardList cards;
    unsigned int cardIndex;
    GameFlowState state;
    GameFlowState nextState;
    MELTimeInterval pause;
    CardSprites hand;
    LCDSprite * _Nullable cursor;
    Card nextHand[2];
    LCDSpriteRefList preview;
    Actor * _Nullable actor;
    int32_t chips;
    uint32_t chainCount;
    int32_t receivedChips;
    LCDSpriteRefList chipPreview;
    MELBoolean waitingForDefense;
} GameFlow;

void GameFlowInit(GameFlow * _Nonnull self, const Side side, Board * _Nullable board, CardList cards);
void GameFlowDeinit(GameFlow * _Nonnull self);
void GameFlowPopulateWithCards(GameFlow * _Nonnull self, const char * _Nonnull cards, unsigned int length);

void GameFlowSetState(GameFlow * _Nonnull self, GameFlowState state);

#endif /* gameflow_h */
