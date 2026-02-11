//
//  selectfile_card.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/03/2024.
//

#include "selectfile_card.h"
#include "image.h"

#include "../gen/spritefileselectcaroline.h"

typedef struct {
    MELSprite super;
    float time;
    float maxAngle;
    int8_t * _Nonnull selectedSaveGame;
    int8_t saveIndex;
    MELBoolean selected;
} SelectFileCard;

static const float cardSize = 0.5f;
static const float distance = 2.0915926f;
static const float duration = 0.6f;

static float maxAngle(uint8_t saveIndex, uint8_t saveCount);

static void draw(SelectFileCard * _Nonnull self, LCDBitmap * _Nonnull image);

static void updateNoop(LCDSprite * _Nonnull sprite);
static void updateSliding(LCDSprite * _Nonnull sprite);

static const MELSpriteClass SelectFileCardClass = (MELSpriteClass) {
    .name = SpriteClassNameSelectFileCard,
    .destroy = ImageDealloc,
};

LCDSprite * _Nonnull SelectFileCardConstructor(MELRectangle frame, int8_t saveIndex, int8_t saveCount, int8_t * _Nonnull selectedSaveGame) {
    SelectFileCard *self = playdate->system->realloc(NULL, sizeof(SelectFileCard));
    *self = (SelectFileCard) {
        .super = {
            .class = &SelectFileCardClass,
            // 200x20
            .frame = frame,
        },
        .selectedSaveGame = selectedSaveGame,
        .saveIndex = saveIndex,
        .maxAngle = maxAngle(saveIndex, saveCount),
    };
    LCDBitmap *image = playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorClear);
    draw(self, image);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setUpdateFunction(sprite, updateNoop);
    playdate->sprite->addSprite(sprite);
    playdate->sprite->setZIndex(sprite, 0);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

void SelectFileCardsSlide(void) {
    LCDSpriteRefList sprites = currentScene->sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->class->name == SpriteClassNameSelectFileCard) {
            SelectFileCard *self = (SelectFileCard *)melSprite;
            self->time = 0.0f;
            playdate->sprite->setUpdateFunction(sprite, updateSliding);
        }
    }
}

void SelectFileCardsReset(void) {
    LCDSpriteRefList sprites = currentScene->sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->class->name == SpriteClassNameSelectFileCard) {
            SelectFileCard *self = (SelectFileCard *)melSprite;
            self->time = 0.0f;
            draw(self, playdate->sprite->getImage(sprite));
            playdate->sprite->markDirty(sprite);
            playdate->sprite->setUpdateFunction(sprite, updateNoop);
        }
    }
}

void SelectFileCardsSkipAnimation(LCDSprite * _Nonnull sprite) {
    SelectFileCard *self = playdate->sprite->getUserdata(sprite);
    self->time = duration;
    draw(self, playdate->sprite->getImage(sprite));
    playdate->sprite->markDirty(sprite);
    playdate->sprite->setUpdateFunction(sprite, updateNoop);
}

const MELSpriteClass * _Nonnull SelectFileCardsGetClass(void) {
    return &SelectFileCardClass;
}

static float maxAngle(uint8_t saveIndex, uint8_t saveCount) {
    return (MEL_PI / (saveCount + 2)) * (saveCount - saveIndex) + cardSize / 4.0f;
}

static void draw(SelectFileCard * _Nonnull self, LCDBitmap * _Nonnull image) {
    const float progress = MELEaseInOut(0, duration, self->time);
    const float angle = MELFloatMin(cardSize / 2 + distance * progress, self->maxAngle);
    playdate->graphics->clearBitmap(image, kColorClear);
    playdate->graphics->pushContext(image);
    const float cosAngle1 = cosf(angle);
    const float sinAngle1 = sinf(angle);
    const float cosAngle2 = cosf(angle + cardSize);
    const float sinAngle2 = sinf(angle + cardSize);
    MELPoint center = (MELPoint) {
        .x = self->super.frame.size.width / 2.0f,
        .y = self->super.frame.size.height / 2.0f,
    };
    int polygon[] = {
        center.x + 50.0f /* inner.x */ * cosAngle1,
        center.y + -10 /* innerTop */ + 10.0f /* inner.y */ * sinAngle1,

        center.x + 50.0f /* inner.x */ * cosAngle2,
        center.y + -10 /* innerTop */ + 10.0f /* inner.y */ * sinAngle2,

        center.x + 100.0f /* outer.x */ * cosAngle2,
        center.y + 20.0f /* outer.y */ * sinAngle2,

        center.x + 100.0f /* outer.x */ * cosAngle1,
        center.y + 20.0f /* outer.y */ * sinAngle1,
    };
    LCDColor fillColor, strokeColor;
    int strokeSize;
    if (self->selected) {
        fillColor = kColorWhite;
        strokeColor = kColorBlack;
        strokeSize = 2;
    } else {
        fillColor = kColorBlack;
        strokeColor = kColorWhite;
        strokeSize = 1;
    }
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->fillPolygon(4, polygon, fillColor, kPolygonFillEvenOdd);
    playdate->graphics->drawLine(polygon[0], polygon[1], polygon[2], polygon[3], strokeSize, strokeColor);
    playdate->graphics->drawLine(polygon[2], polygon[3], polygon[4], polygon[5], strokeSize, strokeColor);
    playdate->graphics->drawLine(polygon[4], polygon[5], polygon[6], polygon[7], strokeSize, strokeColor);
    playdate->graphics->drawLine(polygon[6], polygon[7], polygon[0], polygon[1], strokeSize, strokeColor);
    playdate->graphics->popContext();
}

static void updateNoop(LCDSprite * _Nonnull sprite) {
    SelectFileCard *self = playdate->sprite->getUserdata(sprite);
    MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);

    MELBoolean selected = self->saveIndex == *self->selectedSaveGame;
    if (selected != self->selected) {
        self->selected = selected;
        draw(self, playdate->sprite->getImage(sprite));
        playdate->sprite->markDirty(sprite);
    }
}

static void updateSliding(LCDSprite * _Nonnull sprite) {
    SelectFileCard *self = playdate->sprite->getUserdata(sprite);

    const float time = self->time;
    if (time <= duration) {
        self->time = time + DELTA;
        draw(self, playdate->sprite->getImage(sprite));
        playdate->sprite->markDirty(sprite);
    } else {
        playdate->sprite->setUpdateFunction(sprite, updateNoop);
    }

    MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
}
