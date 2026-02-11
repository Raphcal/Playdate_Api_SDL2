//
//  cardsprite.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 19/12/2023.
//

#include "cardsprite.h"

#include "gamescene.h"
#include "samples.h"
#include "scenes.h"
#include "../gen/spritecards18.h"
#include "../gen/spritechip18.h"

static void dealloc(LCDSprite * _Nonnull sprite);

static void updateMain(LCDSprite * _Nonnull sprite);
static void updateExtra(LCDSprite * _Nonnull sprite);
static void updateLateralMove(LCDSprite * _Nonnull sprite);
static void updateRotation(LCDSprite * _Nonnull sprite);
static void updateBlinking(LCDSprite * _Nonnull sprite);
static void updateFalling(LCDSprite * _Nonnull sprite);
static void updateBouncing(LCDSprite * _Nonnull sprite);
static void updateNoop(LCDSprite * _Nonnull sprite);

static MELBoolean isAboveSomething(CardSprite * _Nonnull self);
static void onOtherSideLost(void * _Nonnull userdata, int _);

static const MELSpriteClass CardSpriteClass = (MELSpriteClass) {
    .destroy = dealloc,
};

static void dealloc(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    LCDSpriteRefListDeinit(&self->tail);
    MELEventBusRemoveListeners(sprite);
    MELSpriteDealloc(sprite);
}

#pragma mark - Constructeurs

