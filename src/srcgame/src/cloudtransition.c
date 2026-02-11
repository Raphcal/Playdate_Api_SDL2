//
//  cloudtransition.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/08/2023.
//

#include "cloudtransition.h"

#include "camera.h"
#include "image.h"
#include "layersprite.h"
#include "../lib/subsprite.h"

struct cloud_transition {
    MELSprite super;

    uint8_t statePart;
    MELSubSprite * _Nullable bottom;
    float time;
    MELSubSprite * _Nullable top;

    GameScene * _Nonnull gameScene;
    MapName mapName;
    MELMap * _Nullable map;
    LCDBitmap * _Nonnull * _Nullable images;
    unsigned int imageIndex;
};

static const int kCloudWidth = 768;
static const float kTransitionDuration = 4.0f;

static MELSubSprite * _Nonnull createSubSprite(LCDBitmap * _Nonnull cloud, float y);

static void mainStatePart0(LCDSprite * _Nonnull sprite);
static void updateSubSprite(LCDSprite * _Nonnull sprite);

static void destroy(LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);

static const MELSpriteClass CloudTransitionClass = (MELSpriteClass) {
    .name = SpriteClassNameCloudTransition,
    .destroy = destroy,
    .save = save,
    .load = CloudTransitionLoader,
};

LCDSprite * _Nonnull CloudTransitionConstructor(MapName nextMapName, GameScene * _Nonnull gameScene) {
    struct cloud_transition *self = playdate->system->realloc(NULL, sizeof(struct cloud_transition));
    LCDBitmap *cloud = gameScene->cloud;
    LCDSprite *sprite = ImageConstructorWithSelf(&self->super, (MELPoint) {
        .x = LCD_COLUMNS /* screenWidth */ + kCloudWidth / 2.0f,
        .y = LCD_ROWS /* screenHeight */ / 2.0f
    }, cloud);
    // Modification du z-index avant de créer les sous sprites pour que l'update de ce sprite soit appelé avant ceux des sous sprites.
    playdate->sprite->setZIndex(sprite, ZINDEX_BG_GRID);
    self->super.class = &CloudTransitionClass;
    self->top = createSubSprite(cloud, LCD_ROWS / 6.0f);
    self->bottom = createSubSprite(cloud, 5.0f * LCD_ROWS / 6.0f);
    self->statePart = 0;
    self->gameScene = gameScene;
    MELMap *map = MELMapOpen(kMapNameFileNames[nextMapName]);
    self->map = map;
    self->mapName = nextMapName;
    self->images = playdate->system->realloc(NULL, sizeof(LCDBitmap *) * map->layerCount);
    self->imageIndex = 0;
    self->time = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart0);
    return sprite;
}

const MELSpriteClass * _Nonnull CloudTransitionGetClass(void) {
    return &CloudTransitionClass;
}

static MELSubSprite * _Nonnull createSubSprite(LCDBitmap * _Nonnull cloud, float y) {
    MELSubSprite *self = playdate->system->realloc(NULL, sizeof(MELSubSprite));
    LCDSprite *sprite = ImageConstructorWithSelfDontPush(&self->super, (MELPoint) {
        .x = LCD_COLUMNS /* screenWidth */ + kCloudWidth / 2,
        .y = y
    }, cloud);
    self->sprite = sprite;
    playdate->sprite->setZIndex(sprite, ZINDEX_BG_GRID);
    playdate->sprite->setUpdateFunction(sprite, updateSubSprite);
    return self;
}

static void destroy(LCDSprite * _Nonnull sprite) {
    struct cloud_transition *self = playdate->sprite->getUserdata(sprite);
    playdate->sprite->setUpdateFunction(self->bottom->sprite, MELSubSpriteDealloc);
    playdate->sprite->setUpdateFunction(self->top->sprite, MELSubSpriteDealloc);
    playdate->system->realloc(self->images, 0);
    MELSpriteDealloc(sprite);
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    struct cloud_transition *self = (struct cloud_transition *)sprite;
    MELOutputStreamWriteByte(outputStream, self->statePart);
    MELSubSpriteSave(self->bottom, outputStream);
    MELOutputStreamWriteFloat(outputStream, self->time);
    MELSubSpriteSave(self->top, outputStream);
    MELOutputStreamWriteByte(outputStream, self->mapName);
}

