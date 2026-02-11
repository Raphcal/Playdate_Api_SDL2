//
//  progressbar.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/10/2023.
//

#include "progressbar.h"

#include "image.h"
#include "gui_lifebar.h"

static const int kBorderWidth = 1;

static void draw(LCDBitmap *image, MELSize size, float progress);

LCDSprite * _Nonnull ProgressBarConstructor(const MELRectangle bounds, const float progress) {
    LCDBitmap *image = playdate->graphics->newBitmap(bounds.size.width, bounds.size.height, kColorClear);
    draw(image, bounds.size, progress);
    LCDSprite *sprite = ImageConstructorWithSelfDontPush(playdate->system->realloc(NULL, sizeof(MELSprite)), bounds.origin, image);
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->hitTimer = progress;
    return sprite;
}

void ProgressBarSetProgress(LCDSprite * _Nonnull sprite, float progress) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    if (progress != self->hitTimer) {
        self->hitTimer = progress;
        LCDBitmap *image = playdate->sprite->getImage(sprite);
        draw(image, self->frame.size, progress);
        playdate->sprite->markDirty(sprite);
    }
}

static void draw(LCDBitmap *image, MELSize size, float progress) {
    playdate->graphics->pushContext(image);
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->clearBitmap(image, kColorClear);
    if (progress < 1.0f) {
        const int width = size.width * MELFloatMax(progress, 0.0f);
        playdate->graphics->fillRect(kBorderWidth, kBorderWidth, width - kBorderWidth, size.height - kBorderWidth * 2, kColorBlack);
        playdate->graphics->fillRect(kBorderWidth, width + kBorderWidth, size.width - width - kBorderWidth, size.height - kBorderWidth * 2, kColorWhite);
        playdate->graphics->fillRect(kBorderWidth, width + kBorderWidth, size.width - width - kBorderWidth, size.height - kBorderWidth * 2, kDiagonalsPattern);
    } else {
        playdate->graphics->fillRect(kBorderWidth, kBorderWidth, size.width - kBorderWidth * 2, size.height - kBorderWidth * 2, kColorBlack);
    }
    playdate->graphics->drawRect(0, 0, size.width, size.height, kColorWhite);
    playdate->graphics->popContext();
}
