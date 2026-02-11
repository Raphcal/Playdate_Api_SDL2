//
//  layersprite.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/07/2023.
//

#include "layersprite.h"

#include "image.h"
#include "gamescene.h"
#include "../lib/camera.h"

typedef struct {
    MELSprite super;
    MELPoint scrollRate;
    MELIntPoint topLeft;
    MELIntSize size;
    MELCamera * _Nullable camera;
} LayerSprite;

static void update(LCDSprite * _Nonnull sprite);
static void updateLoop(LCDSprite * _Nonnull sprite);
static void dealloc(LCDSprite * _Nonnull sprite);

const MELSpriteClass LayerSpriteClass = (MELSpriteClass) {
    .destroy = dealloc,
};

static LCDSprite * _Nonnull LayerSpriteConstructorPrivate(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, MELIntPoint topLeft);

LCDSprite * _Nonnull LayerSpriteConstructor(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image) {
    LCDSprite *sprite = LayerSpriteConstructorPrivate(layer, image, MELIntPointZero);
    playdate->sprite->setUpdateFunction(sprite, update);
    return sprite;
}

LCDSprite * _Nonnull LayerSpriteConstructorLooping(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, MELIntPoint topLeft) {
    LCDSprite *sprite = LayerSpriteConstructorPrivate(layer, image, topLeft);
    playdate->sprite->setUpdateFunction(sprite, updateLoop);
    return sprite;
}

static LCDSprite * _Nonnull LayerSpriteConstructorPrivate(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, MELIntPoint topLeft) {
    LayerSprite *self = playdate->system->realloc(NULL, sizeof(LayerSprite));

    const int layerIndex = (int) (layer - layer->parent->layers);
    MELIntSize tileSize = layer->parent->tileSize;
    MELIntRectangle layerFrame = layer->frame;
    MELPoint origin = (MELPoint) {
        .x = layerFrame.origin.x * tileSize.width,
        .y = layerFrame.origin.y * tileSize.height
    };

    *self = (LayerSprite) {
        .super = {
            .class = &LayerSpriteClass,
            .frame = {
                .size = {
                    .width = layerFrame.size.width * tileSize.width,
                    .height = layerFrame.size.height * tileSize.height
                },
                .origin = origin,
            },
        },
        .scrollRate = layer->scrollRate,
        .topLeft = topLeft,
        .size = layer->parent->size,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_BG + ZINDEX_LAYER_MULTIPLIER * layerIndex);
    playdate->sprite->addSprite(sprite);
    return sprite;
}

void LayerSpriteSetCamera(LCDSprite * _Nonnull sprite, MELCamera * _Nullable camera) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);
    self->camera = camera;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);
    if (MELIntPointEquals(self->topLeft, MELIntPointZero)) {
        LCDBitmap *bitmap = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(bitmap);
    }
    MELSpriteDealloc(sprite);
}

/**
 * Fait un scrolling sans répétition pour une carte de n'importe quelle taille.
 *
 * @param sprite Couche d'une carte.
 */
static void update(LCDSprite * _Nonnull sprite) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);

    const MELRectangle frame = self->super.frame;
    const MELPoint scrollRate = self->scrollRate;

    playdate->sprite->moveTo(sprite, frame.origin.x - camera.frame.origin.x * scrollRate.x + frame.size.width / 2.0f, frame.origin.y - camera.frame.origin.y * scrollRate.y + frame.size.height / 2.0f);
}

/**
 * Fait un scrolling infini horizontalement pour une carte faisant 768 pixels de large.
 *
 * @param sprite Couche d'une carte.
 */
static void updateLoop(LCDSprite * _Nonnull sprite) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);

    const MELRectangle frame = self->super.frame;
    const MELPoint scrollRate = self->scrollRate;
    const MELIntPoint topLeft = self->topLeft;
    const MELIntSize size = self->size;
    const MELCamera cameraToUse = self->camera ? *self->camera : camera;
    const int32_t left = (int32_t)(frame.origin.x - cameraToUse.frame.origin.x * scrollRate.x) % size.width;
    const int32_t top = (int32_t)(frame.origin.y - cameraToUse.frame.origin.y * scrollRate.y) % size.height;

    playdate->sprite->moveTo(sprite, left + topLeft.x + frame.size.width / 2.0f, top + topLeft.y + frame.size.height / 2.0f);
}
