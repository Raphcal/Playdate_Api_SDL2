//
//  progressbar.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/11/2024.
//

#include "progressbar.h"

#include "image.h"

static const LCDColor kGrey25Pattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = noir, 1 = blanc
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    // Mask, 0 = transparent, 1 = visible
    0b00000000,
    0b01010101,
    0b00000000,
    0b01010101,
    0b00000000,
    0b01010101,
    0b00000000,
    0b01010101,
};

typedef struct {
    MELSprite super;
    int minimum;
    int maximum;
    int oldValue;
    int * _Nonnull currentValue;
} ProgressBar;

static void update(LCDSprite * _Nonnull sprite);

static void repaint(ProgressBar * _Nonnull self, LCDSprite * _Nonnull sprite);

LCDSprite * _Nullable ProgressBarConstructor(MELRectangle frame, int minimum, int maximum, int * _Nonnull current) {
    ProgressBar *self = new(ProgressBar);
    *self = (ProgressBar) {
        .super = {
            .class = ImageGetClass(),
            .frame = frame,
        },
        .minimum = minimum,
        .maximum = maximum,
        .oldValue = *current,
        .currentValue = current,
    };

    LCDBitmap *image = playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorWhite);

    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->setUserdata(sprite, self);
    spriteAPI->setImage(sprite, image, kBitmapUnflipped);
    spriteAPI->setUpdateFunction(sprite, update);
    spriteAPI->addSprite(sprite);

    repaint(self, sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void repaint(ProgressBar * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELRectangle frame = self->super.frame;
    LCDBitmap *image = playdate->sprite->getImage(sprite);

    struct playdate_graphics gfx = *playdate->graphics;
    gfx.pushContext(image);
    gfx.setDrawMode(kDrawModeCopy);
    gfx.clearBitmap(image, kColorWhite);

    const float progress = MELProgress(self->minimum, self->maximum, *self->currentValue);
    gfx.fillRect(0, 0, frame.size.width, frame.size.height, kGrey25Pattern);
    gfx.fillRect(0, 0, frame.size.width * progress, frame.size.height, kColorBlack);
    gfx.drawRect(0, 0, frame.size.width, frame.size.height, kColorBlack);

    gfx.popContext();

    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->markDirty(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    ProgressBar *self = playdate->sprite->getUserdata(sprite);

    const int newValue = *self->currentValue;
    if (newValue != self->oldValue) {
        self->oldValue = newValue;
        repaint(self, sprite);
    }

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
}
