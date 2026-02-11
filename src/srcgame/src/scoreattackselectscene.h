//
//  scoreattackselectscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 10/11/2024.
//

#ifndef scoreattackselectscene_h
#define scoreattackselectscene_h

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
} ScoreAttackSelectScene;

extern const MELLocalizedString kLastResults;
extern const MELLocalizedString kLeaderboad;
extern const MELLocalizedString kLoading;
extern const MELLocalizedString kEmpty;

ScoreAttackSelectScene * _Nonnull ScoreAttackSelectSceneAlloc(void);

#endif /* scoreattackselectscene_h */
