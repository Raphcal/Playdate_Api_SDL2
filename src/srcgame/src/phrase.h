//
//  phrase.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 23/03/2024.
//

#ifndef phrase_h
#define phrase_h

#include "common.h"
#include "../lib/melice.h"

#include "character.h"
#include "side.h"
#include "background.h"
#include "effect.h"
#include "../gen/animationnames.h"

typedef struct {
    Character character;
    Expression expression;
    Side side;
    Effect effect;
    AnimationName animation;
    /// Direction de l'animation +1, 0 si pas d'image.
    int8_t direction;
    MELLocalizedString line;
} Phrase;
MELListDefine(Phrase);

#endif /* phrase_h */
