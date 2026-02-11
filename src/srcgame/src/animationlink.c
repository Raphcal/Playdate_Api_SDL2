//
//  animationlink.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 02/09/2023.
//

#include "animationlink.h"

#include "camera.h"
#include "image.h"
#include "textbubble.h"

static const float kTextBubbleCrankFactor = 2.0f; // Pour la démo : 8

#pragma mark - Déplacement de la caméra

float CameraAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    const float from = self->from;
    const float to = self->to;
    const float distance = to - from;
    const float progress = self->progress += crankChange;
    if (progress < 0.0f) {
        camera.frame.origin.y = from;
        self->increment = -1;
        return progress;
    }
    else if (progress >= distance) {
        camera.frame.origin.y = to;
        self->increment = 1;
        return progress - distance;
    }
    else {
        self->increment = 0;
        camera.frame.origin.y = from + MELEaseInOut(0, distance, progress) * distance;
        return 0.0f;
    }
}

#pragma mark - B pour passer.

MELCrankIndicator * _Nullable crankIndicator;

static LCDSprite * _Nonnull createTextOnBlackBackground(const char * _Nonnull text, const size_t length) {
    const int height = 32;
    LCDBitmap *image = playdate->graphics->newBitmap(LCD_COLUMNS, height, kColorBlack);
    playdate->graphics->pushContext(image);
    playdate->graphics->setDrawMode(kDrawModeInverted);
    playdate->graphics->setFont(romajiFont);
    playdate->graphics->drawText(text, length, kASCIIEncoding, 8, 8);
    playdate->graphics->popContext();
    return ImageConstructor((MELPoint) {
        .x = LCD_COLUMNS / 2,
        .y = LCD_ROWS - height / 2
    }, image);
}

float BToSkipSpriteAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *bToSkip = self->sprite;
    if (bToSkip == NULL) {
        self->sprite = bToSkip = createTextOnBlackBackground("\b skip", 6);
        playdate->sprite->setZIndex(bToSkip, ZINDEX_PLAYER - 1);
    }
    if (crankIndicator == NULL) {
        crankIndicator = MELCrankIndicatorConstructor(true);
    }
    MELCrankIndicatorDraw(crankIndicator, 0, 0);
    if (crankChange > 0.0f) {
        MELSprite *sprite = playdate->sprite->getUserdata(bToSkip);
        sprite->class->destroy(bToSkip);
        MELCrankIndicatorDealloc(crankIndicator);
        crankIndicator = NULL;
        self->sprite = NULL;
        self->increment = 1;
        return crankChange;
    }
    self->increment = 0;
    return 0.0f;
}

#pragma mark - Affichage et défilement du texte dans une bulle

float ScrollTextBubbleAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *textBubble = self->sprite;
    if (textBubble == NULL) {
        self->sprite = textBubble = TextBubbleConstructor(self->text, self->textLineCount, (MELPoint) {
            .x = self->TEXT_X,
            .y = self->TEXT_Y
        });
        TextBubbleAnimateAppear(textBubble);
    } else if (!playdate->sprite->isVisible(textBubble)) {
        TextBubbleAnimateAppear(textBubble);
    }
    float remaining = TextBubbleScroll(textBubble, crankChange / kTextBubbleCrankFactor);
    if (remaining != 0.0f) {
        TextBubbleAnimateDisappear(textBubble);
    }
    self->increment = MELFloatSign(remaining);
    return remaining * kTextBubbleCrankFactor;
}

#pragma mark - Attente

float WaitSpriteAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    float duration = self->to == 0.0f ? 150.0f : self->to;
    const float wait = self->from + crankChange;
    if (wait < 0.0f) {
        self->from = 0.0f;
        self->increment = -1;
        return wait;
    }
    else if (wait >= duration) {
        self->from = duration;
        self->increment = 1;
        return wait - duration;
    } else {
        self->from = wait;
        self->increment = 0;
        return 0.0f;
    }
}
