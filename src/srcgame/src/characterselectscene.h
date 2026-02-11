//
//  characterselectscene.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/08/2023.
//

#ifndef characterselectscene_h
#define characterselectscene_h

#include "common.h"
#include "player.h"
#include "../lib/melice.h"

typedef struct {
    MELScene super;
    FilePlayer * _Nullable filePlayer;
    LCDBitmapTable * _Nullable artworks;
    LCDBitmapTable * _Nullable names;
    LCDBitmapTable * _Nullable controls;
    PlayerCharacter selection;
} CharacterSelectScene;

CharacterSelectScene * _Nonnull CharacterSelectSceneAlloc(void);

#endif /* characterselectscene_h */
