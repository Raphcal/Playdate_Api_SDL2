//
//  mapscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 03/04/2024.
//

#include "mapscene.h"

#include "music.h"
#include "image.h"
#include "dialogscene.h"
#include "map_player.h"
#include "savegame.h"
#include "../gen/spritemap.h"
#include "../gen/spritemappin.h"

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

MapScene * _Nonnull MapSceneAlloc(const Story * _Nullable story, unsigned int encounterIndex) {
    MapScene *self = playdate->system->realloc(NULL, sizeof(MapScene));
    *self = (MapScene) {
        .super = {
            .type = SceneTypeMap,
            .dealloc = dealloc,
            .init = init,
            .update = update,
        },
        .story = story,
        .encounterIndex = encounterIndex,
    };
    return self;
}

#pragma mark - Fonctions privées

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeMap) {
        playdate->system->error("Given scene is not a MapScene instance");
        return;
    }
    MapScene *self = (MapScene *)scene;

    MELEventBusRemoveListener(EventDialogHasEnded, self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    playdate->system->realloc(self, 0);
}

static void init(MELScene * _Nonnull scene) {
    MapScene *self = (MapScene *)scene;

    ImageConstructorWithSpritePalette((MELPoint) {
        .x = spriteMap.size.width / 2,
        .y = spriteMap.size.height / 2,
    }, SpriteNameMap, 0);

    EncounterList encounters = self->story->encounters;
    loadSpriteMapPinPalette();
    MELPoint lastPin = encounters.memory[0].pin;
    const unsigned int encounterIndex = self->encounterIndex;

    for (unsigned int index = 0; index < encounters.count; index++) {
        const MELPoint pin = encounters.memory[index].pin;
        if (!MELPointEquals(pin, lastPin) && lastPin.x && lastPin.y) {
            const int bitmapIndex = index <= encounterIndex;
            LCDBitmap *image = playdate->graphics->getTableBitmap(spriteMapPin.palette, bitmapIndex);
            ImageConstructor(lastPin, image);
        }
        lastPin = pin;
    }

    MapPlayerConstructorWithCharacterAndFromAndTo(self->story->character, encounters.memory[MELIntMax(encounterIndex - 1, 0)].pin, encounters.memory[encounterIndex].pin);

    MusicManagerPlay("musics/map", 1, 0.0f);

    setRefreshRate(DEFAULT_REFRESH_RATE);
}

static int update(void * _Nonnull userdata) {
    MapScene *self = userdata;
    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;
    playdate->sprite->updateAndDrawSprites();

    self->time += DELTA;

    const MELBoolean goToNextScene =  MELControllerMake().pressedB;
    if (goToNextScene || self->time >= 5.0f) {
        DialogScene *dialogScene = DialogSceneAlloc(self->story, self->encounterIndex);
        MELSceneMakeCurrent(&dialogScene->super);
        return true;
    }
    return true;
}
