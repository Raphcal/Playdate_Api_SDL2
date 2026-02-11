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

#include "carddeck.h"
#include "savegame.h"

#define kCarolineLeftPadding 76

typedef enum {
    TitleMenuItemStory,
    TitleMenuItemArcade,
    TitleMenuItemScoreAttack,
    TitleMenuItemSchool,
    TitleMenuItemCount
} TitleMenuItem;

typedef struct {
    MELScene super;
    MELMap * _Nullable map;
    MELSprite * _Nullable fileSelectCaroline;
#if TITLE_SCREEN_CARD_FOUNTAIN
    CardDeck cards;
#endif
    LCDSprite * _Nullable menuGridSprite;
    MELGridView * _Nullable menuGrid;
    SaveGameList saveGames;
    LCDSprite * _Nullable saveGameTitle;
    LCDSprite * _Nullable saveGameDescription;
    float time;
    float from;
    float to;
    int8_t selectedSaveGame;
    MELLanguage oldLanguage;
    TitleMenuItem initialMenuSelection;
} TitleScene;

TitleScene * _Nonnull TitleSceneAlloc(void);

#endif /* titlescene_h */
