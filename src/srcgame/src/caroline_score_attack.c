//
//  caroline_score_attack.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 14/11/2024.
//

#include "caroline_score_attack.h"

#include "../gen/spritecarolinefacing.h"

#define TIME frame.size.width
#define DURATION frame.size.height

static const AnimationName kStand = AnimationNameStand;
static const AnimationName kPause = AnimationNameWalk;
static const AnimationName kCelebration = AnimationNameRun;
static const AnimationName kLost = AnimationNameDuck;

static const MELAnimationDirection kStandCheering = MELAnimationDirectionBottomLeft;

static const int kStandCount = 3;
static const int kPauseCount = 2;
static const int kCelebrationCount = 3;

static const float kStandDuration = 6.0f;
static const float kCheeringDuration = 4.0f;

static void dealloc(LCDSprite * _Nonnull sprite);

static void update(LCDSprite * _Nonnull sprite);
static void updateWaitBeforeCelebration(LCDSprite * _Nonnull sprite);
static void updateCelebration(LCDSprite * _Nonnull sprite);
static void updateCheering(LCDSprite * _Nonnull sprite);
static void updatePause(LCDSprite * _Nonnull sprite);

static void onCombo(void * _Nullable userdata, int combo);
static void onLost(void * _Nullable userdata, int combo);

const MELSpriteClass CarolineScoreAttackClass = (MELSpriteClass) {
    .destroy = dealloc,
};

LCDSprite * _Nonnull CarolineScoreAttackConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    return playdate->sprite->newSprite();
}
MELSprite * _Nullable CarolineScoreAttackLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    return NULL;
}

LCDSprite * _Nonnull CarolineScoreAttackConstructorDefault(void) {
    loadSpriteCarolineFacingPalette();
    MELSprite *self = new(MELSprite);
    LCDSprite *sprite = MELSpriteInitWithCenter(self, &spriteCarolineFacing, MELPointMake(62.0f, 200.0f));
    self->class = &CarolineScoreAttackClass;
    self->TIME = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setZIndex(sprite, ZINDEX_PORTAITS);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);

    MELEventBusAddListener(EventLeftSideMadeCombo, onCombo, sprite);
    MELEventBusAddListener(EventCenterSideLost, onLost, sprite);

    return sprite;
}

MELBoolean CarolineScoreAttackIsCheering(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    return self->animationDirection == kStandCheering
        && (self->animationName == kStand || self->animationName == kCelebration);
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    MELEventBusRemoveListeners(sprite);
    MELSpriteDealloc(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float time = self->TIME;
    if (time < kStandDuration) {
        self->TIME = time + DELTA;
        MELSpriteUpdate(sprite);
        return;
    }
    MELSpriteSetAnimationAndDirection(self, kPause, MELRandomInt(kPauseCount));
    MELSpriteUpdate(sprite);

    const float duration = (float)self->animation->definition->frameCount / self->animation->definition->frequency;
    self->DURATION = duration;
    self->TIME = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, updatePause);
}

static void updatePause(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float time = self->TIME;
    if (time < self->DURATION) {
        self->TIME = time + DELTA;
        MELSpriteUpdate(sprite);
        return;
    }
    MELSpriteSetAnimationAndDirection(self, kStand, MELRandomInt(kStandCount));
    MELSpriteUpdate(sprite);

    self->TIME = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, update);
}

static void updateWaitBeforeCelebration(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float time = self->TIME;
    if (time < CARD_DISAPPEAR_DURATION) {
        self->TIME = time + DELTA;
        MELSpriteUpdate(sprite);
        return;
    }
    const MELBoolean isCheering = CarolineScoreAttackIsCheering(sprite);
    self->animationName = AnimationNameStand;
    MELSpriteSetAnimationAndDirection(self, kCelebration, isCheering
          ? kStandCheering
          : MELRandomInt(kCelebrationCount));
    MELSpriteUpdate(sprite);

    const float duration = (float)self->animation->definition->frameCount / self->animation->definition->frequency;
    self->DURATION = duration;
    self->TIME = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, updateCelebration);
}

static void updateCelebration(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float time = self->TIME;
    if (time < self->DURATION) {
        self->TIME = time + DELTA;
        MELSpriteUpdate(sprite);
        return;
    }
    MELSpriteSetAnimationAndDirection(self, kStand, kStandCheering);
    MELSpriteUpdate(sprite);

    self->TIME = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, updateCheering);
}

static void updateCheering(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float time = self->TIME;
    if (time < kCheeringDuration) {
        self->TIME = time + DELTA;
        MELSpriteUpdate(sprite);
        return;
    }
    MELSpriteSetAnimationAndDirection(self, kStand, MELRandomInt(kStandCount));
    MELSpriteUpdate(sprite);

    self->TIME = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, update);
}

#pragma mark - Événements

static void onCombo(void * _Nullable userdata, int combo) {
    LCDSprite *sprite = userdata;
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    self->TIME = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, updateWaitBeforeCelebration);
}

static void onLost(void * _Nullable userdata, int combo) {
    LCDSprite *sprite = userdata;
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    MELSpriteSetAnimationAndDirection(self, kLost, 0);
    playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
}
