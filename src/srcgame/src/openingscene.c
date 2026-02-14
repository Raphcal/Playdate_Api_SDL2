//
//  openingscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 27/07/2024.
//

#include "openingscene.h"

#include "music.h"
#include "fade.h"
#include "stride.h"
#include "card.h"
#include "titlescene.h"
#include "image.h"
#include "layersprite.h"
#include "carddeck.h"

#include "../gen/spriteopeningcavehat.h"
#include "../gen/spriteopeningcaroline.h"
#include "../gen/spriteopeningcarolinerunning.h"
#include "../gen/spriteopeningclovis.h"
#include "../gen/spriteopeningceres.h"
#include "../gen/spriteopeningceresflying.h"
#include "../gen/spriteopeningbelleplume.h"
#include "../gen/spriteopeningcrocoford.h"
#include "../gen/spriteopeningsapique.h"
#include "../gen/spriteopeninggarouge.h"
#include "../gen/spriteopeningeljoker.h"
#include "../gen/spriteopeningprince.h"
#include "../gen/spriteopeningorelia.h"
#include "../gen/spritecarolinedanse.h"
#include "../gen/spritefileselectcard.h"
#include "../gen/spriteblack.h"
#include "../gen/spritetitle.h"
#include "../gen/spritetitlemagichat.h"
#include "../gen/spritetitlesparks.h"
#include "../gen/spritechip18.h"

static const float kFadeDuration = 0.15f;
static const float kIsometricRunDuration = 2.75f;

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull self);
static int update(void * _Nonnull userdata);
static int updateFadeIn(void * _Nonnull userdata);
static int updateFadeOut(void * _Nonnull userdata);

static int updateFadeInCards(void * _Nonnull userdata);
static int updateFadeOutCards(void * _Nonnull userdata);

#if ENABLE_OPENING_SHADE
static void updateAndShadeSprite(LCDSprite * _Nonnull sprite);
#endif
static void updateChips(LCDSprite * _Nonnull sprite);

static void fadeTo(OpeningScene * _Nonnull self, OpeningSceneState state);
static void loadNextState(OpeningScene * _Nonnull self);

static void updateJump(LCDSprite * _Nonnull sprite);

static void moveCameraRight(LCDSprite * _Nonnull sprite);
static void moveCameraDown(LCDSprite * _Nonnull sprite);
static void moveCameraRightAndDown(LCDSprite * _Nonnull sprite);

typedef struct {
    MELPoint speed;
    float time;
} CardJump;

OpeningScene * _Nonnull OpeningSceneAlloc(void) {
    OpeningScene *self = new(OpeningScene);
    *self = (OpeningScene) {
        .super = {
            .type = SceneTypeOpening,
            .init = init,
            .dealloc = dealloc,
            .update = update,
            .sprites = LCDSpriteRefListEmpty,
        },
        .forestMap = MELMapOpen(kMapNameFileNames[MapNameOpeningForest]),
        .horizontalMap = MELMapOpen(kMapNameFileNames[MapNameIntroSeq1]),
        .verticalMap = MELMapOpen(kMapNameFileNames[MapNameIntroSeq2]),
        .danseBackground = MELMapOpen(kMapNameFileNames[MapNameIntro]),
    };
    return self;
}

static void init(MELScene * _Nonnull scene) {
    OpeningScene *self = (OpeningScene *)scene;

    camera.frame.origin = MELPointZero;

    setRefreshRate(DEFAULT_REFRESH_RATE);

    fadeTo(self, OpeningSceneStateIntro);
    loadNextState(self);
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeOpening) {
        playdate->system->error("Given scene is not an OpeningScene instance");
        return;
    }
    OpeningScene *self = (OpeningScene *)scene;
    MELEventBusRemoveListeners(self);
    LCDSpriteRefListDeallocReverse(&self->super.sprites);

    MELSpriteDefinitionFreePalette(&spriteBlack);
    MELSpriteDefinitionFreePalette(&spriteOpeningCaroline);
    MELSpriteDefinitionFreePalette(&spriteOpeningCarolineRunning);
    MELSpriteDefinitionFreePalette(&spriteOpeningClovis);
    MELSpriteDefinitionFreePalette(&spriteOpeningCeres);
    MELSpriteDefinitionFreePalette(&spriteOpeningCeresFlying);
    MELSpriteDefinitionFreePalette(&spriteOpeningCrocoford);
    MELSpriteDefinitionFreePalette(&spriteOpeningBelleplume);
    MELSpriteDefinitionFreePalette(&spriteOpeningSapique);
    MELSpriteDefinitionFreePalette(&spriteOpeningGarouge);
    MELSpriteDefinitionFreePalette(&spriteOpeningOrelia);
    MELSpriteDefinitionFreePalette(&spriteOpeningPrince);
    MELSpriteDefinitionFreePalette(&spriteOpeningElJoker);

    MELMapDealloc(self->forestMap);
    MELMapDealloc(self->horizontalMap);
    MELMapDealloc(self->verticalMap);
    MELMapDealloc(self->danseBackground);

    playdate->system->realloc(self, 0);
}

