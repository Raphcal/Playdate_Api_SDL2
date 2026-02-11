//
//  sinusoidal.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 13/02/2023.
//

#include "sinusoidal.h"

static const float kSpeed = 150.0f;
static const float kAmplitude = 32.0f;

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static void withdraw(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);

static const MELSpriteClass SinusoidalClass = (MELSpriteClass) {
    .destroy = MELSpriteDealloc,
    .save = save,
    .withdraw = withdraw,
};

LCDSprite * _Nonnull SinusoidalConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    LCDSprite *sprite = MELSpriteInit(self, definition, instance);
    self->class = &SinusoidalClass;
    self->frame.origin.x = self->instance->center.x + LCD_COLUMNS;
    self->hitPoints = ENEMY_SINUSOIDAL_HIT_POINTS;
    self->score = ENEMY_SINUSOIDAL_SCORE;
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setZIndex(sprite, ZINDEX_ENEMIES);
    return sprite;
}

MELSprite * _Nullable SinusoidalLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    *self = (MELSprite) {
        .class = &SinusoidalClass
    };
    playdate->sprite->setUpdateFunction(sprite, update);
    return self;
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    // Rien à sauvegarder.
}

static void withdraw(LCDSprite * _Nonnull sprite) {
    // Sans effet.
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    MELRectangle frame = self->frame;
    frame.origin.x -= DELTA * kSpeed;
    const float sinOriginX = sinf(frame.origin.x / 64.0f);
    frame.origin.y = self->instance->center.y + sinOriginX * kAmplitude;

    // TODO: Changer l'image d'animation en fonction du résultat de (sinOriginX + 1.0f) / 2.0f.

    if (frame.origin.x < -frame.size.width) {
        MELSpriteDealloc(sprite);
        return;
    }
    self->frame = frame;
    MELSpriteDraw(self, sprite);
}
