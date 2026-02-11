//
//  dialogscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#ifndef dialogscene_h
#define dialogscene_h

#include "common.h"

#include "story.h"
#include "lesson.h"

typedef struct {
    MELScene super;
    MELMap * _Nullable background;
    MapName backgroundName;
    const Lesson * _Nullable lesson;
    const Story * _Nullable story;
    unsigned int encounterIndex;
    PhraseList dialog;
    Background dialogBackground;
    MELCharList text;
    MELBoolean goToGameScene;
} DialogScene;

DialogScene * _Nonnull DialogSceneAlloc(const Story * _Nullable story, unsigned int encounterIndex);
DialogScene * _Nonnull DialogSceneAllocWithLesson(const Lesson * _Nullable lesson);

#endif /* dialogscene_h */
