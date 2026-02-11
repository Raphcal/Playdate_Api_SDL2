//
//  layersprite.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/07/2023.
//

#include "layersprite.h"

#include "image.h"
#include "gamescene.h"
#include "camera.h"

typedef struct {
    MELSprite super;
    MELPoint scrollRate;
    int32_t leftPadding;
} LayerSprite;

static const int32_t kMapWidth = 768;

static void update(LCDSprite * _Nonnull sprite);
static void update768(LCDSprite * _Nonnull sprite);
static void dealloc(LCDSprite * _Nonnull sprite);

const MELSpriteClass LayerSpriteClass = (MELSpriteClass) {
    .destroy = dealloc,
};

LCDSprite * _Nonnull LayerSpriteConstructor(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, MELBoolean isRepeat) {
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
                    .height = layerFrame.size.height * tileSize.width
                },
                .origin = origin,
            },
        },
        .scrollRate = layer->scrollRate,
        .leftPadding = isRepeat * kMapWidth,
    };

    const int32_t mapWidth = layer->parent->size.width;
#if CHECK_LAYERSPRITE_MAP_WIDTH
    if (isRepeat && mapWidth != kMapWidth) {
        playdate->system->error("LayerSpriteConstructor: expected mapWidth of %d but was: %d", kMapWidth, mapWidth);
    }
#endif

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, mapWidth == kMapWidth ? update768 : update);
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_BG + layerIndex);
    playdate->sprite->addSprite(sprite);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push LayerSprite(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    if (currentScene->type != SceneTypeGame) {
        LCDSpriteRefListPush(&currentScene->sprites, sprite);
    }
    return sprite;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);
    if (self->leftPadding == 0.0f) {
        LCDBitmap *bitmap = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(bitmap);
    }
    if (currentScene->type == SceneTypeGame) {
        GameScene *gameScene = GameSceneGetCurrent();
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        playdate->system->logToConsole("LayerSprite#dealloc(%x, %x): %d", sprite, self, self->super.definition.name);
        int index = LCDSpriteRefListRemoveSwapEntry(&gameScene->layers, sprite);
        if (index < 0) {
            playdate->system->logToConsole("LCDSpriteRefListRemoveSwapEntry: sprite %x not found (%d)", sprite, index);
        }
#else
        LCDSpriteRefListRemoveSwapEntry(&gameScene->layers, sprite);
#endif
        playdate->system->realloc(self, 0);
        playdate->sprite->removeSprite(sprite);
        playdate->sprite->freeSprite(sprite);
    } else {
        MELSpriteDealloc(sprite);
    }
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
static void update768(LCDSprite * _Nonnull sprite) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);

    const MELRectangle frame = self->super.frame;
    const MELPoint scrollRate = self->scrollRate;
    const int32_t leftPadding = self->leftPadding;
    const int32_t left = (int32_t)(frame.origin.x - camera.frame.origin.x * scrollRate.x) % kMapWidth;

    playdate->sprite->moveTo(sprite, left + leftPadding + frame.size.width / 2.0f, frame.origin.y + camera.frame.origin.y * scrollRate.y + frame.size.height / 2.0f);
}
