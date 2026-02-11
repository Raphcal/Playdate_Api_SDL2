//
//  rect.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 05/08/2025.
//

#include "rect.h"

#include "image.h"

typedef struct {
    MELSprite super;
    float time;
    float duration;
} Rect;

static void update(LCDSprite * _Nonnull sprite);

LCDSprite * _Nonnull RectConstructor(float targetHeight, float duration) {
    Rect *self = new(Rect);
    *self = (Rect) {
        .super = {
            .class = ImageGetClass(),
            .frame = (MELRectangle) {
                .origin = {
                    .x = LCD_COLUMNS / 2,
                    .y = LCD_ROWS / 2
                },
                .size = {
                    .width = LCD_COLUMNS,
                    .height = targetHeight,
                }
            },
            .direction = MELDirectionRight,
        },
        .duration = duration,
    };

    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->moveTo(sprite, LCD_COLUMNS / 2, LCD_ROWS / 2);
    spriteAPI->setImage(sprite, playdate->graphics->newBitmap(LCD_COLUMNS, targetHeight, kColorBlack), kBitmapUnflipped);
    spriteAPI->setSize(sprite, LCD_COLUMNS, 1);
    spriteAPI->setUserdata(sprite, self);
    spriteAPI->addSprite(sprite);
    spriteAPI->setUpdateFunction(sprite, update);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void update(LCDSprite * _Nonnull sprite) {
    const struct playdate_sprite *spriteAPI = playdate->sprite;
    Rect *self = spriteAPI->getUserdata(sprite);
    const float time = self->time;
    const float duration = self->duration;
    if (time < duration) {
        self->time = time + DELTA;
        const float progress = MELEaseInOut(0, duration, time);
        spriteAPI->setSize(sprite, LCD_COLUMNS, self->super.frame.size.height * progress);
    } else {
        spriteAPI->setSize(sprite, LCD_COLUMNS, self->super.frame.size.height);
        spriteAPI->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    }
}
