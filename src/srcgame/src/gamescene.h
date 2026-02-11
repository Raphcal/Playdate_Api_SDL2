//
//  gamescene.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 24/01/2023.
//

#ifndef gamescene_h
#define gamescene_h

#include "common.h"
#include "../lib/melice.h"

#include "gameflow.h"
#include "story.h"
#include "../gen/maps.h"

typedef struct {
    MELScene super;
    const Story * _Nullable story;
    uint32_t encounterIndex;
    MELMap * _Nullable background;
    MapName backgroundName;
    uint32_t filePlayerOffset;
    CardList cards;
    GameFlow leftPlayerGameFlow;
    GameFlow rightPlayerGameFlow;
    float time;
    float speed;
    MELBoolean pinch;
} GameScene;

GameScene * _Nonnull GameSceneAlloc(const Story * _Nullable story, uint32_t encounterIndex);

#endif /* gamescene_h */
