//
//  fadeexplosion.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 06/11/2023.
//

#include "fadeexplosion.h"

#include "image.h"

static void update(LCDSprite * _Nonnull sprite);

const SpriteName kSpriteNameFadeExplosion = SpriteNameDokan;

LCDSprite * _Nonnull FadeExplosionConstructor(MELPoint origin) {
    // Image faisant la taille de l'écran.
    LCDBitmap *image = playdate->graphics->newBitmap(LCD_COLUMNS, LCD_ROWS, kColorClear);
    LCDSprite *sprite = ImageConstructor(MELPointZero, image);
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setZIndex(sprite, ZINDEX_FADE);
    playdate->sprite->moveTo(sprite, LCD_COLUMNS / 2, LCD_ROWS / 2);

    const float maxWidth = MELFloatMax(origin.x, LCD_COLUMNS - origin.x) * 2.0f;
    const float maxHeight = MELFloatMax(origin.y, LCD_ROWS - origin.y) * 2.0f;
    const float size = MELFloatMax(maxWidth, maxHeight);

    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->definition.name = kSpriteNameFadeExplosion;
    self->frame = (MELRectangle) {
        .origin = origin,
        .size = {
            .width = size,
            .height = size
        }
    };
    return sprite;
}

static void draw(MELSprite * _Nonnull self, LCDSprite * _Nonnull sprite, float progress) {
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->pushContext(image);

    playdate->graphics->clear(kColorClear);

    const float rotation = progress * MEL_PI * 2.0f;
    MELRectangle frame = self->frame;
    frame.size.width *= progress;
    frame.size.height *= progress;
    MELQuadrilateral quadrilateral = MELRectangleRotateWithPivot(frame, rotation, frame.origin);
    int points[] = {
        quadrilateral.topLeft.x, quadrilateral.topLeft.y,
        quadrilateral.topRight.x, quadrilateral.topRight.y,
        quadrilateral.bottomRight.x, quadrilateral.bottomRight.y,
        quadrilateral.bottomLeft.x, quadrilateral.bottomLeft.y,
    };
    playdate->graphics->fillPolygon(4, points, kColorWhite, kPolygonFillNonZero);

    playdate->graphics->popContext();

    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->markDirty(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float duration = 1.5f;
    if (self->hitTimer < duration) {
        const float newTime = self->hitTimer = MELFloatMin(self->hitTimer + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        draw(self, sprite, progress);
        return;
    }

    MELSpriteDealloc(sprite);
}
