//
//  fade.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 21/07/2023.
//

#include "fade.h"

static void dealloc(LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);
static void update(LCDSprite * _Nonnull sprite);

static const MELSpriteClass FadeClass = (MELSpriteClass) {
    .name = SpriteClassNameFade,
    .destroy = dealloc,
    .save = save,
    .load = load,
};

static void dealloc(LCDSprite * _Nonnull sprite) {
    Fade *self = playdate->sprite->getUserdata(sprite);
    LCDBitmap *bitmap = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(bitmap);
    playdate->graphics->freeBitmap(self->pattern);
    MELSpriteDealloc(sprite);
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    Fade *self = (Fade *)sprite;
    MELOutputStreamWriteByte(outputStream, self->opacity);
}

static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    const uint8_t opacity = MELInputStreamReadByte(inputStream);
    LCDBitmap *bitmap = playdate->graphics->newBitmap(LCD_COLUMNS, LCD_ROWS, kColorClear);
    LCDBitmap *pattern = playdate->graphics->newBitmap(8, 8, kColorClear);

    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setImage(sprite, bitmap, kBitmapUnflipped);

    Fade *self = playdate->system->realloc(NULL, sizeof(Fade));
    *self = (Fade) {
        .super = {
            .class = &FadeClass,
        },
        .opacity = opacity,
        .oldOpacity = 0,
        .pattern = pattern,
    };
    return &self->super;
}

static void update(LCDSprite * _Nonnull sprite) {
    Fade *self = playdate->sprite->getUserdata(sprite);
    const uint8_t opacity = self->opacity;
    if (opacity == self->oldOpacity) {
        return;
    }
    self->oldOpacity = opacity;
    LCDBitmap *pattern = self->pattern;
    playdate->graphics->clearBitmap(pattern, kColorClear);
    LCDBitmapFadeImage(pattern, opacity);

    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->clearBitmap(image, kColorClear);
    playdate->graphics->pushContext(image);
    playdate->graphics->tileBitmap(pattern, 0, 0, LCD_COLUMNS, LCD_ROWS, kBitmapUnflipped);
    playdate->graphics->popContext();

    playdate->sprite->markDirty(sprite);
}

LCDSprite * _Nonnull FadeConstructor(void) {
    Fade *self = playdate->system->realloc(NULL, sizeof(Fade));

    LCDBitmap *bitmap = playdate->graphics->newBitmap(LCD_COLUMNS, LCD_ROWS, kColorClear);
    LCDBitmap *pattern = playdate->graphics->newBitmap(8, 8, kColorClear);

    *self = (Fade) {
        .super = {
            .class = &FadeClass,
            .frame = {
                .size = {
                    .width = LCD_COLUMNS,
                    .height = LCD_ROWS
                },
                .origin = {
                    .x = LCD_COLUMNS / 2,
                    .y = LCD_ROWS / 2
                },
            },
        },
        .opacity = 0,
        .oldOpacity = 0,
        .pattern = pattern,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setImage(sprite, bitmap, kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_FADE);
    playdate->sprite->moveTo(sprite, LCD_COLUMNS / 2, LCD_ROWS / 2);
    playdate->sprite->addSprite(sprite);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push FadeConstructor(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

const MELSpriteClass * _Nonnull FadeGetClass(void) {
    return &FadeClass;
}
