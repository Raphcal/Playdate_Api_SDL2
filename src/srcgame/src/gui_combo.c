//
//  gui_combo.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/10/2023.
//

#include "gui_combo.h"

#include "gamescene.h"
#include "image.h"
#include "progressbar.h"
#include "counter.h"

static const float kHitLabelX = 95.0f;
static const float kHitLabelY = 57.0f;
static const float kProgressBarX = 95.0f;
static const float kProgressBarY = 73.0f;
static const float kCounterX = 77.0f;
static const float kCounterY = 63.0f;
static const int kAnimationDurationInMillis = 250;
static const float kAnimationDistance = 8.0f;

void GUIComboInit(GUICombo * _Nonnull self) {
    LCDSprite *hitLabel = ImageConstructorWithSelfDontPush(playdate->system->realloc(NULL, sizeof(MELSprite)), (MELPoint) {
        .x = kHitLabelX,
        .y = kHitLabelY,
    }, LCDBitmapLoadOrError("gui-hit"));

    LCDSprite *progressBar = ProgressBarConstructor((MELRectangle) {
        .origin = {
            .x = kProgressBarX,
            .y = kProgressBarY
        },
        .size = {
            .width = 37,
            .height = 6
        }
    }, self->timer / COMBO_DURATION);

    LCDSprite *counter = CounterConstructor((MELPoint) {
        .x = kCounterX,
        .y = kCounterY
    }, kCounterAlignmentRight, numbers32Font, self->combo);

    GameScene *gameScene = GameSceneGetCurrent();
    LCDSpriteRefListPush(&gameScene->others, hitLabel);
    LCDSpriteRefListPush(&gameScene->others, progressBar);
    // NOTE: `counter` est ajouté à `others` dans `CounterConstructor`.

    const MELBoolean visible = self->timer > 0.0f;

    playdate->sprite->setVisible(hitLabel, visible);
    playdate->sprite->setVisible(progressBar, visible);
    playdate->sprite->setVisible(counter, visible);

    playdate->sprite->setZIndex(hitLabel, ZINDEX_GUI);
    playdate->sprite->setZIndex(progressBar, ZINDEX_GUI);
    playdate->sprite->setZIndex(counter, ZINDEX_GUI);

    playdate->sprite->setUpdateFunction(hitLabel, MELSpriteNoopUpdate);
    playdate->sprite->setUpdateFunction(progressBar, MELSpriteNoopUpdate);

    self->hitLabel = hitLabel;
    self->progressBar = progressBar;
    self->counter = counter;
}


static void updateAppear(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    // during
    const int time = self->score;
    const int duration = kAnimationDurationInMillis;
    if (time < duration) {
        const int newTime = self->score = MELIntMin(time + DELTA * 1000.0f, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        const MELPoint origin = self->frame.origin;
        const float y = origin.y + kAnimationDistance * (1.0f - progress);

        playdate->sprite->moveTo(sprite, origin.x, y);
    } else {
        self->score = 0;
        playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    }
}
void GUIComboAnimateAppear(GUICombo self) {
    playdate->sprite->moveTo(self.hitLabel, kHitLabelX, kHitLabelY + kAnimationDistance);
    playdate->sprite->moveTo(self.progressBar, kProgressBarX, kProgressBarY + kAnimationDistance);
    MELSprite *counter = playdate->sprite->getUserdata(self.counter);
    playdate->sprite->moveTo(self.counter, counter->frame.origin.x, kCounterY + kAnimationDistance);

    playdate->sprite->setVisible(self.hitLabel, true);
    playdate->sprite->setVisible(self.progressBar, true);
    playdate->sprite->setVisible(self.counter, true);

    playdate->sprite->setUpdateFunction(self.hitLabel, updateAppear);
    playdate->sprite->setUpdateFunction(self.progressBar, updateAppear);
    playdate->sprite->setUpdateFunction(self.counter, updateAppear);
}

static void updateDisappear(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    // during
    const int time = self->score;
    const int duration = kAnimationDurationInMillis;
    if (time < duration) {
        const int newTime = self->score = MELIntMin(time + DELTA * 1000.0f, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        const MELPoint origin = self->frame.origin;
        const float y = origin.y + kAnimationDistance * progress;

        playdate->sprite->moveTo(sprite, origin.x, y);
    } else {
        self->score = 0;
        playdate->sprite->setVisible(sprite, false);
        playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    }
}
void GUIComboAnimateDisappear(GUICombo self) {
    playdate->sprite->setUpdateFunction(self.hitLabel, updateDisappear);
    playdate->sprite->setUpdateFunction(self.progressBar, updateDisappear);
    playdate->sprite->setUpdateFunction(self.counter, updateDisappear);
}

