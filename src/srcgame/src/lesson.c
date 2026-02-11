//
//  lesson.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 23/03/2024.
//

#include "lesson.h"

MELListImplement(Lesson);

MELBoolean LessonObjectivesAreEmpty(LessonObjectives self) {
    return !self.chipsToSend
    && !self.cardsToRemove
    && !self.chipsToRemove
    && !self.comboToDo;
}
