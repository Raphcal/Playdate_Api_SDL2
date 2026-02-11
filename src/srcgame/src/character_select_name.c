//
//  character_select_name.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/08/2023.
//

#include "character_select_name.h"

#include "character_select_common.h"
#include "image.h"

typedef struct {
    MELSprite super;
    PlayerCharacter character;
    PlayerCharacter * _Nonnull selection;
    float time;
} CharacterSelectName;

static void updateSelected(LCDSprite * _Nonnull sprite);
static void updateUnselected(LCDSprite * _Nonnull sprite);
static void updateShow(LCDSprite * _Nonnull sprite);
static void updateHide(LCDSprite * _Nonnull sprite);

static MELPoint getOrigin(PlayerCharacter character, MELBoolean isSelected);

LCDSprite * _Nonnull CharacterSelectNameConstructor(PlayerCharacter character, PlayerCharacter * _Nonnull selection, LCDBitmap * _Nonnull image) {
    CharacterSelectName *self = playdate->system->realloc(NULL, sizeof(CharacterSelectName));
    const MELBoolean isSelected = *selection == character;
    LCDSprite *sprite = ImageConstructorWithSelf(&self->super, getOrigin(character, isSelected), image);
    self->super.class = &MELSpriteClassDefault;
    self->character = character;
    self->selection = selection;
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, isSelected ? updateSelected : updateUnselected);
    return sprite;
}

static MELPoint getOrigin(PlayerCharacter character, MELBoolean isSelected) {
    return (MELPoint) {
        .x = character == PlayerCharacterKatsuo ? 165.0f : 220.0f,
        .y = isSelected ? 60.0f : -60.0f
    };
}

static void draw(CharacterSelectName * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
}

static void updateSelected(LCDSprite * _Nonnull sprite) {
    CharacterSelectName *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != self->character) {
        playdate->sprite->setUpdateFunction(sprite, updateHide);
    }
    draw(self, sprite);
}

static void updateUnselected(LCDSprite * _Nonnull sprite) {
    CharacterSelectName *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection == self->character) {
        playdate->sprite->setUpdateFunction(sprite, updateShow);
    }
    draw(self, sprite);
}

static void updateShow(LCDSprite * _Nonnull sprite) {
    CharacterSelectName *self = playdate->sprite->getUserdata(sprite);
    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        self->super.frame.origin.y = -60 + 120 * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, updateSelected);

    draw(self, sprite);
}

static void updateHide(LCDSprite * _Nonnull sprite) {
    CharacterSelectName *self = playdate->sprite->getUserdata(sprite);
    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        self->super.frame.origin.y = 60 - 120 * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, updateUnselected);

    draw(self, sprite);
}
