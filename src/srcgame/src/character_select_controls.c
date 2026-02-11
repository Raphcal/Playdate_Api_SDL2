//
//  character_select_controls.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/02/2024.
//

#include "character_select_controls.h"

#include "character_select_common.h"
#include "image.h"

typedef struct {
    MELSprite super;
    PlayerCharacter character;
    PlayerCharacter * _Nonnull selection;
    float time;
} CharacterSelectControls;

static void updateSelected(LCDSprite * _Nonnull sprite);
static void updateUnselected(LCDSprite * _Nonnull sprite);
static void updateShow(LCDSprite * _Nonnull sprite);
static void updateHide(LCDSprite * _Nonnull sprite);

static MELPoint getOrigin(PlayerCharacter character, MELBoolean isSelected);

LCDSprite * _Nonnull CharacterSelectControlsConstructor(PlayerCharacter character, PlayerCharacter * _Nonnull selection, LCDBitmap * _Nonnull image) {
    CharacterSelectControls *self = playdate->system->realloc(NULL, sizeof(CharacterSelectControls));
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
    const float marginX = 32.0f;
    const float marginY = 16.0f;
    const float halfWidth = 93.0f;
    const float halfHeight = 38.0f;
    return (MELPoint) {
        .x = character == PlayerCharacterKatsuo ? LCD_COLUMNS - halfWidth - marginX - marginX : marginX + marginX + halfWidth,
        .y = isSelected ? LCD_ROWS - halfHeight - marginY : LCD_ROWS + halfHeight
    };
}

static void draw(CharacterSelectControls * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
}

static void updateSelected(LCDSprite * _Nonnull sprite) {
    CharacterSelectControls *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != self->character) {
        playdate->sprite->setUpdateFunction(sprite, updateHide);
    }
    draw(self, sprite);
}

static void updateUnselected(LCDSprite * _Nonnull sprite) {
    CharacterSelectControls *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection == self->character) {
        playdate->sprite->setUpdateFunction(sprite, updateShow);
    }
    draw(self, sprite);
}

static void updateShow(LCDSprite * _Nonnull sprite) {
    CharacterSelectControls *self = playdate->sprite->getUserdata(sprite);
    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        const float from = getOrigin(PlayerCharacterKatsuo, false).y;
        const float to = getOrigin(PlayerCharacterKatsuo, true).y;
        self->super.frame.origin.y = from + (to - from) * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, updateSelected);

    draw(self, sprite);
}

static void updateHide(LCDSprite * _Nonnull sprite) {
    CharacterSelectControls *self = playdate->sprite->getUserdata(sprite);
    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        const float from = getOrigin(PlayerCharacterKatsuo, true).y;
        const float to = getOrigin(PlayerCharacterKatsuo, false).y;
        self->super.frame.origin.y = from + (to - from) * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, updateUnselected);

    draw(self, sprite);
}
