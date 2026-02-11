//
//  openingscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 27/07/2024.
//

#ifndef openingscene_h
#define openingscene_h

#include "common.h"
#include "../lib/melice.h"

typedef enum {
    OpeningSceneStateIntro,
    OpeningSceneStateHeroes,
    OpeningSceneStateOpponents,
    OpeningSceneStateDanse,
    OpeningSceneStateCave,
    OpeningSceneStateOutro,
    OpeningSceneStateTitle,
} OpeningSceneState;

typedef struct {
    MELScene super;
    LCDSprite * _Nullable fade;
    MELMap * _Nullable forestMap;
    MELMap * _Nullable horizontalMap;
    MELMap * _Nullable verticalMap;
    MELMap * _Nullable danseBackground;
    MELCamera camera;
    float time;
    float duration;
    OpeningSceneState state;
} OpeningScene;

OpeningScene * _Nonnull OpeningSceneAlloc(void);

#endif /* openingscene_h */
