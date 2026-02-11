//
//  lesson.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 23/03/2024.
//

#ifndef lesson_h
#define lesson_h

#include "common.h"
#include "phrase.h"

typedef struct {
    const char * _Nullable board;
    const char * _Nullable deck;
    uint8_t boardCount;
    uint8_t deckCount;
    uint8_t chips;
    uint8_t chipsToSend;
    uint8_t chipsToRemove;
    uint8_t cardsToRemove;
    uint8_t comboToDo;
    uint8_t victoriesToUnlock;
} LessonObjectives;

typedef struct {
    MELLocalizedString title;
    LessonObjectives objectives;
    PhraseList dialog;
} Lesson;
MELListDefine(Lesson);

MELBoolean LessonObjectivesAreEmpty(LessonObjectives self);

#endif /* lesson_h */