static void loadStateIntro(OpeningScene * _Nonnull self) {
    self->duration = kIsometricRunDuration;

    // Fond
    MELSprite *melSprite = new(MELSprite);
    LCDSprite *sprite = MELSpriteInitWithCenter(melSprite, &spriteTitle, MELPointZero);
    StrideSpriteTo(sprite, MELPointMake(0, 144.0f), 0.5f, 1.0f);
    playdate->sprite->setUpdateFunction(sprite, moveCameraRightAndDown);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateIntro: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    sprite = LayerSpriteConstructor(self->forestMap->layers + 0, loadMapLayer(MapNameOpeningForest, 0));
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateIntro: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);
    for (unsigned int index = 1; index < self->forestMap->layerCount; index++) {
        LCDBitmap *bitmap = loadMapLayer(MapNameOpeningForest, index);

        sprite = LayerSpriteConstructorLooping(self->forestMap->layers + index, bitmap, MELIntPointZero);
        LayerSpriteSetCamera(sprite, &self->camera);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateIntro: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    const int zIndex = (self->forestMap->layerCount - 1) * 2 + 1;

    // Caroline
    loadSpriteOpeningCarolineRunningPalette();
    sprite = StrideConstructor(&spriteOpeningCarolineRunning, MELPointMake(-24, 200 - 144), MELPointMake(360, 410 - 144), 1.5f, 3.0f);
    playdate->sprite->setZIndex(sprite, zIndex);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateIntro: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // Cérès
    loadSpriteOpeningCeresFlyingPalette();
    sprite = StrideConstructor(&spriteOpeningCeresFlying, MELPointMake(-45, 210 - 144), MELPointMake(320, 420 - 144), 1.5f, 3.0f);
    playdate->sprite->setZIndex(sprite, zIndex);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateIntro: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    MusicManagerPlay("musics/opening", 0, 0.2f);
}

