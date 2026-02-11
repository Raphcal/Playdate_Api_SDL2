//
//  gamescene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 24/01/2023.
//

#include "gamescene.h"

#include "samples.h"
#include "music.h"
#include "carddeck.h"
#include "layersprite.h"
#include "cpu.h"
#include "mapscene.h"
#include "dialogscene.h"
#include "savegame.h"
#include "opponent_portait.h"

#include "../gen/maps.h"

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);
static int updateWon(void * _Nullable userdata);
static int updateLost(void * _Nullable userdata);

static void playerWon(void * _Nullable userdata, int _);
static void playerLost(void * _Nullable userdata, int _);
static void playerMayLose(void * _Nullable userdata, int mayLose);
static void playerSentChips(void * _Nullable userdata, int chips);
static void playerSentStar(void * _Nullable userdata, int stars);
static void playerClearedBoard(void * _Nullable userdata, int boardCount);
static void playerMadeCombo(void * _Nullable userdata, int comboCount);
static void playerMadePokerHand(void * _Nullable userdata, const void * _Nullable hand);

static MELBoolean isBoss(GameScene * _Nonnull self);

#pragma mark - Fonctions publiques

GameScene * _Nonnull GameSceneAlloc(const Story * _Nullable story, uint32_t encounterIndex) {
    GameScene *self = playdate->system->realloc(NULL, sizeof(GameScene));

    while (!SampleLoadNextSample());

    *self = (GameScene) {
        .super = {
            .type = SceneTypeGame,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .sprites = LCDSpriteRefListEmpty,
        },
        .background = MELMapOpen(kMapNameFileNames[MapNamePlateauPd]),
        .backgroundName = MapNamePlateauPd,
        .cards = CardListMakeWithRandomCards(DECK_CARD_COUNT),
        .story = story,
        .encounterIndex = encounterIndex,
        .speed = 1.0f,
    };
    return self;
}

#pragma mark - Fonctions privées

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeGame) {
        playdate->system->error("Given scene is not a GameScene instance");
        return;
    }
    GameScene *self = (GameScene *)scene;

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->background);

    GameFlowDeinit(&self->leftPlayerGameFlow);
    GameFlowDeinit(&self->rightPlayerGameFlow);

    CardListDeinit(&self->cards);

    MELAchievementToastAdjustLeft = 0;
    MELAchievementToastAdjustWidth = 0;

    playdate->system->realloc(self, 0);
}

static void init(MELScene * _Nonnull scene) {
    GameScene *self = (GameScene *)scene;

    MELAchievementToastAdjustLeft = 64;
    MELAchievementToastAdjustWidth = 64;

    const MapName mapName = self->backgroundName;
    MELMap *background = self->background;
    for (unsigned int index = 0; index < background->layerCount; index++) {
        MELLayer *layer = background->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);
        LCDSpriteRefListPush(&self->super.sprites, LayerSpriteConstructor(layer, image));
    }

    const Character player = self->story->character;
    Encounter encounter = self->story->encounters.memory[self->encounterIndex];

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
    self->leftPlayerGameFlow.actor = ActorMakePlayer();

    Actor *cpuActor = encounter.actor;
    self->rightPlayerGameFlow.actor = cpuActor->alloc(cpuActor);

    MELEventBusAddListener(EventLeftSideLost, playerLost, self);
    MELEventBusAddListener(EventRightSideLost, playerWon, self);
    MELEventBusAddListener(EventLeftSideMayLose, playerMayLose, self);
    MELEventBusAddListener(EventLeftSideSentChips, playerSentChips, self);
    MELEventBusAddListener(EventLeftSideSentStar, playerSentStar, self);
    MELEventBusAddListener(EventLeftSideClearedBoard, playerClearedBoard, self);
    MELEventBusAddListener(EventLeftSideMadeCombo, playerMadeCombo, self);
    MELEventBusAddListenerVoidPointer(EventLeftSideMadePokerHand, playerMadePokerHand, self);

    MusicManagerPlay(!isBoss(self) ? "musics/performing" : "musics/boss", 0, 0.25f);

    camera.frame.origin = MELPointZero;
    setRefreshRate(DEFAULT_REFRESH_RATE);
}

static void playerWon(void * _Nullable userdata, int _) {
    GameScene *self = userdata;
    const StoryName storyName = StoryNameByCharacter(self->story->character);
    const int nextEncounterIndex = self->encounterIndex + 1;
    if (nextEncounterIndex > currentSaveGame.progression[storyName]) {
        currentSaveGame.progression[storyName] = nextEncounterIndex;
    }
    currentSaveGame.wins++;
    SaveGameToDisk(&currentSaveGame);

    MusicManagerPlay("musics/masterful", 1, 0.1f);

    MELAchievementToastAdjustLeft = 0;
    MELAchievementToastAdjustWidth = 0;
    if (self->pinch) {
        unlockAchievement(AchievementReversal);
    }
    unlockAchievement(AchievementGameVictory1);
    if (currentSaveGame.wins >= 20) {
        unlockAchievement(AchievementGameVictory20);
    }

    playdate->system->setUpdateCallback(updateWon, self);
}

