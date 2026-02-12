//
//  endingscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 30/03/2024.
//

#include "endingscene.h"

#include "music.h"
#include "layersprite.h"
#include "titlescene.h"

#include "../gen/spritestoryselectcharacter.h"

static const MapName mapName = MapNameEndingWalk;

static const float speed = 48.0f;
static const float mapWidth = 32.0f * 42.0f;

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull self);
static int update(void * _Nonnull userdata);

static void updateWalking(LCDSprite * _Nonnull sprite);
static void lockCamera(LCDSprite * _Nonnull sprite);

/**
 * Fin
 *
 * Poker Poker Magic
 * © 2024
 *
 * A game by Raphaël Calabro
 *
 * Credits
 *
 * Art, code, audio, and texts:
 * Raphaël Calabro
 *
 * Japanese Text Proofreading:
 * Ayuchi Calabro
 *
 * Title Screen Doodles:
 * Mylène Calabro
 *
 * Special Thanks:
 * Twin Cities Playtest
 * IrishJiminy
 * Kirkpad
 *
 * And you!
 */

EndingScene * _Nonnull EndingSceneAlloc(void) {
    EndingScene *self = playdate->system->realloc(NULL, sizeof(EndingScene));
    *self = (EndingScene) {
        .super = {
            .type = SceneTypeEnding,
            .init = init,
            .dealloc = dealloc,
            .update = update,
        },
        .map = MELMapOpen(kMapNameFileNames[mapName]),
    };
    return self;
}

static void init(MELScene * _Nonnull scene) {
    EndingScene *self = (EndingScene *)scene;
    MELMap *map = self->map;
    for (unsigned int index = 0; index < map->layerCount; index++) {
        MELLayer *layer = map->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);
        LCDSpriteRefListPush(&self->super.sprites, LayerSpriteConstructor(layer, image));
    }

    MELSpriteInstanceList instances = map->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        if (instance.name == SpriteNameStorySelectCharacter) {
            continue;
        }
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette == NULL) {
            definition->palette = SpriteNameLoadBitmapTable(instance.name);
        }
        MELSprite *melSprite = new(MELSprite);
        LCDSprite *sprite = MELSpriteInit(melSprite, definition, instances.memory + index);
        playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
        playdate->sprite->setZIndex(sprite, 99);
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    const int completed = (currentSaveGame.completion[0] != 0)
        + (currentSaveGame.completion[1] != 0)
        + (currentSaveGame.completion[2] != 0)
        + (currentSaveGame.completion[3] != 0)
        + (currentSaveGame.completion[4] != 0);

    const float firstCharacterX = mapWidth + 48.0f * completed - 32.0f;
    static const float spaceBetweenCharacters = 48.0f;
    static const float y = 96.0f;

    // Personnages qui marchent
    float x = firstCharacterX;
    for (unsigned int index = 0; index < kStoryCount; index++) {
        if (currentSaveGame.completion[index]) {
            const Story *story = kStories[index];
            const Character character = story->character;
            loadSpriteStorySelectCharacterPalette();
            MELSprite *melSprite = new(MELSprite);
            LCDSprite *sprite =  MELSpriteInitWithCenter(melSprite, &spriteStorySelectCharacter, (MELPoint) { .x = x, .y = y });
            melSprite->direction = MELDirectionLeft;
            MELSpriteSetAnimationAndDirection(melSprite, CharacterAnimationName[character], MELAnimationDirectionRight);
            playdate->sprite->setUpdateFunction(sprite, updateWalking);
            playdate->sprite->setZIndex(sprite, 90);

            LCDSpriteRefListPush(&self->super.sprites, sprite);
            x -= spaceBetweenCharacters;

            if (self->trailingCharacter == NULL) {
                self->trailingCharacter = melSprite;
            }
        }
    }
    // TODO: Faire Crocoford et Belleplume qui courent quand vrai fin

    // Caméra
    MELSprite *cameraSprite = new(MELSprite);
    *cameraSprite = (MELSprite) {
        .class = &MELSpriteClassDefault,
        .userdata = self->trailingCharacter,
    };
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUserdata(sprite, cameraSprite);
    playdate->sprite->setUpdateFunction(sprite, lockCamera);
    playdate->sprite->setVisible(sprite, false);
    playdate->sprite->addSprite(sprite);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    MusicManagerPlay("musics/ending", 1, 1.0f);

    camera.frame.origin = MELPointZero;
    setRefreshRate(DEFAULT_REFRESH_RATE);
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeEnding) {
        playdate->system->error("Given scene is not a EndingScene instance");
        return;
    }
    EndingScene *self = (EndingScene *)scene;

    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->map);
    playdate->system->realloc(self, 0);
}

#pragma mark - Personnages

static void updateWalking(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->frame.origin.x -= DELTA * speed;
    MELSpriteUpdate(sprite);
    playdate->sprite->setDrawMode(sprite, kDrawModeFillBlack);
}

static void lockCamera(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELSprite *center = self->userdata;
    camera.frame.origin.x = MELFloatBound(0.0f, center->frame.origin.x - 360.0f, mapWidth - LCD_COLUMNS);
}

#pragma mark - Update

static int update(void * _Nonnull userdata) {
    EndingScene *self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    if (self->trailingCharacter->frame.origin.x < -self->trailingCharacter->frame.size.width) {
        TitleScene *titleScene = TitleSceneAlloc();
        titleScene->initialMenuSelection = TitleMenuItemStory;
        MELSceneMakeCurrent(&titleScene->super);
        return true;
    }

    const MELController controller = MELControllerMake();
    if (controller.pressedB) {
        TitleScene *titleScene = TitleSceneAlloc();
        titleScene->initialMenuSelection = TitleMenuItemStory;
        MELSceneMakeCurrent(&titleScene->super);
        return true;
    }
    return true;
}