static void loadStateHeroes(OpeningScene * _Nonnull self) {
    self->duration = 4.0f;

    // Fond
    MELSprite *melSprite = new(MELSprite);
    LCDSprite *sprite = MELSpriteInitWithCenter(melSprite, &spriteTitle, MELPointZero);
    melSprite->userdata = &self->camera;
    playdate->sprite->setUpdateFunction(sprite, moveCameraRight);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    sprite = LayerSpriteConstructor(self->horizontalMap->layers + 0, loadMapLayer(MapNameIntroSeq1, 0));
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);
    for (unsigned int index = 1; index < self->horizontalMap->layerCount; index++) {
        LCDBitmap *bitmap = loadMapLayer(MapNameIntroSeq1, index);

        sprite = LayerSpriteConstructorLooping(self->horizontalMap->layers + index, bitmap, MELIntPointZero);
        LayerSpriteSetCamera(sprite, &self->camera);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
        LCDSpriteRefListPush(&self->super.sprites, sprite);

        sprite = LayerSpriteConstructorLooping(self->horizontalMap->layers + index, bitmap, (MELIntPoint) { .x = self->horizontalMap->size.width });
        LayerSpriteSetCamera(sprite, &self->camera);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    // Caroline
    loadSpriteOpeningCarolinePalette();
    sprite = StrideConstructor(&spriteOpeningCaroline, MELPointMake(-spriteOpeningCaroline.size.width / 2.0f, 120.0f), MELPointMake(320.0f, 120.0f), 0.3f, 1.0f);
    StrideSetFillBlack(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    sprite = StrideConstructor(&spriteOpeningCaroline, MELPointMake(-spriteOpeningCaroline.size.width / 2.0f, 120.0f), MELPointMake(320.0f, 120.0f), 0.2f, 1.0f);
    StrideSetFillBlack(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);
 
    sprite = StrideConstructor(&spriteOpeningCaroline, MELPointMake(-spriteOpeningCaroline.size.width / 2.0f, 120.0f), MELPointMake(320.0f, 120.0f), 0.1f, 1.0f);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // Cérès
    loadSpriteOpeningCeresPalette();
    sprite = StrideConstructor(&spriteOpeningCeres, MELPointMake(-spriteOpeningCeres.size.width / 2.0f, 90.0f), MELPointMake(207.0f, 90.0f), 1.2f, 1.0f);
    StrideSetFillBlack(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    sprite = StrideConstructor(&spriteOpeningCeres, MELPointMake(-spriteOpeningCeres.size.width / 2.0f, 90.0f), MELPointMake(207.0f, 90.0f), 1.1f, 1.0f);
    StrideSetFillBlack(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);
 
    sprite = StrideConstructor(&spriteOpeningCeres, MELPointMake(-spriteOpeningCeres.size.width / 2.0f, 90.0f), MELPointMake(207.0f, 90.0f), 1.0f, 1.0f);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // Clovis
    loadSpriteOpeningClovisPalette();
    sprite = StrideConstructor(&spriteOpeningClovis, MELPointMake(LCD_COLUMNS + spriteOpeningClovis.size.width / 2.0f, 121.0f), MELPointMake(68.0f, 121.0f), 2.2f, 1.0f);
    StrideSetFillBlack(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    sprite = StrideConstructor(&spriteOpeningClovis, MELPointMake(LCD_COLUMNS + spriteOpeningClovis.size.width / 2.0f, 121.0f), MELPointMake(68.0f, 121.0f), 2.1f, 1.0f);
    StrideSetFillBlack(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);
 
    sprite = StrideConstructor(&spriteOpeningClovis, MELPointMake(LCD_COLUMNS + spriteOpeningClovis.size.width / 2.0f, 121.0f), MELPointMake(68.0f, 121.0f), 2.0f, 1.0f);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("loadStateHeroes: push (%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, sprite);
}

static void loadStateOpponents(OpeningScene * _Nonnull self) {
    self->duration = 6.0f;

    // Fond
    MELSprite *melSprite = new(MELSprite);
    LCDSprite *sprite = MELSpriteInitWithCenter(melSprite, &spriteTitle, MELPointZero);
    melSprite->userdata = &self->camera;
    playdate->sprite->setUpdateFunction(sprite, moveCameraDown);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    sprite = LayerSpriteConstructor(self->verticalMap->layers + 0, loadMapLayer(MapNameIntroSeq2, 0));
    LCDSpriteRefListPush(&self->super.sprites, sprite);
    for (unsigned int index = 1; index < self->verticalMap->layerCount; index++) {
        LCDBitmap *bitmap = loadMapLayer(MapNameIntroSeq2, index);

        sprite = LayerSpriteConstructorLooping(self->verticalMap->layers + index, bitmap, MELIntPointZero);
        LayerSpriteSetCamera(sprite, &self->camera);
        LCDSpriteRefListPush(&self->super.sprites, sprite);

        sprite = LayerSpriteConstructorLooping(self->verticalMap->layers + index, bitmap, (MELIntPoint) { .x = self->horizontalMap->size.width });
        LayerSpriteSetCamera(sprite, &self->camera);
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    static const float sceneHeight = 640.0f;

    // Crocoford
    loadSpriteOpeningCrocofordPalette();
    sprite = StrideConstructor(&spriteOpeningCrocoford, MELPointMake(140.0f, -472.0f), MELPointMake(140.0f, -472.0f + sceneHeight), 0.0f, 6.0f);
#if ENABLE_OPENING_SHADE
    playdate->sprite->setUpdateFunction(sprite, updateAndShadeSprite);
#endif
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // Prince
    loadSpriteOpeningPrincePalette();
    sprite = StrideConstructor(&spriteOpeningPrince, MELPointMake(268.0f, -247.0f), MELPointMake(268.0f, -247.0f + sceneHeight), 0.0f, 6.0f);
#if ENABLE_OPENING_SHADE
    playdate->sprite->setUpdateFunction(sprite, updateAndShadeSprite);
#endif
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // El Joker
    loadSpriteOpeningElJokerPalette();
    sprite = StrideConstructor(&spriteOpeningElJoker, MELPointMake(90.0f, -180.0f), MELPointMake(90.0f, -180.0f + sceneHeight), 0.0f, 6.0f);
#if ENABLE_OPENING_SHADE
    playdate->sprite->setUpdateFunction(sprite, updateAndShadeSprite);
#endif
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // Orelia
    loadSpriteOpeningOreliaPalette();
    sprite = StrideConstructor(&spriteOpeningOrelia, MELPointMake(100.0f, 56.0f), MELPointMake(100.0f, 56.0f + sceneHeight), 0.0f, 6.0f);
#if ENABLE_OPENING_SHADE
    playdate->sprite->setUpdateFunction(sprite, updateAndShadeSprite);
#endif
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // Belleplume
    loadSpriteOpeningBelleplumePalette();
    sprite = StrideConstructor(&spriteOpeningBelleplume, MELPointMake(260.0f, 84.0f), MELPointMake(260.0f, 84.0f + sceneHeight), 0.0f, 6.0f);
#if ENABLE_OPENING_SHADE
    playdate->sprite->setUpdateFunction(sprite, updateAndShadeSprite);
#endif
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);
}

static void loadStateDanse(OpeningScene * _Nonnull self) {
    self->duration = 3.5f;
    LCDSprite *sprite = LayerSpriteConstructor(self->danseBackground->layers + 0, loadMapLayer(MapNameIntro, 0));
    LCDSpriteRefListPush(&self->super.sprites, sprite);
    sprite = LayerSpriteConstructor(self->danseBackground->layers + 1, loadMapLayer(MapNameIntro, 1));
    LCDSpriteRefListPush(&self->super.sprites, sprite);
    sprite = LayerSpriteConstructor(self->danseBackground->layers + 2, loadMapLayer(MapNameIntro, 2));
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    loadSpriteCarolineDansePalette();
    MELSprite *melSprite = new(MELSprite);
    sprite = MELSpriteInitWithCenter(melSprite, &spriteCarolineDanse, (MELPoint) { .x = 228, .y = 120 });
    playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    const int halfSize = 9;
    const int chipCount = 30;

    // TODO: Lancer des confettis !
    loadSpriteChip18Palette();
    for (unsigned int index = 0; index < chipCount; index++) {
        // Gauche
        MELSprite *melSprite = new(MELSprite);
        LCDSprite *sprite = MELSpriteInitWithCenter(melSprite, &spriteChip18, MELPointMake(-halfSize, LCD_ROWS + halfSize));
        MELSpriteSetAnimation(melSprite, AnimationNameWalk);
        melSprite->autoReleaseUserdata = true;
        CardJump *cardJump = new(CardJump);
        melSprite->userdata = cardJump;

        cardJump->time = 2.0f;
        cardJump->speed = (MELPoint) {
            .x = 200.0f + MELRandomFloat(100.0f),
            .y = -400.0f - MELRandomFloat(300.0f),
        };
        playdate->sprite->setUpdateFunction(sprite, updateChips);
        playdate->sprite->setZIndex(sprite, 99);
        LCDSpriteRefListPush(&self->super.sprites, sprite);

        // Droite
        melSprite = new(MELSprite);
        sprite = MELSpriteInitWithCenter(melSprite, &spriteChip18, MELPointMake(LCD_COLUMNS + halfSize, LCD_ROWS + halfSize));
        melSprite->autoReleaseUserdata = true;
        cardJump = new(CardJump);
        melSprite->userdata = cardJump;

        cardJump->time = 2.0f;
        cardJump->speed = (MELPoint) {
            .x = -200.0f - MELRandomFloat(100.0f),
            .y = -400.0f - MELRandomFloat(300.0f),
        };
        playdate->sprite->setUpdateFunction(sprite, updateChips);
        playdate->sprite->setZIndex(sprite, 99);
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    // Sapique
    loadSpriteOpeningSapiquePalette();
    sprite = StrideConstructor(&spriteOpeningSapique, MELPointMake(-65, 342.0f), MELPointMake(112.0f, 124.0f), 2.0f, 0.25f);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // Garouge
    loadSpriteOpeningGarougePalette();
    sprite = StrideConstructor(&spriteOpeningGarouge, MELPointMake(545.0f, 370.0f), MELPointMake(313.0f, 139.0f), 2.0f, 0.25f);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);
}

static void loadStateCave(OpeningScene * _Nonnull self) {
    self->duration = 2.5f;
    loadSpriteOpeningCaveHatPalette();

    LCDSprite *sprite = StrideConstructor(&spriteOpeningCaveHat,
        (MELPoint) { .x = LCD_COLUMNS / 2 - 10, .y = LCD_ROWS / 2 },
        (MELPoint) { .x = LCD_COLUMNS / 2 + 20, .y = LCD_ROWS / 2 },
        0.0f, 2.2f);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    // Noir
    loadSpriteBlackPalette();
    sprite = StrideConstructor(&spriteBlack, MELPointMake(200.0f, 60.0f), MELPointMake(200.0f, 0.0f), 0.0f, 1.0f);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    sprite = StrideConstructor(&spriteBlack, MELPointMake(200.0f, 180.0f), MELPointMake(200.0f, 240.0f), 0.0f, 1.0f);
    playdate->sprite->setZIndex(sprite, ZINDEX_DANSE);
    LCDSpriteRefListPush(&self->super.sprites, sprite);
}

static void loadStateOutro(OpeningScene * _Nonnull self) {
    LCDSprite *sprite;
    MELSprite *melSprite;

    self->duration = 1.0f;

    loadSpriteTitlePalette();
    loadSpriteTitleMagicHatPalette();
    loadSpriteTitleSparksPalette();

    melSprite = new(MELSprite);
    sprite = MELSpriteInitWithCenter(melSprite, &spriteTitleSparks, (MELPoint) { .x = 200, .y = 120 });
    playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
    LCDSpriteRefListPush(&self->super.sprites, sprite);
    MELSpriteSetAnimation(melSprite, AnimationNameJump);

    melSprite = new(MELSprite);
    sprite = MELSpriteInitWithCenter(melSprite, &spriteTitleSparks, (MELPoint) { .x = 200, .y = 120 });
    playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
    LCDSpriteRefListPush(&self->super.sprites, sprite);
    MELSpriteSetAnimation(melSprite, AnimationNameFall);

    sprite = StrideConstructor(&spriteTitleMagicHat,
          (MELPoint) { .x = 200, .y = 100 },
          (MELPoint) { .x = 200, .y = 200 },
          0.0f, 1.0f);
    StrideSetEasingFunction(sprite, MELEaseOutBounce);
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    sprite = StrideConstructor(&spriteTitle,
          (MELPoint) { .x = 200, .y = -spriteTitle.size.height / 2 },
          (MELPoint) { .x = 200, .y = 94 },
          0.0f, 1.0f);
    LCDSpriteRefListPush(&self->super.sprites, sprite);
}

static void loadNextState(OpeningScene * _Nonnull self) {
    LCDSprite *sprite;
    MELSprite *melSprite;

    FadeSetOpacity(self->fade, 1.0f);

    LCDSpriteRefList sprites = self->super.sprites;
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("%d sprites to destroy", sprites.count);
    for (int index = 0; index < sprites.count; index++) {
        playdate->system->logToConsole("- %d: %x", index, sprites.memory[index]);
    }
#endif
    for (int index = sprites.count - 1; index >= 0; index--) {
        sprite = sprites.memory[index];
        if (sprite != self->fade) {
            melSprite = playdate->sprite->getUserdata(sprite);
            melSprite->class->destroy(sprite);
        }
    }

    self->duration = 2.0f;

    switch (self->state) {
        case OpeningSceneStateIntro:
            loadStateIntro(self);
            break;
        case OpeningSceneStateHeroes:
            loadStateHeroes(self);
            break;
        case OpeningSceneStateOpponents:
            loadStateOpponents(self);
            break;
        case OpeningSceneStateDanse:
            loadStateDanse(self);
            break;
        case OpeningSceneStateCave:
            loadStateCave(self);
            break;
        default:
            playdate->system->logToConsole("Unsupported state: %d", self->state);
            break;
    }

    self->time = 0.0f;
    self->camera.frame.origin = MELPointZero;
    playdate->system->setUpdateCallback(updateFadeOut, self);
}

static void fadeTo(OpeningScene * _Nonnull self, OpeningSceneState state) {
    self->state = state;
    self->time = 0.0f;
    if (self->fade == NULL) {
        self->fade = FadeConstructor();
    } else {
        FadeSetOpacity(self->fade, 0.0f);
    }
    if (state == OpeningSceneStateOutro) {
        CardDeck deck = (CardDeck) {};
        float delay = 0.0f;
        loadSpriteFileSelectCardPalette();
        for (int y = LCD_ROWS - spriteFileSelectCard.size.height / 2; y > 0; y -= spriteFileSelectCard.size.height) {
            for (unsigned int x = spriteFileSelectCard.size.width / 2; x < LCD_COLUMNS; x += spriteFileSelectCard.size.width) {
                const MELPoint origin = (MELPoint) {
                    .x = x,
                    .y = -spriteFileSelectCard.size.height / 2,
                };
                const MELPoint target = (MELPoint) {
                    .x = x,
                    .y = y,
                };
                Card card = CardDeckPickRandomCard(&deck);
                LCDSprite *sprite = StrideConstructor(&spriteFileSelectCard, origin, target, delay + MELRandomFloat(0.2f), 2.0f);
                playdate->sprite->setZIndex(sprite, ZINDEX_FADE);
                StrideSetEasingFunction(sprite, MELEaseOutBounce);
                MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
                MELSpriteSetAnimation(melSprite, AnimationNameWalk + card.suit);
                MELSingleFrameAnimationReuse(melSprite->animation);
                MELAnimationSetFrameIndex(melSprite->animation, card.rank);
                LCDSpriteRefListPush(&self->super.sprites, sprite);
            }
            delay += 0.2f;
        }
        CardDeckDeinit(&deck);
        playdate->system->setUpdateCallback(updateFadeInCards, self);
    } else {
        playdate->system->setUpdateCallback(updateFadeIn, self);
    }
}

static void moveCameraRight(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELCamera *camera = self->userdata;
    camera->frame.origin.x += 600.0f * DELTA;
}

static void moveCameraDown(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELCamera *camera = self->userdata;
    camera->frame.origin.y += 600.0f * DELTA;
}

static void moveCameraRightAndDown(LCDSprite * _Nonnull sprite) {
    StrideUpdate(sprite);
    playdate->sprite->setUpdateFunction(sprite, moveCameraRightAndDown);

    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELCamera *camera = &((OpeningScene *)currentScene)->camera;
    camera->frame.origin.x += 16.0f / kIsometricRunDuration * DELTA;
    camera->frame.origin.y = self->frame.origin.y;
}

static MELBoolean playerDidSkip(OpeningScene * _Nonnull self) {
    MELController controller = MELControllerMake();
    if ((self->state > 0 || self->time > 0.5f) && (controller.pressedA || controller.pressedB)) {
        TitleScene *titleScene = TitleSceneAlloc();
        MELSceneMakeCurrent(&titleScene->super);
        return true;
    }
    return false;
}

#if ENABLE_OPENING_SHADE
static void updateAndShadeSprite(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    const float oldShade = MELProgress(0, LCD_ROWS / 3, self->frame.origin.y);
    StrideUpdate(sprite);
    const float shade = MELProgress(0, LCD_ROWS / 3, self->frame.origin.y);

    if (shade == 0) {
        playdate->sprite->setDrawMode(sprite, kDrawModeFillBlack);
        return;
    }
    playdate->sprite->setDrawMode(sprite, kDrawModeCopy);

    LCDBitmap *source = playdate->graphics->getTableBitmap(self->definition.palette, self->animation->frame.atlasIndex);
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    const MELBoolean notShadedYet = image == NULL || image == source;

    const uint8_t oldValue = (oldShade * 160.0f) / 8;
    const uint8_t value = (shade * 160.0f) / 8;

    if (value != oldValue || notShadedYet) {
        MELSize size = self->definition.size;
        if (image == source) {
            image = playdate->graphics->newBitmap(size.width, size.height, kColorClear);
        }
        playdate->graphics->pushContext(image);
        playdate->graphics->drawBitmap(source, 0, 0, kBitmapUnflipped);
        playdate->graphics->popContext();

        LCDBitmapShadeImage(image, shade);
        playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    }
}
#endif

static void updateChips(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    CardJump *cardJump = self->userdata;

    if (cardJump->time > 0.0f) {
        cardJump->time -= DELTA;
        return;
    }

    MELPoint speed = cardJump->speed;
    speed.y = fminf(speed.y + 700.0f * DELTA, 600.0f);
    cardJump->speed = speed;

    MELRectangle frame = self->frame;
    frame.origin.x += DELTA * speed.x;
    frame.origin.y += DELTA * speed.y;
    self->frame = frame;

    if (!MELRectangleIntersectsWithRectangle(frame, MELScreen)) {
        self->class->destroy(sprite);
        return;
    }

    MELAnimationUpdate(self->animation, DELTA);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, self->animation->frame.atlasIndex), kBitmapUnflipped);
}

static int updateFadeIn(void * _Nonnull userdata) {
    OpeningScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    playdate->sprite->updateAndDrawSprites();

    if (playerDidSkip(self)) {
        return true;
    }

    float time = self->time;
    if (time < kFadeDuration) {
        time += DELTA;
        self->time = time;
        const float progress = MELEaseIn(0.0f, kFadeDuration, time);
        FadeSetOpacity(self->fade, progress);
        return true;
    }

    loadNextState(self);
    return true;
}

static int updateFadeOut(void * _Nonnull userdata) {
    OpeningScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    playdate->sprite->updateAndDrawSprites();

    if (playerDidSkip(self)) {
        return true;
    }

    float time = self->time;
    if (time < kFadeDuration) {
        time += DELTA;
        self->time = time;
        const float progress = MELEaseOut(0.0f, kFadeDuration, time);
        FadeSetOpacity(self->fade, 1.0f - progress);
        return true;
    }

    self->time = 0.0f;
    playdate->system->setUpdateCallback(update, self);
    return true;
}

static void updateJump(LCDSprite * _Nonnull sprite) {
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    CardJump *cardJump = melSprite->userdata;

    melSprite->frame.origin = MELPointAdd(melSprite->frame.origin, MELPointMultiplyByValue(cardJump->speed, DELTA));
    cardJump->speed.y += DELTA * 600.0f;

    MELSpriteUpdate(sprite);

    if (!MELRectangleIntersectsWithRectangle(melSprite->frame, MELScreen)) {
        melSprite->class->destroy(sprite);
    }
}

static int updateFadeInCards(void * _Nonnull userdata) {
    OpeningScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    playdate->sprite->updateAndDrawSprites();

    if (playerDidSkip(self)) {
        return true;
    }

    float time = self->time;
    if (time < 3.0f) {
        time += DELTA;
        self->time = time;
        return true;
    }

    LCDSpriteRefList sprites = self->super.sprites;
    for (int index = sprites.count - 1; index >= 0; index--) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.name == SpriteNameFileSelectCard) {
            melSprite->autoReleaseUserdata = true;
            CardJump *cardJump = new(CardJump);
            melSprite->userdata = cardJump;

            const float distanceToCenter = (melSprite->frame.origin.x - LCD_COLUMNS / 2) / (LCD_COLUMNS / 2);
            cardJump->speed = (MELPoint) {
                .x = MELFloatSign(distanceToCenter) * (distanceToCenter * distanceToCenter) * 1200.0f,
                .y = (1.0f - fabsf(distanceToCenter)) * (1.0f - fabsf(distanceToCenter)) * -1200.0f,
            };
            playdate->sprite->setUpdateFunction(sprite, updateJump);
        } else {
            melSprite->class->destroy(sprite);
        }
    }
    loadStateOutro(self);

    self->time = 0.0f;
    playdate->system->setUpdateCallback(updateFadeOutCards, self);
    return true;
}

static int updateFadeOutCards(void * _Nonnull userdata) {
    OpeningScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    playdate->sprite->updateAndDrawSprites();

    if (playerDidSkip(self)) {
        return true;
    }

    float time = self->time;
    if (time < kFadeDuration) {
        time += DELTA;
        self->time = time;
        return true;
    }

    self->time = 0.0f;
    playdate->system->setUpdateCallback(update, self);
    return true;
}

static int update(void * _Nonnull userdata) {
    OpeningScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    playdate->sprite->updateAndDrawSprites();

    if (playerDidSkip(self)) {
        return true;
    }

    float time = self->time;
    if (time < self->duration) {
        self->time += DELTA;
        return true;
    }

    const OpeningSceneState nextState = self->state + 1;
    if (nextState < OpeningSceneStateTitle) {
        fadeTo(self, nextState);
    } else {
        TitleScene *titleScene = TitleSceneAlloc();
        MELSceneMakeCurrent(&titleScene->super);
    }
    return true;
}