static void playerLost(void * _Nullable userdata, int _) {
    GameScene *self = userdata;
    currentSaveGame.loses++;
    SaveGameToDisk(&currentSaveGame);

    MusicManagerPlay("musics/outoftricks", 1, 0.1f);

    MELAchievementToastAdjustLeft = 0;
    MELAchievementToastAdjustWidth = 0;
    unlockAchievement(AchievementGameDefeat1);

    playdate->system->setUpdateCallback(updateLost, self);
}

static void playerMayLose(void * _Nullable userdata, int mayLose) {
    GameScene *self = userdata;
    self->pinch = self->pinch || mayLose;
    MusicManagerPlay(mayLose
         ? "musics/pinch"
         : (!isBoss(self) ? "musics/performing" : "musics/boss"), 0, 0.25f);
}

static void playerSentChips(void * _Nullable userdata, int chips) {
    currentSaveGame.sentChips += chips;
}

static void playerSentStar(void * _Nullable userdata, int stars) {
    unlockAchievement(AchievementChainStar);
}

static void playerClearedBoard(void * _Nullable userdata, int boardCount) {
    unlockAchievement(AchievementAllClear);
}

static void playerMadeCombo(void * _Nullable userdata, int comboCount) {
    if (comboCount == 2) {
        unlockAchievement(AchievementChainX2);
    }
    if (comboCount == 4) {
        unlockAchievement(AchievementChainX2);
    }
}

static void playerMadePokerHand(void * _Nullable userdata, const void * _Nullable pokerHand) {
    const Hand *hand = pokerHand;
    if (hand->type == HandTypeSameKind && hand->rank == RankSeven) {
        unlockAchievement(AchievementJackpot);
    }
    if (hand->type == HandTypeSameKind && hand->count > 4) {
        unlockAchievement(AchievementFiveOfAKind);
    }
    if (hand->type == HandTypeStraight && hand->rank == RankAce) {
        unlockAchievement(AchievementRoyalStraight);
    }
}

#pragma mark - Boucle de jeu

static int update(void * _Nonnull userdata) {
    GameScene *self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;

    // Gère les boutons et l'IA.
    ActorUpdate(self->leftPlayerGameFlow.actor);
    ActorUpdate(self->rightPlayerGameFlow.actor);

    // Déplace et affiche les sprites.
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->leftPlayerGameFlow.update(&self->leftPlayerGameFlow);
    self->rightPlayerGameFlow.update(&self->rightPlayerGameFlow);

    return true;
}

static int updateWon(void * _Nullable userdata) {
    GameScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->leftPlayerGameFlow.update(&self->leftPlayerGameFlow);
    self->rightPlayerGameFlow.update(&self->rightPlayerGameFlow);

    if (self->time < 1.0f) {
        self->time += DELTA;
        return true;
    }

    // TODO: Faire une animation et jouer un son
    if (MELControllerMake().pressedA) {
        Encounter encounter = self->story->encounters.memory[self->encounterIndex + 1];
        MELScene *nextScene;
        if (encounter.pin.x && encounter.pin.y) {
            MapScene *mapScene = MapSceneAlloc(self->story, self->encounterIndex + 1);
            nextScene = &mapScene->super;
        } else {
            DialogScene *dialogScene = DialogSceneAlloc(self->story, self->encounterIndex + 1);
            nextScene = &dialogScene->super;
        }
        MELSceneMakeCurrent(nextScene);
    }
    return true;
}

static int updateLost(void * _Nullable userdata) {
    GameScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->leftPlayerGameFlow.update(&self->leftPlayerGameFlow);
    self->rightPlayerGameFlow.update(&self->rightPlayerGameFlow);

    if (self->time < 1.0f) {
        self->time += DELTA;
        return true;
    }

    // TODO: Faire une animation et jouer un son
    if (MELControllerMake().pressedA) {
        GameScene *gameScene = GameSceneAlloc(self->story, self->encounterIndex);
        MELSceneMakeCurrent(&gameScene->super);
    }
    return true;
}

static MELBoolean isBoss(GameScene * _Nonnull self) {
    return self->story
        && self->story->encounters.memory[self->encounterIndex + 1].opponent == CharacterNone;
}
