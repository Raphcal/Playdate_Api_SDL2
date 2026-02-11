//
//  character_select_character.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/08/2023.
//

#include "character_select_character.h"

#include "character_select_common.h"
#include "image.h"

typedef struct {
    MELSprite super;
    PlayerCharacter character;
    PlayerCharacter * _Nonnull selection;
    float time;
} CharacterSelectCharacter;

static void updateSelected(LCDSprite * _Nonnull sprite);
static void updateUnselected(LCDSprite * _Nonnull sprite);
static void updateShow(LCDSprite * _Nonnull sprite);
static void updateHide(LCDSprite * _Nonnull sprite);

static MELPoint getOrigin(PlayerCharacter character, MELBoolean isSelected);

static const float hidden = 0.0f;
static const float visible = 60.0f;

LCDSprite * _Nonnull CharacterSelectCharacterConstructor(PlayerCharacter character, PlayerCharacter * _Nonnull selection, LCDBitmapTable * _Nonnull table) {
    CharacterSelectCharacter *self = playdate->system->realloc(NULL, sizeof(CharacterSelectCharacter));
    const MELBoolean isSelected = *selection == character;
    LCDSprite *sprite = ImageConstructorWithSelf(&self->super, getOrigin(character, isSelected), playdate->graphics->getTableBitmap(table, character));
    self->super.class = &MELSpriteClassDefault;
    self->character = character;
    self->selection = selection;
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, isSelected ? updateSelected : updateUnselected);
    return sprite;
}

static MELPoint getOrigin(PlayerCharacter character, MELBoolean isSelected) {
    switch (character * 2 + isSelected) {
        case 0:
            return (MELPoint) { .x = -hidden, .y = LCD_ROWS / 2.0f };
        case 1:
            return (MELPoint) { .x = visible, .y = LCD_ROWS / 2.0f };
        case 2:
            return (MELPoint) { .x = LCD_COLUMNS + hidden, .y = LCD_ROWS / 2.0f };
        case 3:
            return (MELPoint) { .x = LCD_COLUMNS - visible, .y = LCD_ROWS / 2.0f };
        default:
            // Erreur !
            return MELPointZero;
    }
}

static void draw(CharacterSelectCharacter * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
}

static void updateSelected(LCDSprite * _Nonnull sprite) {
    CharacterSelectCharacter *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != self->character) {
        playdate->sprite->setUpdateFunction(sprite, updateHide);
    }
    draw(self, sprite);
}

static void updateUnselected(LCDSprite * _Nonnull sprite) {
    CharacterSelectCharacter *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection == self->character) {
        playdate->sprite->setUpdateFunction(sprite, updateShow);
    }
    draw(self, sprite);
}

static void updateShow(LCDSprite * _Nonnull sprite) {
    CharacterSelectCharacter *self = playdate->sprite->getUserdata(sprite);
    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        if (self->character == PlayerCharacterKatsuo) {
            self->super.frame.origin.x = -hidden + (hidden + visible) * (progress * progress * progress);
        } else {
            self->super.frame.origin.x = LCD_COLUMNS + hidden - (hidden + visible) * (progress * progress * progress);
        }
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, updateSelected);

    draw(self, sprite);
}

static void updateHide(LCDSprite * _Nonnull sprite) {
    CharacterSelectCharacter *self = playdate->sprite->getUserdata(sprite);
    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        if (self->character == PlayerCharacterKatsuo) {
            self->super.frame.origin.x = visible - (hidden + visible) * progress;
        } else {
            self->super.frame.origin.x = LCD_COLUMNS - visible + (hidden + visible) * progress;
        }
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, updateUnselected);

    draw(self, sprite);
}
