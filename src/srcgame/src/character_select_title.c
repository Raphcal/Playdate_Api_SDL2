//
//  character_select_title.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/08/2023.
//

#include "character_select_title.h"

#include "character_select_common.h"
#include "image.h"

typedef struct {
    MELSprite super;
    PlayerCharacter * _Nonnull selection;
    float time;
} CharacterSelectTitle;

static void updateKatsuo(LCDSprite * _Nonnull sprite);
static void updateSaki(LCDSprite * _Nonnull sprite);

static void changeToSakiStatePart0(LCDSprite * _Nonnull sprite);
static void changeToSakiStatePart1(LCDSprite * _Nonnull sprite);
static void changeToSakiStatePart2(LCDSprite * _Nonnull sprite);

static void changeToKatsuoStatePart0(LCDSprite * _Nonnull sprite);
static void changeToKatsuoStatePart1(LCDSprite * _Nonnull sprite);
static void changeToKatsuoStatePart2(LCDSprite * _Nonnull sprite);

static MELPoint getOrigin(MELBoolean isKatsuo);

LCDSprite * _Nonnull CharacterSelectTitleConstructor(PlayerCharacter * _Nonnull selection) {
    CharacterSelectTitle *self = playdate->system->realloc(NULL, sizeof(CharacterSelectTitle));
    const MELBoolean isKatsuo = *selection == PlayerCharacterKatsuo;
    LCDSprite *sprite = ImageConstructorWithSelf(&self->super, getOrigin(isKatsuo), LCDBitmapLoadOrError("sprite-character_select_title"));
    self->selection = selection;
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, isKatsuo ? updateKatsuo : updateSaki);
    return sprite;
}

static MELPoint getOrigin(MELBoolean isKatsuo) {
    return (MELPoint) {
        .x = isKatsuo ? LCD_COLUMNS - 40.0f : 40.0f,
        .y = 100.0f
    };
}

static void draw(CharacterSelectTitle * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
}

static void updateKatsuo(LCDSprite * _Nonnull sprite) {
    CharacterSelectTitle *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != PlayerCharacterKatsuo) {
        playdate->sprite->setUpdateFunction(sprite, changeToSakiStatePart0);
    }
    draw(self, sprite);
}

static void updateSaki(LCDSprite * _Nonnull sprite) {
    CharacterSelectTitle *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != PlayerCharacterSaki) {
        playdate->sprite->setUpdateFunction(sprite, changeToKatsuoStatePart0);
    }
    draw(self, sprite);
}

static void changeToSakiStatePart0(LCDSprite * _Nonnull sprite) {
    CharacterSelectTitle *self = playdate->sprite->getUserdata(sprite);

    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, &changeToSakiStatePart1);

    draw(self, sprite);
}

static void changeToSakiStatePart1(LCDSprite * _Nonnull sprite) {
    CharacterSelectTitle *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = kHalfAnimationDuration;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.y = 100.0f - 200.0f * progress;
        draw(self, sprite);
        return;
    }
    self->super.frame.origin.x = 40.0f;
    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, &changeToSakiStatePart2);

    draw(self, sprite);
}

static void changeToSakiStatePart2(LCDSprite * _Nonnull sprite) {
    CharacterSelectTitle *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = kHalfAnimationDuration;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.y = -100.0f + 200.0f * progress;
        draw(self, sprite);
        return;
    }
    playdate->sprite->setUpdateFunction(sprite, updateSaki);
    draw(self, sprite);
}


static void changeToKatsuoStatePart0(LCDSprite * _Nonnull sprite) {
    CharacterSelectTitle *self = playdate->sprite->getUserdata(sprite);

    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, &changeToKatsuoStatePart1);

    draw(self, sprite);
}

static void changeToKatsuoStatePart1(LCDSprite * _Nonnull sprite) {
    CharacterSelectTitle *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = kHalfAnimationDuration;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.y = 100.0f - 200.0f * progress;
        draw(self, sprite);
        return;
    }
    self->super.frame.origin.x = LCD_COLUMNS - 40.0f;
    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, &changeToKatsuoStatePart2);

    draw(self, sprite);
}

static void changeToKatsuoStatePart2(LCDSprite * _Nonnull sprite) {
    CharacterSelectTitle *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = kHalfAnimationDuration;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.y = -100.0f + 200.0f * progress;
        draw(self, sprite);
        return;
    }
    playdate->sprite->setUpdateFunction(sprite, updateKatsuo);
    draw(self, sprite);
}
