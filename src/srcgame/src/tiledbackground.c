//
//  tiledbackground.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 29/03/2024.
//

#include "tiledbackground.h"

typedef struct {
    MELSprite super;
    LCDBitmap * _Nullable tile;
    MELPoint offset;
    MELPoint speed;
} TiledBackground;

static void destroy(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);

static const MELSpriteClass TiledBackgroundClass = (MELSpriteClass) {
    .destroy = destroy,
};

LCDSprite * _Nonnull TiledBackgroundAlloc(LCDBitmap * _Nonnull tile, MELPoint speed) {
    TiledBackground *self = playdate->system->realloc(NULL, sizeof(TiledBackground));
    *self = (TiledBackground) {
        .super = {
            .class = &TiledBackgroundClass,
            .frame = {
                .origin = {
                    .x = LCD_COLUMNS / 2,
                    .y = LCD_ROWS / 2,
                },
                .size = {
                    .width = LCD_COLUMNS,
                    .height = LCD_ROWS
                }
            },
        },
        .tile = tile,
        .speed = speed,
    };

    LCDBitmap *image = playdate->graphics->newBitmap(LCD_COLUMNS, LCD_ROWS, kColorWhite);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->moveTo(sprite, LCD_COLUMNS / 2, LCD_ROWS / 2);
    playdate->sprite->addSprite(sprite);
    return sprite;
}

static void destroy(LCDSprite * _Nonnull sprite) {
    TiledBackground *self = playdate->sprite->getUserdata(sprite);
    if (self->tile) {
        playdate->graphics->freeBitmap(self->tile);
        self->tile = NULL;
    }
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);
    MELSpriteDealloc(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    TiledBackground *self = playdate->sprite->getUserdata(sprite);

    const MELPoint offset = MELPointAdd(self->offset, MELPointMultiplyByValue(self->speed, DELTA));
    self->offset = offset;

    const struct playdate_graphics *graphics = playdate->graphics;

    int width, height;
    graphics->getBitmapData(self->tile, &width, &height, NULL, NULL, NULL);

    int x = ((int)offset.x) % width;
    int y = ((int)offset.y) % height;

    if (x > 0) {
        x -= width;
    }
    if (y > 0) {
        y -= height;
    }

    LCDBitmap *image = playdate->sprite->getImage(sprite);
    graphics->clearBitmap(image, kColorWhite);
    graphics->pushContext(image);
    graphics->tileBitmap(self->tile, x, y, LCD_COLUMNS - x, LCD_ROWS - y, kBitmapUnflipped);
    graphics->popContext();
    playdate->sprite->markDirty(sprite);
}