MELSprite * _Nullable CloudTransitionLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    struct cloud_transition *self = playdate->system->realloc(NULL, sizeof(struct cloud_transition));
    const uint8_t statePart = MELInputStreamReadByte(inputStream);
    MELSubSprite *bottom = MELSubSpriteLoad(inputStream);
    const float time = MELInputStreamReadFloat(inputStream);
    MELSubSprite *top = MELSubSpriteLoad(inputStream);
    const MapName mapName = MELInputStreamReadByte(inputStream);
    MELMap *map = MELMapOpen(kMapNameFileNames[mapName]);

    GameScene *gameScene = GameSceneGetCurrent();
    if (!gameScene->cloud) {
        gameScene->cloud = LCDBitmapLoadOrError("gui-cloud");
    }
    LCDBitmap *cloud = gameScene->cloud;
    playdate->sprite->setImage(sprite, cloud, kBitmapUnflipped);
    playdate->sprite->setImage(top->sprite, cloud, kBitmapUnflipped);
    playdate->sprite->setImage(bottom->sprite, cloud, kBitmapUnflipped);

    playdate->sprite->setUpdateFunction(sprite, mainStatePart0);
    playdate->sprite->setUpdateFunction(top->sprite, updateSubSprite);
    playdate->sprite->setUpdateFunction(bottom->sprite, updateSubSprite);

    *self = (struct cloud_transition) {
        .super = {
            .class = &CloudTransitionClass
        },
        .statePart = statePart,
        .bottom = bottom,
        .time = time,
        .top = top,
        .gameScene = gameScene,
        .mapName = mapName,
        .map = map,
        .images = playdate->system->realloc(NULL, sizeof(LCDBitmap *) * map->layerCount),
    };
    return &self->super;
}

static void draw(MELSprite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELPoint origin = self->frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
}

static void mainStatePart0(LCDSprite * _Nonnull sprite) {
    struct cloud_transition *self = playdate->sprite->getUserdata(sprite);

    MELMap *map = self->map;
    if (map && self->imageIndex < map->layerCount) {
        const unsigned int imageIndex = self->imageIndex;
        self->images[imageIndex] = loadMapLayer(self->mapName, imageIndex);
        self->imageIndex = imageIndex + 1;
    }

    // during
    if (self->time < kTransitionDuration) {
        const float newTime = MELFloatMin(self->time + DELTA, kTransitionDuration);
        const float progress = newTime / kTransitionDuration;
        self->time = newTime;

        if (progress >= 0.5f && map && self->imageIndex == map->layerCount) {
            GameScene *gameScene = self->gameScene;
            MELMapDealloc(gameScene->background);
            gameScene->background = map;
            gameScene->backgroundName = self->mapName;
            self->map = NULL;

            LCDSpriteRefList layers = gameScene->layers;
            for (unsigned int index = 0; index < layers.count; index++) {
                LCDSpriteRefDeinit(layers.memory + index);
            }
            gameScene->layers.count = 0;
            const unsigned int layerCount = map->layerCount;
            for (unsigned int index = 0; index < layerCount; index++) {
                MELLayer *layer = map->layers + index;
                LCDBitmap *image = self->images[index];
                LCDSpriteRefListPush(&gameScene->layers, LayerSpriteConstructor(layer, image, false));
                LCDSpriteRefListPush(&gameScene->layers, LayerSpriteConstructor(layer, image, true));
            }
        }

        self->super.frame.origin.x = 784 + -1168 * progress;
        self->top->super.frame.origin.x = 784 + -1403.2f * progress;
        self->bottom->super.frame.origin.x = 784 + -1324.8f * progress;
        draw(&self->super, sprite);
        return;
    }
    GameSceneNextWaveWithGameScene(self->gameScene);
    destroy(sprite);
}

static void updateSubSprite(LCDSprite * _Nonnull sprite) {
    MELSubSprite *self = playdate->sprite->getUserdata(sprite);
    draw(&self->super, sprite);
}
