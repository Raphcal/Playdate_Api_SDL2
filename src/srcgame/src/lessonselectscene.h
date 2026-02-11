//
//  lessonselectscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 21/03/2024.
//

#ifndef lessonselectscene_h
#define lessonselectscene_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELScene super;
    MELMap * _Nullable map;
    MELGridView * _Nullable gridView;
    LCDSprite * _Nullable gridViewSprite;
} LessonSelectScene;

LessonSelectScene * _Nonnull LessonSelectSceneAlloc(void);

#endif /* lessonselectscene_h */