static LCDSprite * _Nonnull make(CardSpriteState state, Card card, MELPoint origin, Board * _Nonnull board) {
    CardSprite *self = playdate->system->realloc(NULL, sizeof(CardSprite));
    *self = (CardSprite) {
        .super = {
            .class = &CardSpriteClass,
            .definition = spriteCards18,
            .frame = {
                .size = {
                    .width = TILE_SIZE,
                    .height = TILE_SIZE
                },
                .origin = origin,
            },
        },
        .card = card,
        .board = board,
        .state = state,
        .direction = MELDirectionUp,
    };
    const MELPoint camera = board->camera;
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, CardGetBitmap(card), kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->moveTo(sprite, origin.x + camera.x, origin.y + camera.y);
    playdate->sprite->setZIndex(sprite, ZINDEX_CARDS);
    playdate->sprite->addSprite(sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    MELEventBusAddListener(SideLostEvent[SideOpposite[board->side]], onOtherSideLost, sprite);
    return sprite;
}

LCDSprite * _Nonnull CardSpriteMakeMain(Card card, Board * _Nonnull board) {
    const MELPoint origin = (MELPoint) {
        .x = board->super.frame.origin.x + TILE_SIZE * 2 + TILE_SIZE / 2,
        .y = board->super.frame.origin.y - TILE_SIZE / 2,
    };
    LCDSprite *sprite = make(CardSpriteStateMain, card, origin, board);
    playdate->sprite->setUpdateFunction(sprite, updateMain);
    return sprite;
}

LCDSprite * _Nonnull CardSpriteMakeExtra(Card card, Board * _Nonnull board) {
    const MELPoint origin = (MELPoint) {
        .x = board->super.frame.origin.x + TILE_SIZE * 2 + TILE_SIZE / 2,
        .y = board->super.frame.origin.y - TILE_SIZE - TILE_SIZE / 2,
    };
    LCDSprite *sprite = make(CardSpriteStateExtra, card, origin, board);
    playdate->sprite->setUpdateFunction(sprite, updateExtra);
    return sprite;
}

LCDSprite * _Nonnull CardSpriteMakeChip(Board * _Nonnull board, uint8_t column, uint8_t tailIndex) {
    loadSpriteChip18Palette();
    MELPoint boardTopLeft = board->super.frame.origin;
    MELPoint origin = (MELPoint) {
        .x = boardTopLeft.x + column * TILE_SIZE  + TILE_SIZE / 2,
        .y = boardTopLeft.y - tailIndex * TILE_SIZE - TILE_SIZE / 2
    };
    CardSprite *self = playdate->system->realloc(NULL, sizeof(CardSprite));
    *self = (CardSprite) {
        .super = {
            .class = &CardSpriteClass,
            .definition = spriteChip18,
            .frame = {
                .size = {
                    .width = TILE_SIZE,
                    .height = TILE_SIZE
                },
                .origin = origin,
            },
        },
        .board = board,
        .state = CardSpriteStateNoop,
    };
    MELSpriteSetAnimation(&self->super, AnimationNameWalk);
    MELAnimationSetFrameIndex(self->super.animation, MELRandomInt(self->super.animation->definition->frameCount));
    const MELPoint camera = board->camera;
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(spriteChip18.palette, 0), kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->moveTo(sprite, origin.x + camera.x, origin.y + camera.y);
    playdate->sprite->setZIndex(sprite, ZINDEX_CARDS);
    playdate->sprite->setUpdateFunction(sprite, updateNoop);
    playdate->sprite->addSprite(sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

void CardSpriteSetController(LCDSprite * _Nonnull sprite, MELController * _Nonnull controller) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    self->controller = controller;
}

void CardSpriteSetLinkedSprite(LCDSprite * _Nonnull sprite, LCDSprite * _Nonnull linkedSprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    CardSprite *other = playdate->sprite->getUserdata(linkedSprite);
#if CHECK_CLASS_CAST
    if (other->super.class != &CardSpriteClass) {
        playdate->system->error("Unable to link sprite. Given sprite is not an instance of CardSprite");
    }
#endif
    self->linkedSprite = other;
}

void CardSpriteLockDelay(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    const uint32_t now = playdate->system->getCurrentTimeMilliseconds();
    self->lockDelay = now;
    self->linkedSprite->lockDelay = now;
}

void CardSpriteStop(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    self->state = CardSpriteStateNoop;
    playdate->sprite->setUpdateFunction(sprite, updateNoop);

    if (!MELPointEquals(self->moveTarget, MELPointZero)) {
#if LOG_MOVES
        playdate->system->logToConsole("moveTarget was not zero: (x: %f, y: %f)", (double)self->moveTarget.x, (double)self->moveTarget.y);
#endif
        self->moveTarget = MELPointZero;
    }

    MELEventBusRemoveListener(SideLostEvent[SideOpposite[self->board->side]], sprite);
}

void CardSpriteFall(LCDSprite * _Nonnull sprite, LCDSpriteRefList tail, float initialSpeed, float waitDuration) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    self->state = CardSpriteStateFalling;
    if (self->tail.capacity) {
        LCDSpriteRefListDeinit(&self->tail);
    }
    self->tail = tail;
    self->time = waitDuration;
    self->moveTime = initialSpeed;
    if (!MELPointEquals(self->moveTarget, MELPointZero)) {
        self->super.frame.origin = self->moveTarget;
        self->moveTarget = MELPointZero;
    }
    playdate->sprite->setUpdateFunction(sprite, updateFalling);
    MELEventBusRemoveListener(SideLostEvent[SideOpposite[self->board->side]], sprite);
}

void CardSpriteBlink(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    self->time = 0.0f;
    self->state = CardSpriteStateBlinking;
    playdate->sprite->setUpdateFunction(sprite, updateBlinking);
}

void CardSpriteBounce(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    const MELRectangle boardFrame = self->board->super.frame;
    const float x = self->super.frame.origin.x;
    self->time = 64.0f * (x - (BOARD_WIDTH / 2 + boardFrame.origin.x)) / BOARD_WIDTH;
    self->moveTime = -300.0f;
    self->state = CardSpriteStateBouncing;
    playdate->sprite->setUpdateFunction(sprite, updateBouncing);
    playdate->sprite->setZIndex(sprite, ZINDEX_EXPLOSIONS);
}

void CardSpriteChangeToChip(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    if (self->super.definition.name == SpriteNameCards18) {
        loadSpriteChip18Palette();
        self->super.definition = spriteChip18;
        playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(spriteChip18.palette, 0), kBitmapUnflipped);
    }
}

void CardSpriteRemoveMainAndExtra(void) {
    LCDSpriteRefList sprites = currentScene->sprites;
    for (int index = sprites.count - 1; index >= 0; index--) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->class == &CardSpriteClass) {
            CardSprite *cardSprite = (CardSprite *) melSprite;
            const CardSpriteState state = cardSprite->state;
            if (state == CardSpriteStateMain || state == CardSpriteStateExtra) {
                cardSprite->super.class->destroy(sprite);
            }
        }
    }
}

MELBoolean CardSpriteIsCard(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    return self->definition.name == SpriteNameCards18;
}

MELBoolean CardSpriteHasLanded(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    const MELBoolean oldLanded = self->landed;
    self->landed = true;
    return oldLanded;
}

