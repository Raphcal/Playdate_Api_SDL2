//
//  storyselectscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 25/01/2024.
//

#ifndef storyselectscene_h
#define storyselectscene_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELScene super;
    MELGridView * _Nullable gridView;
    MELMap * _Nullable backgroundMap;
    LCDBitmap * _Nullable cellBackground;
    LCDBitmap * _Nullable selected;
    LCDSprite * _Nullable dialog;
    MELNineSlice * _Nullable menuBorder;
    MELNineSlice * _Nullable menuSelection;
} StorySelectScene;

StorySelectScene * _Nonnull StorySelectSceneAlloc(void);

#endif /* storyselectscene_h */
