//
//  opponent_portait.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 05/09/2024.
//

#include "opponent_portait.h"

#include "../gen/spritesweat.h"

static unsigned int minimumDuration = 90;
static unsigned int variableDuration = 300;

typedef struct {
    Board * _Nullable board;
    float speed;
    unsigned int lastCreationTime;
} OpponentPortait;

static void dealloc(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void updateSweat(LCDSprite * _Nonnull sprite);

static void onGameEnd(void * _Nullable userdata, int _);

static const MELSpriteClass OpponentPortaitClass = (MELSpriteClass) {
    .destroy = dealloc,
};

void OpponentPortaitConfigure(MELSprite * _Nonnull self, LCDSprite * _Nonnull sprite, Board * _Nonnull board) {
    OpponentPortait *opponentPortait = new(OpponentPortait);
    *opponentPortait = (OpponentPortait) {
        .board = board,
    };
    self->userdata = opponentPortait;
    self->autoReleaseUserdata = true;
    self->class = &OpponentPortaitClass;
    playdate->sprite->setUpdateFunction(sprite, update);

    loadSpriteSweatPalette();
    MELEventBusAddListener(EventLeftSideLost, onGameEnd, sprite);
    MELEventBusAddListener(EventRightSideLost, onGameEnd, sprite);
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    MELEventBusRemoveListeners(sprite);
    MELSpriteDealloc(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSpriteUpdate(sprite);

    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    OpponentPortait *self = melSprite->userdata;

    Board *board = self->board;
    const float sadness  = MELProgress(BOARD_COLUMNS * BOARD_ROWS / 2, BOARD_COLUMNS * BOARD_ROWS, board->count);

    const unsigned int now = playdate->system->getCurrentTimeMilliseconds();
    if (sadness && now >= self->lastCreationTime + minimumDuration + (1.0f - sadness) * variableDuration) {
        self->lastCreationTime = now;
        const MELPoint origin = melSprite->frame.origin;
        MELSprite *sweat = new(MELSprite);
        LCDSprite *sweatSprite = MELSpriteInitWithCenter(sweat, &spriteSweat, (MELPoint) {
            .x = origin.x,
            .y = origin.y - 16.0f + MELRandomFloat(32.0f)
        });
        sweat->frame.size.height = -120.0f - MELRandomFloat(100.0f);
        playdate->sprite->setZIndex(sweatSprite, ZINDEX_PORTAITS + 1);
        playdate->sprite->setUpdateFunction(sweatSprite, updateSweat);
        LCDSpriteRefListPush(&currentScene->sprites, sweatSprite);
    }
}

static void updateSweat(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELAnimation *animation = self->animation;
    if (MELAnimationIsLastFrame(animation)) {
        self->class->destroy(sprite);
    } else {
        MELRectangle frame = self->frame;
        frame.size.height += DELTA * 600.0f;
        frame.origin.y += frame.size.height * DELTA;
        self->frame = frame;

        MELAnimationUpdate(animation, DELTA);

        MELPoint origin = self->frame.origin;
        playdate->sprite->moveTo(sprite, origin.x, origin.y);
        playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
    }
}

static void onGameEnd(void * _Nullable userdata, int _) {
    LCDSprite *sprite = userdata;
    playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
}
