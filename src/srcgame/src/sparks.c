#include "sparks.h"

#include "../gen/spritesparks.h"

typedef struct {
    MELSprite super;
    MELShootingStyleDefinition sparksDefinition;
    MELShootingStyle style;
    float time;
} Sparks;

static const float kDuration = 1.0f;

static void update(LCDSprite * _Nonnull sprite);

LCDSprite * _Nullable SparksConstructor(MELRectangle frame) {
    Sparks *self = new(Sparks);
    *self = (Sparks) {
        .super = {
            .class = &MELSpriteClassDefault,
            .frame = frame,
        },
        .sparksDefinition = (MELShootingStyleDefinition) {
            .bulletDefinition = &spriteSparks,
            .origin = MELShotOriginCenter,
            .shootInterval = 0.05f,
            .bulletAmount = 1,
            .space = (frame.size.width + frame.size.height) / 2.0f,
        }
    };
    loadSpriteSparksPalette();
    ParticuleShootingStyleInit(&self->style, &self->sparksDefinition);

    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->setUserdata(sprite, self);
    spriteAPI->setUpdateFunction(sprite, update);
    spriteAPI->addSprite(sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void update(LCDSprite * _Nonnull sprite) {
    Sparks *self = playdate->sprite->getUserdata(sprite);

    const float time = self->time;
    if (time < kDuration) {
        self->time = time + DELTA;
        MELShootingStyleShootFromSprite(&self->style, &self->super, 0.0f);
        return;
    }

    MELSpriteDealloc(sprite);
}
