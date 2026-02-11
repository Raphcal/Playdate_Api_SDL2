//
//  character_select_robot.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/08/2023.
//

#include "character_select_robot.h"

#include "character_select_common.h"
#include "../gen/spritehero.h"

typedef struct {
    MELSprite super;
    PlayerCharacter * _Nonnull selection;
    float time;
} CharacterSelectRobot;

static void changeToKatsuo(LCDSprite * _Nonnull sprite);
static void updateKatsuo(LCDSprite * _Nonnull sprite);

static void changeToSaki(LCDSprite * _Nonnull sprite);
static void updateSaki(LCDSprite * _Nonnull sprite);

static MELPoint getOrigin(MELBoolean isKatsuo);

static const float katsuoX = 216.0f;
static const float sakiX = 120.0f;

LCDSprite * _Nonnull CharacterSelectRobotConstructor(PlayerCharacter * _Nonnull selection) {
    CharacterSelectRobot *self = playdate->system->realloc(NULL, sizeof(CharacterSelectRobot));
    const MELBoolean isKatsuo = *selection == PlayerCharacterKatsuo;
    loadSpriteHeroPalette();
    LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, &spriteHero, getOrigin(isKatsuo));
    self->selection = selection;
    self->time = 0;
    if (isKatsuo) {
        MELSpriteSetAnimation(&self->super, AnimationNameRun);
        playdate->sprite->setUpdateFunction(sprite, updateKatsuo);
    } else {
        MELSpriteSetAnimation(&self->super, AnimationNameSkid);
        playdate->sprite->setUpdateFunction(sprite, updateSaki);
    }
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static MELPoint getOrigin(MELBoolean isKatsuo) {
    return (MELPoint) {
        .x = isKatsuo ? katsuoX : sakiX,
        .y = LCD_ROWS / 2.0f
    };
}

static void draw(CharacterSelectRobot * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimationUpdate(self->super.animation, DELTA);

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, self->super.animation->frame.atlasIndex), kBitmapUnflipped);
}

static void updateKatsuo(LCDSprite * _Nonnull sprite) {
    CharacterSelectRobot *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != PlayerCharacterKatsuo) {
        self->time = 0;
        playdate->sprite->setUpdateFunction(sprite, changeToSaki);
    }
    draw(self, sprite);
}

static void updateSaki(LCDSprite * _Nonnull sprite) {
    CharacterSelectRobot *self = playdate->sprite->getUserdata(sprite);
    if (*self->selection != PlayerCharacterSaki) {
        self->time = 0;
        playdate->sprite->setUpdateFunction(sprite, changeToKatsuo);
    }
    draw(self, sprite);
}

static void changeToSaki(LCDSprite * _Nonnull sprite) {
    CharacterSelectRobot *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.x = katsuoX + (sakiX - katsuoX) * progress;

        if (progress >= 0.5f) {
            MELSpriteSetAnimation(&self->super, AnimationNameSkid);
        }

        draw(self, sprite);
        return;
    }
    playdate->sprite->setUpdateFunction(sprite, updateSaki);
    draw(self, sprite);
}

static void changeToKatsuo(LCDSprite * _Nonnull sprite) {
    CharacterSelectRobot *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = kAnimationDuration;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.x = sakiX + (katsuoX - sakiX) * progress;

        if (progress >= 0.5f) {
            MELSpriteSetAnimation(&self->super, AnimationNameRun);
        }

        draw(self, sprite);
        return;
    }
    playdate->sprite->setUpdateFunction(sprite, updateKatsuo);
    draw(self, sprite);
}