#pragma mark - Méthodes privées

static void tryLateralMove(CardSprite * _Nonnull self, LCDSprite * _Nonnull sprite, MELDirection direction) {
    const float distance = TILE_SIZE * MELDirectionValues[direction];
    const MELPoint mainOrigin = self->super.frame.origin;
    const MELPoint extraOrigin = MELPointEquals(self->linkedSprite->moveTarget, MELPointZero)
        ? self->linkedSprite->super.frame.origin
        : self->linkedSprite->moveTarget;

#if LOG_MOVES
    playdate->system->logToConsole("Try lateral move, main(x: %f, y: %f), extra(x: %f, y: %f), extraTarget(x: %f, y: %f)", (double)mainOrigin.x, (double)mainOrigin.y, (double)self->linkedSprite->super.frame.origin.x, (double)self->linkedSprite->super.frame.origin.y, (double)extraOrigin.x, (double)extraOrigin.y);
#endif

    const MELPoint moveTarget = (MELPoint) {
        .x = mainOrigin.x + distance,
        .y = mainOrigin.y
    };

    Board *board = self->board;
    if (!BoardCanMoveToPoint(board, moveTarget) || !BoardCanMoveToPoint(board, (MELPoint) {.x = extraOrigin.x + distance, .y = extraOrigin.y})) {
        return;
    }
#if LOG_MOVES
    playdate->system->logToConsole("Can move to x: %f, y: %f", (double)moveTarget.x, (double)moveTarget.y);
#endif
    SamplePlay(SampleMove);
    self->moveTarget = moveTarget;
    self->mainStart = mainOrigin.x;
    self->extraStart = self->linkedSprite->super.frame.origin.x;
    self->distanceOrRotation = distance;
    self->state = CardSpriteStateMovingLaterally;
    playdate->sprite->setUpdateFunction(sprite, updateLateralMove);
}

