//
//  camera.c
//  Roll
//
//  Created by Raphaël Calabro on 03/06/2022.
//

#include "camera.h"

#include "player.h"

Camera camera = (Camera) {
    .frame = {
        .size = {
            .width = LCD_COLUMNS,
            .height = LCD_ROWS
        }
    },
};

MELBoolean CameraIsVisible(Camera camera, MELSprite * _Nonnull sprite) {
    return MELHitboxCollidesWithRectangle(sprite->hitbox, (MELRectangle) {
        .origin = {
            .x = camera.frame.origin.x + LCD_COLUMNS / 2,
            .y = camera.frame.origin.y + LCD_ROWS / 2
        },
        .size = camera.frame.size
    });
}

#pragma mark - Tremblement de terre

static void shakeSave(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static MELSprite * _Nullable shakeLoad(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

typedef struct {
    MELSprite super;
    float time;
    float duration;
    float intensity;
    MELPoint oldTranslation;
} CameraShake;

static const MELSpriteClass CameraShakeClass = (MELSpriteClass) {
    .name = SpriteClassNameCameraShake,
    .destroy = MELSpriteDealloc,
    .save = shakeSave,
    .load = shakeLoad,
};

const MELSpriteClass * _Nonnull CameraShakeGetClass(void) {
    return &CameraShakeClass;
}

static void updateShaking(LCDSprite * _Nonnull sprite) {
    CameraShake *self = playdate->sprite->getUserdata(sprite);
    const float time = self->time += DELTA;
    const float progress = MELEaseInOut(0, self->duration, time);
    const float intensity = (1.0f - progress) * self->intensity;
    MELPoint oldTranslation = self->oldTranslation;
    MELPoint translation = (MELPoint) {
        .x = MELRandomFloat(intensity) - intensity / 2.0f,
        .y = MELRandomFloat(intensity) - intensity / 2.0f
    };
    self->oldTranslation = translation;
    camera.frame.origin.x += translation.x - oldTranslation.x;
    camera.frame.origin.y += translation.y - oldTranslation.y;
    if (progress == 1.0f) {
        MELSpriteDealloc(sprite);
    }
}

LCDSprite * _Nonnull CameraShakeConstructor(float duration, float intensity) {
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, updateShaking);
    playdate->sprite->setDrawFunction(sprite, MELSpriteNoopDraw);
    playdate->sprite->setVisible(sprite, false);
    playdate->sprite->addSprite(sprite);

    CameraShake *self = playdate->system->realloc(NULL, sizeof(CameraShake));
    *self = (CameraShake) {
        .super = {
            .class = &MELSpriteClassDefault,
        },
        .duration = duration,
        .intensity = intensity,
    };
    playdate->sprite->setUserdata(sprite, self);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push CameraShake(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void shakeSave(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    CameraShake *self = (CameraShake *)sprite;
    MELOutputStreamWriteFloat(outputStream, self->time);
    MELOutputStreamWriteFloat(outputStream, self->duration);
    MELOutputStreamWriteFloat(outputStream, self->intensity);
    MELOutputStreamWritePoint(outputStream, self->oldTranslation);
}

static MELSprite * _Nullable shakeLoad(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    playdate->sprite->setUpdateFunction(sprite, updateShaking);

    const float time = MELInputStreamReadFloat(inputStream);
    const float duration = MELInputStreamReadFloat(inputStream);
    const float intensity = MELInputStreamReadFloat(inputStream);
    const MELPoint oldTranslation = MELInputStreamReadPoint(inputStream);

    CameraShake *self = playdate->system->realloc(NULL, sizeof(CameraShake));
    *self = (CameraShake) {
        .super = {
            .class = &CameraShakeClass,
        },
        .time = time,
        .duration = duration,
        .intensity = intensity,
        .oldTranslation = oldTranslation,
    };
    return &self->super;
}
