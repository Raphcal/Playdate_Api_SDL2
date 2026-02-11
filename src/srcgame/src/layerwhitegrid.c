//
//  layerwhitegrid.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/08/2023.
//

#include "layerwhitegrid.h"

#include "image.h"

LCDColor kGridPattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = noir, 1 = blanc
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,

    // Mask, 0 = transparent, 1 = visible
    0b01010101,
    0b11111111,
    0b01010101,
    0b11111111,
    0b01010101,
    0b11111111,
    0b01010101,
    0b11111111,
};

LCDSprite * _Nonnull LayerWhiteGridConstructor(void) {
    LCDBitmap *image = playdate->graphics->newBitmap(LCD_COLUMNS, LCD_ROWS, kColorClear);
    playdate->graphics->pushContext(image);
    playdate->graphics->fillRect(0, 0, LCD_COLUMNS, LCD_ROWS, kGridPattern);
    playdate->graphics->popContext();
    LCDSprite *sprite = ImageConstructor(MELPointZero, image);
    playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    playdate->sprite->moveTo(sprite, LCD_COLUMNS / 2, LCD_ROWS / 2);
    playdate->sprite->setZIndex(sprite, ZINDEX_BG_GRID);
    return sprite;
}
