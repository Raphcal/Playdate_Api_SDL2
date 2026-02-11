//
//  rallyintroscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 13/07/2025.
//

#ifndef rallyintroscene_h
#define rallyintroscene_h

#include "common.h"
#include "../lib/melice.h"

#include "rally.h"

typedef struct {
    MELScene super;
    MELNineSlice * _Nullable border;
    MELNineSlice * _Nullable selectedBorder;
    MELGridView * _Nullable gridView;
    LCDSprite * _Nullable gridViewSprite;
    LCDSprite * _Nullable scrollbar;
    float time;
    ArcadeGame game;
} RallyIntroScene;

RallyIntroScene * _Nonnull RallyIntroSceneAlloc(ArcadeGame game);

#endif /* rallyintroscene_h */
