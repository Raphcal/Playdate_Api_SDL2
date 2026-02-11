//
//  endingscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 30/03/2024.
//

#ifndef endingscene_h
#define endingscene_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELScene super;
    MELMap * _Nullable map;
    MELSprite * _Nullable trailingCharacter;
} EndingScene;

EndingScene * _Nonnull EndingSceneAlloc(void);

#endif /* endingscene_h */
