//
//  chain_multiplier.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 06/12/2023.
//

#include "chain_multiplier.h"

#include "text.h"
#include "gamescene.h"

static void update(LCDSprite * _Nonnull sprite);
static void dealloc(LCDSprite * _Nonnull sprite);

static const MELSpriteClass ChainMultiplierClass = (MELSpriteClass) {
    .destroy = dealloc,
};

LCDSprite * _Nonnull ChainMultiplierConstructor(MELPoint origin, int value) {
    const int capacity = 20;
    char buffer[capacity + 1];
    buffer[0] = 'x'; 
    MELUInt32ToStringWithFixedSizeBuffer(value, buffer + 1, capacity);
    LCDSprite *sprite = TextConstructorDontPush(origin, numbersFont, kDrawModeCopy, buffer, capacity);
    playdate->sprite->setZIndex(sprite, ZINDEX_CHAIN);
    playdate->sprite->setUpdateFunction(sprite, update);

    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->class = &ChainMultiplierClass;

    GameScene *gameScene = GameSceneGetCurrent();
    LCDSpriteRefListPush(&gameScene->others, sprite);
    return sprite;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    LCDBitmap *bitmap = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(bitmap);
    MELSpriteDeallocFromGameSceneOthers(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float duration = 1.0f;
    if (self->hitTimer < duration) {
        const float newTime = MELFloatMin(self->hitTimer + DELTA, duration);
        const float progress = MELEaseOutCirc(0, duration, newTime);
        self->hitTimer = newTime;

        MELPoint origin = self->frame.origin;
        playdate->sprite->moveTo(sprite, origin.x, origin.y - 16 * progress);
        return;
    }

    self->class->destroy(sprite);
}
