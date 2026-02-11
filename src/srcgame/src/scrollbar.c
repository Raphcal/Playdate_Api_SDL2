//
//  scrollbar.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 01/04/2025.
//

#include "scrollbar.h"

#include "image.h"

typedef struct {
    MELSprite super;
    float top;
    float height;
    float ratio;
    float oldY;
} ScrollBar;

static const LCDColor kDiagonalsPattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = black, 1 = white
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    // Mask, 0 = transparent, 1 = visible
    0b00010001,
    0b00100010,
    0b01000100,
    0b10001000,
    0b00010001,
    0b00100010,
    0b01000100,
    0b10001000,
};

static const int kSize = 4;
static const int kPadding = 6;
static const int kScrollBarHeight = LCD_ROWS - kPadding * 2;

static void draw(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);

static MELSpriteClass ScrollBarClass = (MELSpriteClass) {
    .name = SpriteClassNameScrollBar,
    .destroy = ImageDealloc,
    .update = update,
};

LCDSprite * _Nullable ScrollBarConstructor(float top, float height) {
    ScrollBar *self = new(ScrollBar);
    height -= kPadding * 2;
    *self = (ScrollBar) {
        .super = {
            .class = &ScrollBarClass,
            .frame = {
                .origin = {
                    .x = LCD_COLUMNS - 4 - kSize / 2,
                    .y = LCD_ROWS / 2
                },
                .size = {
                    .width = kSize,
                    .height = kScrollBarHeight,
                }
            },
            .fixed = MELSpritePositionFixedBoth,
        },
        .top = top,
        .height = height,
        .ratio = kScrollBarHeight / height,
        .oldY = camera.frame.origin.y,
    };

    const MELRectangle frame = self->super.frame;

    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->setUserdata(sprite, self);
    spriteAPI->setUpdateFunction(sprite, update);
    spriteAPI->setImage(sprite, playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorClear), kBitmapUnflipped);
    spriteAPI->setZIndex(sprite, ZINDEX_SCROLLBAR);
    spriteAPI->moveTo(sprite, frame.origin.x, frame.origin.y);
    spriteAPI->addSprite(sprite);
    draw(sprite);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

void ScrollBarSetHeight(LCDSprite * _Nonnull sprite, float height) {
    ScrollBar *self = playdate->sprite->getUserdata(sprite);
    height -= kPadding * 2;
    self->height = height;
    self->ratio = kScrollBarHeight / height;
    draw(sprite);
}

static void draw(LCDSprite * _Nonnull sprite) {
    ScrollBar *self = playdate->sprite->getUserdata(sprite);
    LCDBitmap *image = playdate->sprite->getImage(sprite);

    const MELSize size = self->super.frame.size;
    const float barHeight = ceilf(size.height * self->ratio);
    const float top = MELFloatBound(0, (camera.frame.origin.y - self->top) * self->ratio, size.height - barHeight);

    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);

    graphics->fillRect(0, 0, size.width, size.height, kDiagonalsPattern);
    graphics->fillRect(0, top, size.width, barHeight, kColorBlack);

    graphics->popContext();
}

static void update(LCDSprite * _Nonnull sprite) {
    ScrollBar *self = playdate->sprite->getUserdata(sprite);
    const float y = camera.frame.origin.y;
    if (self->oldY != y) {
        self->oldY = y;
        draw(sprite);
        playdate->sprite->markDirty(sprite);
    }

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
}
