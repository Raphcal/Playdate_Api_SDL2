//
//  rallyscorescene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 10/07/2025.
//

#ifndef rallyscorescene_h
#define rallyscorescene_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELScene super;
    MELNineSlice * _Nullable nineSlice;
    MELGridView * _Nullable gridView;
    LCDSprite * _Nullable gridViewSprite;
    LCDSprite * _Nullable activityIndicator;
    LCDSprite * _Nullable loadingMessage;
    LCDSprite * _Nullable scoreboard;
    LCDSprite * _Nullable scrollbar;
    float time;
    float scrollSpeed;
} RallyScoreScene;

RallyScoreScene * _Nonnull RallyScoreSceneAlloc(void);

#endif /* rallyscorescene_h */