static void tryRotate(CardSprite * _Nonnull self, LCDSprite * _Nonnull sprite, MELDirection direction) {
    Board *board = self->board;
    const MELPoint extra = self->super.frame.origin;
    const MELPoint main = self->linkedSprite->super.frame.origin;
    const float from = atan2f(extra.y - main.y, extra.x - main.x);
    self->extraStart = from;

    const MELPoint mainTarget = MELPointEquals(self->linkedSprite->moveTarget, MELPointZero)
        ? self->linkedSprite->super.frame.origin
        : self->linkedSprite->moveTarget;
#if LOG_MOVES
    playdate->system->logToConsole("Try rotate, extra(x: %f, y: %f), main(x: %f, y: %f), mainTarget(x: %f, y: %f)", (double)extra.x, (double)extra.y, (double)main.x, (double)main.y, (double)mainTarget.x, (double)mainTarget.y);
#endif

    const float rotation = MEL_PI_2 * MELDirectionValues[direction];
    const float targetAngle = from + rotation;
    const MELPoint targetPoint = (MELPoint) {
        .x = mainTarget.x + cosf(targetAngle) * TILE_SIZE,
        .y = mainTarget.y + sinf(targetAngle) * TILE_SIZE,
    };

    if (BoardCanMoveToPoint(board, targetPoint)) {
#if LOG_MOVES
        playdate->system->logToConsole("Can rotate to x: %f, y: %f", (double)targetPoint.x, (double)targetPoint.y);
#endif
        self->count = MELDirectionValues[direction];
        self->distanceOrRotation = rotation;
        self->moveTarget = targetPoint;
        self->state = CardSpriteStateRotating;
        playdate->sprite->setUpdateFunction(sprite, updateRotation);
        SamplePlay(SampleRotate);
        return;
    }

    if (MELFloatEquals(targetPoint.x, mainTarget.x) && targetPoint.y > mainTarget.y) {
        // Floor kick
#if LOG_MOVES
        playdate->system->logToConsole("Can rotate with floor kick to x: %f, y: %f", (double)targetPoint.x, (double)targetPoint.y - TILE_SIZE);
#endif
        self->linkedSprite->super.frame.origin.y -= TILE_SIZE;
        if (!MELPointEquals(self->linkedSprite->moveTarget, MELPointZero)) {
            self->linkedSprite->moveTarget.y -= TILE_SIZE;
        }
        if (self->linkedSprite->lockDelay) {
            const int floorKickCount = ++self->linkedSprite->floorKickCount;
#if LOG_MOVES
            playdate->system->logToConsole("Log delay kick nb %d", floorKickCount);
#endif
            if (floorKickCount >= LOCK_DELAY_MAX_KICK) {
                self->linkedSprite->lockDelay = 1;
            } else {
                self->linkedSprite->lockDelay = playdate->system->getCurrentTimeMilliseconds();
            }
        }
        self->count = MELDirectionValues[direction];
        self->distanceOrRotation = rotation;
        self->moveTarget = targetPoint;
        self->moveTarget.y -= TILE_SIZE;
        self->super.frame.origin.y -= TILE_SIZE;
        self->state = CardSpriteStateRotating;
        playdate->sprite->setUpdateFunction(sprite, updateRotation);
        SamplePlay(SampleRotate);
        return;
    }

    const float translationX = targetPoint.x > extra.x ? TILE_SIZE : -TILE_SIZE;
    MELPoint mainAfterWallKick = self->linkedSprite->super.frame.origin;
    mainAfterWallKick.x -= translationX;
    if (MELFloatEquals(targetPoint.y, mainTarget.y) && BoardCanMoveToPoint(board, mainAfterWallKick)) {
        // Wall kick
#if LOG_MOVES
        playdate->system->logToConsole("Can rotate with wall kick to x: %f, y: %f (translationX: %f)", (double)(targetPoint.x - translationX), (double)targetPoint.y, (double)translationX);
#endif
        self->linkedSprite->super.frame.origin = mainAfterWallKick;
        if (!MELPointEquals(self->linkedSprite->moveTarget, MELPointZero)) {
            self->linkedSprite->moveTarget.x -= translationX;
            self->linkedSprite->mainStart -= translationX;
        }
        self->count = MELDirectionValues[direction];
        self->distanceOrRotation = rotation;
        self->moveTarget = targetPoint;
        self->moveTarget.x -= translationX;
        self->super.frame.origin.x -= translationX;
        self->state = CardSpriteStateRotating;
        playdate->sprite->setUpdateFunction(sprite, updateRotation);
        SamplePlay(SampleRotate);
        return;
    }

    if (MELFloatEquals(mainTarget.x, extra.x)) {
        // Rotation double
        // NOTE: Pourrait être plus simple de juste comparer sur y et faire +/- TILE_SIZE
        const float twiceRotation = MEL_PI_2 * 2.0f * MELDirectionValues[direction];
        const float twiceAngle = from + twiceRotation;
        const MELPoint twiceTarget = (MELPoint) {
            .x = mainTarget.x + cosf(twiceAngle) * TILE_SIZE,
            .y = mainTarget.y + sinf(twiceAngle) * TILE_SIZE
        };

        if (BoardCanMoveToPoint(board, twiceTarget)) {
#if LOG_MOVES
            playdate->system->logToConsole("Can rotate with twice to x: %f, y: %f", (double)twiceTarget.x, (double)twiceTarget.y);
#endif
            self->moveTarget = twiceTarget;
            self->count = MELDirectionValues[direction];
            self->distanceOrRotation = twiceRotation;
            self->state = CardSpriteStateRotating;
            playdate->sprite->setUpdateFunction(sprite, updateRotation);
            SamplePlay(SampleRotate);
            return;
        }
        if (MELFloatEquals(twiceTarget.x, mainTarget.x) && twiceTarget.y > mainTarget.y) {
            // Floor kick
#if LOG_MOVES
        playdate->system->logToConsole("Can rotate with twice + floor kick to x: %f, y: %f", (double)twiceTarget.x, (double)twiceTarget.y - TILE_SIZE);
#endif
            self->linkedSprite->super.frame.origin.y -= TILE_SIZE;
            if (!MELPointEquals(self->linkedSprite->moveTarget, MELPointZero)) {
                self->linkedSprite->moveTarget.y -= TILE_SIZE;
            }
            if (self->linkedSprite->lockDelay) {
                const int floorKickCount = ++self->linkedSprite->floorKickCount;
#if LOG_MOVES
            playdate->system->logToConsole("Log delay kick nb %d", floorKickCount);
#endif
                if (floorKickCount >= LOCK_DELAY_MAX_KICK) {
                    self->linkedSprite->lockDelay = 1;
                } else {
                    self->linkedSprite->lockDelay = playdate->system->getCurrentTimeMilliseconds();
                }
            }
            self->count = MELDirectionValues[direction];
            self->distanceOrRotation = twiceRotation;
            self->moveTarget = twiceTarget;
            self->moveTarget.y -= TILE_SIZE;
            self->super.frame.origin.y -= TILE_SIZE;
            self->state = CardSpriteStateRotating;
            playdate->sprite->setUpdateFunction(sprite, updateRotation);
            SamplePlay(SampleRotate);
            return;
        }
    }

    // Pas possible de tourner.
}

