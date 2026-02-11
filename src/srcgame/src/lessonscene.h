//
//  lessonscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 21/02/2024.
//

#ifndef lessonscene_h
#define lessonscene_h

#include "../lib/melice.h"
#include "gameflow.h"
#include "../gen/maps.h"
#include "story.h"
#include "lesson.h"

typedef struct {
    MELScene super;
    uint32_t lessonIndex;
    MELMap * _Nullable background;
    MapName backgroundName;
    CardList cards;
    GameFlow gameFlow;
    float time;
    float speed;
    LessonObjectives progress;
    LCDSprite * _Nullable cardsToRemoveLabel;
    LCDSprite * _Nullable chipsToRemoveLabel;
    LCDSprite * _Nullable chipsToSendLabel;
    LCDSprite * _Nullable comboToDoLabel;
} LessonScene;

LessonScene * _Nonnull LessonSceneAlloc(uint32_t lessonIndex);

#endif /* lessonscene_h */
