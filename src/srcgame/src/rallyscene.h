//
//  rallyscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 20/07/2025.
//

#ifndef rallyscene_h
#define rallyscene_h

#include "common.h"
#include "../lib/melice.h"

#include "gameflow.h"
#include "story.h"
#include "rally.h"
#include "../gen/maps.h"

typedef struct {
    MELScene super;
    MELMap * _Nullable background;
    MELNineSlice * _Nullable dialogBorder;
    LCDSprite * _Nullable dialog;
    const char * _Nullable music;
    char * _Nullable leftBoard;
    char * _Nullable rightBoard;
    MapName backgroundName;
    uint32_t filePlayerOffset;
    CardList cards;
    GameFlow leftPlayerGameFlow;
    GameFlow rightPlayerGameFlow;
    ArcadeGame game;
    /// Score avant l'ajout du bonus de points.
    uint32_t baseScore;
    /// Points gagnés, égale au bonus mais ne change pas avec l'animation.
    uint32_t earnedScore;
    /// Bonus affiché sur l'interface en cas de victoire.
    uint32_t bonusScore;
    /// Durée de la partie en secondes, pour l'affichage.
    uint32_t timeInSeconds;
    float time;
    float speed;
    float lastSound;
    uint16_t leftDeckIndex;
    uint16_t rightDeckIndex;
    uint8_t sound;
    MELBoolean pinch;
} RallyScene;

RallyScene * _Nonnull RallySceneAlloc(ArcadeGame game);
RallyScene * _Nonnull RallySceneAllocWithSaveState(ArcadeSaveState saveState);

#endif /* rallyscene_h */