static void fall(CardSprite * _Nonnull self) {
    const float time = self->time += DELTA;

    const float duration = 0.5f;
    const float gameSpeed = GetGameSpeed();
    const float currentDuration = self->controller->axe.y > 0.0f
        ? gameSpeed * duration / 15.0f
        : gameSpeed * duration;
    const float height = TILE_SIZE / 2.0f;

    if (time >= currentDuration) {
        self->time -= currentDuration;
        if (isAboveSomething(self)) {
            return;
        }
        self->super.frame.origin.y += height;
        self->linkedSprite->super.frame.origin.y += height;
        if (!MELPointEquals(self->linkedSprite->moveTarget, MELPointZero)) {
            self->linkedSprite->moveTarget.y += height;
        }
    }
}

static MELBoolean isAboveSomething(CardSprite * _Nonnull self) {
    CardSprite *extra = self->linkedSprite;
    return BoardIsMELSpriteAboveSomething(self->board, &self->super) || (extra->state == CardSpriteStateExtra && BoardIsMELSpriteAboveSomething(self->board, &extra->super));
}

#pragma mark - Méthodes update

static void updateNoop(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    const MELPoint origin = self->super.frame.origin;
    const MELPoint camera = self->board->camera;
    playdate->sprite->moveTo(sprite, origin.x + camera.x, origin.y + camera.y);
}

static void updateLateralMove(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    fall(self);

    const float duration = MOVE_DURATION;
    const float oldProgress = MELEaseInOut(0, duration, self->moveTime);
    const float time = self->moveTime += DELTA;
    const float progress = MELEaseInOut(0, duration, time);

    const float x = progress * self->distanceOrRotation;
    const float xDiff = x - (oldProgress * self->distanceOrRotation);
    MELPoint origin = self->super.frame.origin;
    origin.x = self->mainStart + x;
    self->super.frame.origin = origin;
    self->linkedSprite->super.frame.origin.x += xDiff;
    self->linkedSprite->moveTarget.x += xDiff;

    if (progress == 1.0f) {
        self->moveTime = 0.0f;
        self->mainStart = 0.0f;
        self->extraStart = 0.0f;
        self->moveTarget = MELPointZero;
        self->state = CardSpriteStateMain;
        playdate->sprite->setUpdateFunction(sprite, updateMain);
    }

    self->cursor->frame.origin = origin;

    const MELPoint camera = self->board->camera;
    playdate->sprite->moveTo(sprite, origin.x + camera.x, origin.y + camera.y);
}

static void updateRotation(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);

    const float duration = MOVE_DURATION;
    const float time = self->moveTime += DELTA;
    const float progress = MELEaseInOut(0, duration, time);

    const float angle = self->extraStart + self->distanceOrRotation * progress;
    MELPoint center = self->linkedSprite->super.frame.origin;
    MELPoint origin = (MELPoint) {
        .x = center.x + cosf(angle) * TILE_SIZE,
        .y = center.y + sinf(angle) * TILE_SIZE,
    };
    self->super.frame.origin = origin;

    if (progress == 1.0f) {
        self->moveTime = 0.0f;
        self->mainStart = 0.0f;
        self->extraStart = 0.0f;
        self->moveTarget = MELPointZero;
        self->state = CardSpriteStateExtra;
        self->direction = MELDirectionCircle[(MELDirectionCircleIndex[self->direction] + self->count + DIRECTION_COUNT) % DIRECTION_COUNT];
        playdate->sprite->setUpdateFunction(sprite, updateExtra);
    }

    const MELPoint camera = self->board->camera;
    playdate->sprite->moveTo(sprite, origin.x + camera.x, origin.y + camera.y);
}

