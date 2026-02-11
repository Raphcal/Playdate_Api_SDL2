//
//  mapscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 03/04/2024.
//

#ifndef mapscene_h
#define mapscene_h

#include "common.h"

#include "story.h"

typedef struct {
    MELScene super;
    const Story * _Nullable story;
    unsigned int encounterIndex;
    float time;
} MapScene;

MapScene * _Nonnull MapSceneAlloc(const Story * _Nullable story, unsigned int encounterIndex);

#endif /* mapscene_h */
