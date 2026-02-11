//
//  gameover.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 21/07/2023.
//

#include "gameover.h"

#include "../gen/spritegameover.h"


struct gameover {
    MELSprite super;

    float time;
    uint8_t statePart;
};

static void mainStatePart0(LCDSprite * _Nonnull sprite);
static void mainStatePart1(LCDSprite * _Nonnull sprite);
static void mainStatePart2(LCDSprite * _Nonnull sprite);

static void goToCurrentState(struct gameover * _Nonnull self, LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);

static const MELSpriteClass GameoverClass = (MELSpriteClass) {
    .destroy = MELSpriteDealloc,
    .save = save,
};

LCDSprite * _Nonnull GameoverConstructorNoArgs(void) {
    struct gameover *self = playdate->system->realloc(NULL, sizeof(struct gameover));
    loadSpriteGameoverPalette();
    LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, &spriteGameover, MELPointMake(200, 100));
    self->super.class = &GameoverClass;
    self->statePart = 0;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart0);
    playdate->sprite->setZIndex(sprite, ZINDEX_GUI);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push GameOver(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

LCDSprite * _Nonnull GameoverConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    struct gameover *self = playdate->system->realloc(NULL, sizeof(struct gameover));
    LCDSprite *sprite = MELSpriteInit(&self->super, definition, instance);
    self->super.class = &GameoverClass;
    self->super.frame.origin.x = 200;
    self->super.frame.origin.y = 100;
    self->statePart = 0;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart0);
    playdate->sprite->setZIndex(sprite, ZINDEX_GUI);
    return sprite;
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    struct gameover *self = (struct gameover *)sprite;
    MELOutputStreamWriteByte(outputStream, self->statePart);
    MELOutputStreamWriteFloat(outputStream, self->time);
}

MELSprite * _Nullable GameoverLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    struct gameover *self = playdate->system->realloc(NULL, sizeof(struct gameover));
    const uint8_t statePart = MELInputStreamReadByte(inputStream);
    const float time = MELInputStreamReadFloat(inputStream);

    *self = (struct gameover) {
        .super = {
            .class = &GameoverClass
        },
        .statePart = statePart,
        .time = time,
    };
    goToCurrentState(self, sprite);
    return &self->super;
}

static void goToCurrentState(struct gameover * _Nonnull self, LCDSprite * _Nonnull sprite) {
    switch (self->statePart) {
        case 1:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart1);
            break;
        case 2:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart2);
            break;
        default:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart0);
            break;
    }
}

static void draw(struct gameover * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimationUpdate(self->super.animation, DELTA);

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, self->super.animation->frame.atlasIndex), MELDirectionFlip[self->super.direction]);
}

static void mainStatePart0(LCDSprite * _Nonnull sprite) {
    struct gameover *self = playdate->sprite->getUserdata(sprite);

    // during
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart1);

    draw(self, sprite);
}

static void mainStatePart1(LCDSprite * _Nonnull sprite) {
    struct gameover *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = 1.0f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin.y = 100 + 20 * progress;
        draw(self, sprite);
        return;
    }
    // while
    self->statePart = 2;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart2);
    mainStatePart2(sprite);
}

static void mainStatePart2(LCDSprite * _Nonnull sprite) {
    struct gameover *self = playdate->sprite->getUserdata(sprite);

    // while
    // wait
    draw(self, sprite);
}

