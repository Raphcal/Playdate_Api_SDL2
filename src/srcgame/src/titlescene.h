//
//  titlescene.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 06/03/2023.
//

#ifndef titlescene_h
#define titlescene_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELScene super;
    MELMap * _Nullable map;
    LCDBitmapTable * _Nullable palette;
    LCDSprite * _Nullable startText;
    LCDSprite * _Nullable crankToTitle;
    LCDSprite * _Nullable versionNumber;
    MELSprite * _Nullable rankingsMenuItem;
    uint32_t scoreLastUpdateDate;

    LCDBitmap * _Nullable faceBackground;
    LCDBitmapTable * _Nullable faces;
    LCDBitmapTable * _Nullable sectionHeader;

    int height;

    float time;
    float scrollSpeed;
    MELBoolean scrollToRankings;
    MELBoolean showTransition;
} TitleScene;

TitleScene * _Nonnull TitleSceneAlloc(void);

#endif /* titlescene_h */