static void updateMain(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    fall(self);
    self->moveTarget = self->super.frame.origin;

    const float controllerAxeX = self->controller->axe.x;
    if (controllerAxeX < 0.0f) {
        tryLateralMove(self, sprite, MELDirectionLeft);
    } else if (controllerAxeX > 0.0f) {
        tryLateralMove(self, sprite, MELDirectionRight);
    }

    const MELPoint origin = self->super.frame.origin;
    const MELPoint camera = self->board->camera;
    self->cursor->frame.origin = origin;
    playdate->sprite->moveTo(sprite, origin.x + camera.x, origin.y + camera.y);
}

static void updateExtra(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    self->moveTarget = self->super.frame.origin;
    if (self->controller->pressedB) {
        tryRotate(self, sprite, MELDirectionLeft);
    } else if (self->controller->pressedA) {
        tryRotate(self, sprite, MELDirectionRight);
    }

    const MELPoint origin = self->super.frame.origin;
    const MELPoint camera = self->board->camera;
    playdate->sprite->moveTo(sprite, origin.x + camera.x, origin.y + camera.y);
}

static void updateBlinking(LCDSprite * _Nonnull sprite) {
    CardSprite *self = playdate->sprite->getUserdata(sprite);

    const float duration = 0.25f;
    if (self->time < duration) {
        self->time += DELTA;

        const MELBoolean wasVisible = playdate->sprite->isVisible(sprite);
        const uint32_t timeInMillis = playdate->system->getCurrentTimeMilliseconds();
        const MELBoolean isVisible = (timeInMillis / 5) % 2;
        if (isVisible != wasVisible) {
            playdate->sprite->setVisible(sprite, isVisible);
        }
        return;
    }
    self->time = 0.0f;
    self->state = CardSpriteStateNoop;
    playdate->sprite->setVisible(sprite, true);
    playdate->sprite->setUpdateFunction(sprite, updateNoop);
}

static void updateFalling(LCDSprite * _Nonnull sprite) {
    CardSprite * _Nonnull self = playdate->sprite->getUserdata(sprite);
    const float waitDuration = self->time;
    if (waitDuration > 0.0f) {
        self->time = waitDuration - DELTA;
        return;
    }

    const float speed = self->moveTime;
    const float step = DELTA * speed;
    self->moveTime += FALL_ACCELERATION * DELTA;

    LCDSpriteRefList tail = self->tail;
    if (tail.count == 0) {
        tail.count = 1;
        tail.memory = &sprite;
    }

    const MELPoint camera = self->board->camera;
    for (unsigned int index = 0; index < tail.count; index++) {
        LCDSprite *sprite = tail.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        MELPoint origin = melSprite->frame.origin;
        origin.y += step;
        melSprite->frame.origin = origin;
        MELAnimation *animation = melSprite->animation;
        if (animation) {
            MELAnimationUpdate(animation, DELTA);
            playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(melSprite->definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
        }
        playdate->sprite->moveTo(sprite, origin.x + camera.x, origin.y + camera.y);
    }

    if (BoardIsSpriteAboveSomething(self->board, sprite)) {
        self->state = CardSpriteStateNoop;
        playdate->sprite->setUpdateFunction(sprite, updateNoop);

        if (self->super.animation) {
            for (unsigned int index = 0; index < tail.count; index++) {
                LCDSprite *sprite = tail.memory[index];
                MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
                MELAnimationDealloc(melSprite->animation);
                melSprite->animation = NULL;
                playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(melSprite->definition.palette, 0), kBitmapUnflipped);
            }
        }

        BoardAttachSprite(self->board, sprite, tail);
        LCDSpriteRefListDeinit(&self->tail);

        if (self->super.definition.name == SpriteNameChip18) {
            BoardShake(self->board, 0.5f, tail.count);
        }
    }
}

static void updateBouncing(LCDSprite * _Nonnull sprite) {
    CardSprite * _Nonnull self = playdate->sprite->getUserdata(sprite);

    MELRectangle frame = self->super.frame;
    frame.origin.x += self->time * DELTA;
    frame.origin.y += self->moveTime * DELTA;
    self->super.frame = frame;
    self->moveTime += 500 * DELTA;

    if (MELRectangleIntersectsWithRectangle(frame, MELScreen)) {
        playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    } else {
        MELSpriteDealloc(sprite);
    }
}

static void onOtherSideLost(void * _Nonnull userdata, int _) {
    LCDSprite *sprite = userdata;
    CardSprite *self = playdate->sprite->getUserdata(sprite);
    playdate->sprite->setUpdateFunction(sprite, self->super.class->destroy);
}
