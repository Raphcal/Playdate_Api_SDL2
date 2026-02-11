//
//  counter.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/10/2023.
//

#include "counter.h"

#include "gamescene.h"

typedef struct {
    MELSprite super;
    LCDFont * _Nonnull font;
    MELHorizontalAlignment alignment;
    MELPoint anchor;
    char * _Nullable buffer;
    int bufferCapacity;
    uint32_t value;
} Counter;

static void dealloc(LCDSprite * _Nonnull sprite);
static void drawAndMove(Counter * _Nonnull self, LCDSprite * _Nonnull sprite, const int value);

static void updateUInt8(LCDSprite * _Nonnull sprite);
static void updateUInt32(LCDSprite * _Nonnull sprite);
static void updateFloat(LCDSprite * _Nonnull sprite);

static const MELSpriteClass CounterClass = (MELSpriteClass) {
    .destroy = dealloc,
};

LCDSprite * _Nonnull CounterConstructor(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, const uint32_t value) {
    Counter *self = playdate->system->realloc(NULL, sizeof(Counter));
    *self = (Counter) {
        .super = {
            .class = &CounterClass,
        },
        .font = font,
        .alignment = alignment,
        .anchor = origin,
        .buffer = NULL,
        .bufferCapacity = 0,
        .value = value,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->addSprite(sprite);

    drawAndMove(self, sprite, value);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push CounterConstructor(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

LCDSprite * _Nonnull CounterConstructorWithUInt8Pointer(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, uint8_t * _Nonnull value) {
    LCDSprite *sprite = CounterConstructor(origin, alignment, font, *value);
    Counter *self = playdate->sprite->getUserdata(sprite);
    self->super.userdata = value;
   
    playdate->sprite->setUpdateFunction(sprite, updateUInt8);
    return sprite;
}

LCDSprite * _Nonnull CounterConstructorWithUInt32Pointer(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, uint32_t * _Nonnull value) {
    LCDSprite *sprite = CounterConstructor(origin, alignment, font, *value);
    Counter *self = playdate->sprite->getUserdata(sprite);
    self->super.userdata = value;

    playdate->sprite->setUpdateFunction(sprite, updateUInt32);
    return sprite;
}

LCDSprite * _Nonnull CounterConstructorWithFloatPointer(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, float * _Nonnull value) {
    LCDSprite *sprite = CounterConstructor(origin, alignment, font, *value);
    Counter *self = playdate->sprite->getUserdata(sprite);
    self->super.userdata = value;

    playdate->sprite->setUpdateFunction(sprite, updateFloat);
    return sprite;
}

void CounterSetValue(LCDSprite * _Nullable sprite, const uint32_t value) {
    if (!sprite) {
        return;
    }
    Counter *self = playdate->sprite->getUserdata(sprite);
    if (value != self->value) {
        self->value = value;

        LCDBitmap *oldImage = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(oldImage);

        drawAndMove(self, sprite, value);
    }
}

void CountUpdateAnchorWithCurrentOrigin(LCDSprite * _Nonnull sprite) {
    Counter *self = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = self->super.frame;
    switch (self->alignment) {
        case MELHorizontalAlignmentLeft:
            frame.origin.x -= frame.size.width / 2.0f;
            break;

        case MELHorizontalAlignmentRight:
            frame.origin.x += frame.size.width / 2.0f;
            break;

        case MELHorizontalAlignmentCenter:
            // Pas de modification.
        default:
            break;
    }
    self->anchor = frame.origin;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    Counter *self = playdate->sprite->getUserdata(sprite);
    if (self->buffer) {
        playdate->system->realloc(self->buffer, 0);
        self->buffer = NULL;
    }

    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);

    MELSpriteDealloc(sprite);
}

static void drawAndMove(Counter * _Nonnull self, LCDSprite * _Nonnull sprite, const int value) {
    self->buffer = MELUInt32ToStringWithBuffer(value, self->buffer, &self->bufferCapacity);
    LCDFont *font = self->font;
    playdate->graphics->setFont(font);
    const int width = playdate->graphics->getTextWidth(font, self->buffer, self->bufferCapacity, kASCIIEncoding, 0);
    const int height = playdate->graphics->getFontHeight(font);

    MELRectangle frame;
    frame.size = (MELSize) {
        .width = width,
        .height = height
    };

    // Redessine l'image.
    LCDBitmap *image = playdate->graphics->newBitmap(width, height, kColorClear);
    playdate->graphics->pushContext(image);
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->drawText(self->buffer, self->bufferCapacity, kASCIIEncoding, 0, 0);
    playdate->graphics->popContext();

    // Défini et déplace le sprite.
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setSize(sprite, width, height);
    playdate->sprite->markDirty(sprite);

    frame.origin = MELOriginForSizeAndAlignment(self->anchor, frame.size, self->alignment, MELVerticalAlignmentMiddle);

    self->super.frame = frame;
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
}

#pragma mark - Mise à jour du compteur en fonction du changement

static void updateUInt32(LCDSprite * _Nonnull sprite) {
    Counter *self = playdate->sprite->getUserdata(sprite);
    uint32_t *newValuePointer = self->super.userdata;
    const uint32_t newValue = *newValuePointer;
    if (newValue != self->value) {
        self->value = newValue;

        LCDBitmap *oldImage = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(oldImage);

        drawAndMove(self, sprite, newValue);
    } else {
        const MELPoint origin = self->super.frame.origin;
        playdate->sprite->moveTo(sprite, origin.x, origin.y);
    }
}

static void updateUInt8(LCDSprite * _Nonnull sprite) {
    Counter *self = playdate->sprite->getUserdata(sprite);
    uint8_t *newValuePointer = self->super.userdata;
    const uint8_t newValue = *newValuePointer;
    if (newValue != self->value) {
        self->value = newValue;

        LCDBitmap *oldImage = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(oldImage);

        drawAndMove(self, sprite, newValue);
    } else {
        const MELPoint origin = self->super.frame.origin;
        playdate->sprite->moveTo(sprite, origin.x, origin.y);
    }
}

static void updateFloat(LCDSprite * _Nonnull sprite) {
    Counter *self = playdate->sprite->getUserdata(sprite);
    float *newValuePointer = self->super.userdata;
    const float newValue = *newValuePointer;
    if (newValue != self->value) {
        self->value = newValue;

        LCDBitmap *oldImage = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(oldImage);

        drawAndMove(self, sprite, newValue);
    } else {
        const MELPoint origin = self->super.frame.origin;
        playdate->sprite->moveTo(sprite, origin.x, origin.y);
    }
}
