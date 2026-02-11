//
//  new_best.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 05/12/2023.
//

#include "new_best.h"

#include "gamescene.h"
#include "../gen/spritenames.h"
#include "../gen/spritenewbest.h"

struct new_best {
    MELSprite super;

    uint8_t statePart;
    float time;
};

static void mainStatePart0(LCDSprite * _Nonnull sprite);
static void mainStatePart1(LCDSprite * _Nonnull sprite);
static void mainStatePart2(LCDSprite * _Nonnull sprite);
static void mainStatePart3(LCDSprite * _Nonnull sprite);

static void goToCurrentState(struct new_best * _Nonnull self, LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);

static const MELSpriteClass NewBestClass = (MELSpriteClass) {
    .destroy = MELSpriteDeallocFromGameSceneOthers,
    .save = save,
    .load = NewBestLoader,
};

LCDSprite * _Nonnull NewBestConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    struct new_best *self = playdate->system->realloc(NULL, sizeof(struct new_best));
    LCDSprite *sprite = MELSpriteInit(&self->super, definition, instance);
    self->super.class = &NewBestClass;
    self->statePart = 0;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart0);
    return sprite;
}

LCDSprite * _Nonnull NewBestConstructorWithMessage(NewBestMessage message) {
    struct new_best *self = playdate->system->realloc(NULL, sizeof(struct new_best));
    loadSpriteNewBestPalette();
    LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, &spriteNewBest, (MELPoint) {
        .x = 475.5f,
        .y = 20.0f,
    });
    self->super.class = &NewBestClass;
    self->statePart = 0;
    MELSpriteSetAnimation(&self->super, (AnimationName)message);
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart0);
    playdate->sprite->setZIndex(sprite, ZINDEX_HISCORE);

    GameScene *gameScene = GameSceneGetCurrent();
    LCDSpriteRefListPush(&gameScene->others, sprite);
    return sprite;
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    struct new_best *self = (struct new_best *)sprite;
    MELOutputStreamWriteByte(outputStream, self->statePart);
    MELOutputStreamWriteFloat(outputStream, self->time);
}

MELSprite * _Nullable NewBestLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    struct new_best *self = playdate->system->realloc(NULL, sizeof(struct new_best));
    const uint8_t statePart = MELInputStreamReadByte(inputStream);
    const float time = MELInputStreamReadFloat(inputStream);

    *self = (struct new_best) {
        .super = {
            .class = &NewBestClass
        },
        .statePart = statePart,
        .time = time,
    };
    goToCurrentState(self, sprite);
    return &self->super;
}

static void goToCurrentState(struct new_best * _Nonnull self, LCDSprite * _Nonnull sprite) {
    switch (self->statePart) {
        case 1:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart1);
            break;
        case 2:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart2);
            break;
        case 3:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart3);
            break;
        default:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart0);
            break;
    }
}

static void draw(struct new_best * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimationUpdate(self->super.animation, DELTA);

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, self->super.animation->frame.atlasIndex), MELDirectionFlip[self->super.direction]);
}

static void mainStatePart0(LCDSprite * _Nonnull sprite) {
    struct new_best *self = playdate->sprite->getUserdata(sprite);

    // during
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart1);

    draw(self, sprite);
}

static void mainStatePart1(LCDSprite * _Nonnull sprite) {
    struct new_best *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = 0.3f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.x = 475.5f + -265.5f * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    self->statePart = 2;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart2);

    draw(self, sprite);
}

static void mainStatePart2(LCDSprite * _Nonnull sprite) {
    struct new_best *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = 4.0f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.x = 210 + -20 * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    self->statePart = 3;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart3);

    draw(self, sprite);
}

static void mainStatePart3(LCDSprite * _Nonnull sprite) {
    struct new_best *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = 0.3f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.x = 190 + -265.5f * progress;
        draw(self, sprite);
        return;
    }
    self->super.class->destroy(sprite);
}
