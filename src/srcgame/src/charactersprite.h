//
//  charactersprite.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 07/01/2024.
//

#ifndef charactersprite_h
#define charactersprite_h

#include "common.h"
#include "../lib/melice.h"

#include "character.h"
#include "story.h"
#include "../gen/animationnames.h"

typedef struct {
    MELSprite super;
    LCDBitmap * _Nullable inactiveImage;
    Character character;
    PhraseList dialog;
    AnimationName animationName;
    MELAnimationDirection direction;
    Side side;
    MELBoolean isActive;
    float time;
    float start;
    float target;
} CharacterSprite;

LCDSprite * _Nonnull CharacterSpriteMake(MELSpriteInstance * _Nonnull instance, Side side, PhraseList dialog);
LCDSprite * _Nonnull CharacterSpriteMakeImage(MELSpriteInstance * _Nonnull instance, PhraseList dialog);

#endif /* charactersprite_h */
