//
//  character.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#ifndef character_h
#define character_h

#include "common.h"

#include "actor.h"
#include "../gen/spritenames.h"
#include "../gen/animationnames.h"

#define CharacterCount 21

typedef enum {
    CharacterNone,
    CharacterCaroline,
    CharacterCeres,
    CharacterCrocoford,
    CharacterBaguetteMagique,
    CharacterPrince,
    CharacterCheval,
    CharacterClovis,
    CharacterBelleplume,
    CharacterSapique,
    CharacterClown,
    CharacterCrowd,
    CharacterOrelia,
    CharacterChataigne,
    CharacterPetitChaperonRouge,
    CharacterElJoker,
    CharacterGuide,
    CharacterHat,
    CharacterShadow,
    CharacterMelusine,
    CharacterPaguro,
} Character;

typedef enum {
    ExpressionNormal,
    ExpressionKirakira,
    ExpressionHappy,
    ExpressionNeed,
    ExpressionShy,
    ExpressionWat,
    ExpressionMeh,
    ExpressionAngry,
} Expression;

extern const MELLocalizedString CharacterName[CharacterCount];
extern const AnimationName CharacterAnimationName[CharacterCount];
extern const MELAnimationDirection CharacterAnimationDirection[CharacterCount];

#endif /* character_h */
