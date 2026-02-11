//
//  character_select_grid.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/08/2023.
//

#include "character_select_grid.h"

#include "character_select_common.h"
#include "../gen/spritecharacterselectgrid.h"

typedef struct {
    MELSprite super;
    PlayerCharacter * _Nonnull selection;
    float time;
} CharacterSelectGrid;

static void changeToKatsuo(LCDSprite * _Nonnull sprite);
static void updateKatsuo(LCDSprite * _Nonnull sprite);

static void changeToSaki(LCDSprite * _Nonnull sprite);
static void updateSaki(LCDSprite * _Nonnull sprite);

static MELPoint getOrigin(MELBoolean isKatsuo);

static const float halfWidth = 60.0f;

LCDSprite * _Nonnull CharacterSelectGridConstructorWithSelection(PlayerCharacter * _Nonnull selection) {
    CharacterSelectGrid *self = playdate->system->realloc(NULL, sizeof(CharacterSelectGrid));
    const MELBoolean isKatsuo = *selection == PlayerCharacterKatsuo;
    loadSpriteCharacterSelectGridPalette();
    LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, &spriteCharacterSelectGrid, getOrigin(isKatsuo));
    self->selection = selection;
    self->time = 0;
    playdate->sprite->setUpdateFunction(sprite, isKatsuo ? updateKatsuo : updateSaki);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

LCDSprite * _Nonnull CharacterSelectGridConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    playdate->system->error("Method CharacterSelectGridConstructor is not supported");
    return playdate->sprite->newSprite();
}
MELSprite * _Nullable CharacterSelectGridLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    playdate->system->error("Method CharacterSelectGridLoader is not supported");
    return NULL;
}

static MELPoint getOrigin(MELBoolean isKatsuo) {
    return (MELPoint) {
        .x = isKatsuo ? LCD_COLUMNS - halfWidth : halfWidth,
        .y = LCD_ROWS / 2
    };
}

static void draw(CharacterSelectGrid * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimationUpdate(self->super.animation, DELTA);

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, self->super.animation->frame.atlasIndex), kBitmapUnflipped);
}

static void updateKatsuo(LCDSprite * _Nonnull sprite) {
    CharacterSelectGrid *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != PlayerCharacterKatsuo) {
        self->time = 0;
        MELSpriteSetAnimation(&self->super, AnimationNameDisappear);
        playdate->sprite->setUpdateFunction(sprite, changeToSaki);
    }
    draw(self, sprite);
}

static void updateSaki(LCDSprite * _Nonnull sprite) {
    CharacterSelectGrid *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != PlayerCharacterSaki) {
        self->time = 0;
        MELSpriteSetAnimation(&self->super, AnimationNameDisappear);
        playdate->sprite->setUpdateFunction(sprite, changeToKatsuo);
    }
    draw(self, sprite);
}

static void changeToSaki(LCDSprite * _Nonnull sprite) {
    CharacterSelectGrid *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = newTime / duration;

        if (progress >= 0.5f) {
            self->super.frame.origin.x = halfWidth;
            MELSpriteSetAnimation(&self->super, AnimationNameAppear);
        }

        draw(self, sprite);
        return;
    }
    playdate->sprite->setUpdateFunction(sprite, updateSaki);
    draw(self, sprite);
}

static void changeToKatsuo(LCDSprite * _Nonnull sprite) {
    CharacterSelectGrid *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = newTime / duration;

        if (progress >= 0.5f) {
            self->super.frame.origin.x = LCD_COLUMNS - halfWidth;
            MELSpriteSetAnimation(&self->super, AnimationNameAppear);
        }

        draw(self, sprite);
        return;
    }
    playdate->sprite->setUpdateFunction(sprite, updateKatsuo);
    draw(self, sprite);
}
