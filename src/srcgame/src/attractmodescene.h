//
//  attractmodescene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/08/2024.
//

#ifndef gamescene_h
#define gamescene_h

#include "common.h"

#include "../lib/melice.h"
#include "gameflow.h"
#include "../gen/maps.h"
#include "story.h"

typedef struct {
    MELScene super;
    MELMap * _Nullable background;
    MapName backgroundName;
    CardList cards;
    GameFlow leftPlayerGameFlow;
    GameFlow rightPlayerGameFlow;
    float time;
    float speed;
} AttractModeScene;

AttractModeScene * _Nonnull AttractModeSceneAlloc(void);

#endif /* gamescene_h */
