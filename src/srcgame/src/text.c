//
//  text.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 13/07/2023.
//

#include "text.h"

#include "image.h"

static const PDStringEncoding kDefaultEncoding = kASCIIEncoding;

static LCDBitmap * _Nonnull drawText(LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length);

LCDSprite * _Nonnull TextConstructor(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    return ImageConstructor(origin, drawText(font, drawMode, text, length));
}

LCDSprite * _Nonnull TextConstructorDontPush(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    return ImageConstructorWithSelfDontPush(self, origin, drawText(font, drawMode, text, length));
}

void TextSetWithFormatAndInt(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull format, int value) {
    char *text = NULL;
    const struct playdate_sys system = *playdate->system;
    system.formatString(&text, format, value);
    TextSet(sprite, font, drawMode, text, 255);
    system.realloc(text, 0);
}

void TextSet(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    if (!sprite) {
        return;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);

    image = drawText(font, drawMode, text, length);
    int newWidth, newHeight;
    playdate->graphics->getBitmapData(image, &newWidth, &newHeight, NULL, NULL, NULL);
    MELRectangle frame = self->frame;
    frame.origin.x += (newWidth - frame.size.width) / 2;
    frame.origin.y += (newHeight - frame.size.height) / 2;
    frame.size.width = newWidth;
    frame.size.height = newHeight;
    self->frame = frame;

    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setSize(sprite, newWidth, newHeight);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->markDirty(sprite);
}

static LCDBitmap * _Nonnull drawText(LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    const int width = playdate->graphics->getTextWidth(font, text, length, kDefaultEncoding, 0);
    const int height = playdate->graphics->getFontHeight(font);

    LCDBitmap *bitmap = playdate->graphics->newBitmap(width, height, kColorClear);
    playdate->graphics->pushContext(bitmap);
    playdate->graphics->setFont(font);
    playdate->graphics->setDrawMode(drawMode);
    playdate->graphics->drawText(text, length, kDefaultEncoding, 0, 0);
    playdate->graphics->popContext();

    return bitmap;
}
