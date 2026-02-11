//
//  attractmodescene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/08/2024.
//

#include "attractmodescene.h"

#include "samples.h"
#include "music.h"
#include "carddeck.h"
#include "layersprite.h"
#include "cpu.h"
#include "instantcpu.h"
#include "story_caroline.h"
#include "mapscene.h"
#include "titlescene.h"
#include "savegame.h"
#include "opponent_portait.h"

#include "../gen/maps.h"
#include "../gen/spritetitlepressa.h"

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

static void anySideLost(void * _Nullable userdata, int _);

static void updatePressA(LCDSprite * _Nonnull sprite);

static const float kAttractModeDuration = 60.0f;
static const int kPlayerCharacterCount = 3;
static InstantCpu kPlayersCpu[] = {
    // Cérès
    (InstantCpu) {
        UseInstantCpu(0.8f),
        .isFastWhenAboveTarget = true,
        .sameSuitScore = 1,
        .miss = 20,
    },
    // Caroline
    (InstantCpu) {
        UseInstantCpu(0.6f),
        .isFastWhenAboveTarget = true,
        .straightScore = 1,
        .miss = 20,
    },
    // Clovis
    (InstantCpu) {
        UseInstantCpu(0.5f),
        .isFastWhenAboveTarget = true,
        .sameKindScore = 1,
        .miss = 20,
    },
};

#pragma mark - Fonctions publiques

AttractModeScene * _Nonnull AttractModeSceneAlloc(void) {
    AttractModeScene *self = new(AttractModeScene);

    while (!SampleLoadNextSample());

    CardList cards = CardListMakeWithInitialCapacity(DECK_CARD_COUNT);
    CardDeck deck = (CardDeck) {};
    for (unsigned int index = 0; index < DECK_CARD_COUNT; index++) {
        cards.memory[index] = CardDeckPickRandomCard(&deck);
    }
    cards.count = DECK_CARD_COUNT;
    CardDeckDeinit(&deck);

    *self = (AttractModeScene) {
        .super = {
            .type = SceneTypeAttractMode,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .sprites = LCDSpriteRefListEmpty,
        },
        .background = MELMapOpen(kMapNameFileNames[MapNamePlateauPd]),
        .backgroundName = MapNamePlateauPd,
        .cards = cards,
        .speed = 1.0f,
    };
    return self;
}

#pragma mark - Fonctions privées

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeAttractMode) {
        playdate->system->error("Given scene is not a AttractModeScene instance");
        return;
    }
    AttractModeScene *self = (AttractModeScene *)scene;

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->background);

    GameFlowDeinit(&self->leftPlayerGameFlow);
    GameFlowDeinit(&self->rightPlayerGameFlow);

    CardListDeinit(&self->cards);

    playdate->system->realloc(self, 0);
}

static void init(MELScene * _Nonnull scene) {
    AttractModeScene *self = (AttractModeScene *)scene;

    const MapName mapName = self->backgroundName;
    MELMap *background = self->background;
    for (unsigned int index = 0; index < background->layerCount; index++) {
        MELLayer *layer = background->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);
        LCDSpriteRefListPush(&self->super.sprites, LayerSpriteConstructor(layer, image));
    }

    MELSprite *pressA = new(MELSprite);
    LCDSprite *pressASprite = MELSpriteInitWithCenter(pressA, &spriteTitlePressA, MELPointMake(LCD_COLUMNS / 2, LCD_ROWS / 2));
    playdate->sprite->setZIndex(pressASprite, 99);
    playdate->sprite->setUpdateFunction(pressASprite, updatePressA);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push pressASprite(%x, %x): %d", pressASprite, pressA, pressA->definition.name);
#endif
    LCDSpriteRefListPush(&self->super.sprites, pressASprite);

    const int playerIndex = MELRandomInt(kPlayerCharacterCount);
    const Story *story = kStories[playerIndex];
    const Character player = story->character;

    const int encounterIndex = MELRandomInt(kPlayerCharacterCount);
    Encounter encounter = story->encounters.memory[encounterIndex];

    if (encounter.gameSpeed > 0.0f) {
        self->speed = 1.0f / encounter.gameSpeed;
    }

    Board *rightBoard = BoardAlloc(SideRight);

    MELSpriteInstanceList instances = background->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette == NULL) {
            definition->palette = SpriteNameLoadBitmapTable(instance.name);
        }
        MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
        LCDSprite *sprite = MELSpriteInit(melSprite, definition, instances.memory + index);
        LCDSpriteRefListPush(&self->super.sprites, sprite);

        if (instance.name == SpriteNamePortaits) {
            const MELBoolean isPlayer = instance.direction == MELDirectionRight;
            const Character character = isPlayer ? player : encounter.opponent;
            MELSpriteSetAnimationAndDirection(melSprite, CharacterAnimationName[character], CharacterAnimationDirection[character]);
            MELSpriteUpdate(sprite);
            if (!isPlayer) {
                OpponentPortaitConfigure(melSprite, sprite, rightBoard);
            } else {
                playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
            }
            playdate->sprite->setZIndex(sprite, ZINDEX_PORTAITS);
        }
    }

    GameFlowInit(&self->leftPlayerGameFlow, SideLeft, BoardAlloc(SideLeft), self->cards);
    GameFlowInit(&self->rightPlayerGameFlow, SideRight, rightBoard, self->cards);
    self->leftPlayerGameFlow.actor = InstantCpuAlloc(&kPlayersCpu[playerIndex].super.super);

    Actor *cpuActor = encounter.actor;
    self->rightPlayerGameFlow.actor = cpuActor->alloc(cpuActor);

    MELEventBusAddListener(EventLeftSideLost, anySideLost, self);
    MELEventBusAddListener(EventRightSideLost, anySideLost, self);

    MusicManagerFadeOut(0.5f);

    camera.frame.origin = MELPointZero;
    setRefreshRate(DEFAULT_REFRESH_RATE);
}

static void anySideLost(void * _Nullable userdata, int _) {
    AttractModeScene *self = userdata;
    self->time = kAttractModeDuration;
}

#pragma mark - Boucle de jeu

static int update(void * _Nonnull userdata) {
    AttractModeScene *self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    self->time += DELTA;

    // Gère les boutons et l'IA.
    ActorUpdate(self->leftPlayerGameFlow.actor);
    ActorUpdate(self->rightPlayerGameFlow.actor);

    // Déplace et affiche les sprites.
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->leftPlayerGameFlow.update(&self->leftPlayerGameFlow);
    self->rightPlayerGameFlow.update(&self->rightPlayerGameFlow);

    MELController controller = MELControllerMake();
    if (self->time >= kAttractModeDuration || controller.pressedA || controller.pressedB) {
        TitleScene *titleScene = TitleSceneAlloc();
        MELSceneMakeCurrent(&titleScene->super);
    }
    return true;
}

static void updatePressA(LCDSprite * _Nonnull sprite) {
    const unsigned int time = playdate->system->getCurrentTimeMilliseconds();
    playdate->sprite->setVisible(sprite, time % 1500 < 750);
    MELSpriteUpdate(sprite);
}
