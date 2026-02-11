//
//  scoreattackscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 07/11/2024.
//

#include "scoreattackscene.h"

#include "scoreattackselectscene.h"
#include "samples.h"
#include "music.h"
#include "carddeck.h"
#include "layersprite.h"
#include "image.h"
#include "text.h"
#include "counter.h"
#include "mapscene.h"
#include "savegame.h"
#include "caroline_score_attack.h"
#include "scores.h"
#include "scenes.h"
#include "progressbar.h"
#include "sparks.h"

#include "../gen/maps.h"

#define kBoardMapName MapNameScoreAttack

#define ADD_LABEL_AND_COUNTER(name, valuePointer, precision, animation, topY) \
LCDSprite *name##Label = ImageConstructorWithSpritePaletteAndAnimation(MELPointMake(60.0f, topY), SpriteNameScoreAttackLabels, animation, (MELAnimationDirection)currentLanguage);\
LCDSprite *name##Counter = CounterConstructorWithUInt##precision##Pointer(MELPointMake(110.0f, topY + 18), MELHorizontalAlignmentRight, fontRoobert11Bold, valuePointer);\
playdate->sprite->setZIndex(name##Label, ZINDEX_INGAME_UI);\
playdate->sprite->setZIndex(name##Counter, ZINDEX_INGAME_UI)

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);
static int updateLost(void * _Nullable userdata);

static void playerLost(void * _Nullable userdata, int _);
static void playerMayLose(void * _Nullable userdata, int mayLose);
static void playerSentChips(void * _Nullable userdata, int chips);
static void playerRemovedCards(void * _Nullable userdata, int cards);
static void playerMadeCombo(void * _Nullable userdata, int comboCount);
static void playerMadePokerHand(void * _Nullable userdata, const void * _Nullable hand);

static void setNextTarget(ScoreAttackScene * _Nonnull self);

static const uint32_t ScoreAttackDifficultyBaseScore[ScoreAttackDifficultyCount] = {
    0,
    20000,
    50000
};
static const uint8_t ScoreAttackDifficultyBaseLevel[ScoreAttackDifficultyCount] = {
    1,
    20,
    40
};

ScoreAttackScene * _Nonnull ScoreAttackSceneAlloc(ScoreAttackDifficulty difficulty) {
    ScoreAttackScene *self = new(ScoreAttackScene);
    while (!SampleLoadNextSample());

    const int cardCount = DECK_CARD_COUNT + DECK_CARD_COUNT;
    CardList cards = CardListMakeWithInitialCapacity(cardCount);
    CardDeck deck = (CardDeck) {};
    for (unsigned int index = 0; index < cardCount; index++) {
        cards.memory[index] = CardDeckPickRandomCard(&deck);
    }
    cards.count = cardCount;
    CardDeckDeinit(&deck);

    const int8_t nextTargetIndex = scoreAttackScores.count - (scoreAttackScores.count > 0 && currentSaveGame.scoreAttackPersonalBest >= scoreAttackScores.memory[scoreAttackScores.count - 1].super.value);

    *self = (ScoreAttackScene) {
        .super = {
            .type = SceneTypeScoreAttack,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .sprites = LCDSpriteRefListEmpty,
        },
        .boardMap = MELMapOpen(kMapNameFileNames[kBoardMapName]),
        .background = MELMapOpen(kMapNameFileNames[MapNameScoreAttackForest]),
        .backgroundName = MapNameScoreAttackForest,
        .cards = cards,
        .level = ScoreAttackDifficultyBaseLevel[difficulty],
        .score = ScoreAttackDifficultyBaseScore[difficulty],
        .nextTargetIndex = nextTargetIndex,
    };
    setNextTarget(self);
    while (self->nextTargetIndex >= 0 && self->score >= self->nextTarget) {
        self->nextTargetIndex--;
        setNextTarget(self);
    }
    return self;
}

#pragma mark - Fonctions privées

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeScoreAttack) {
        playdate->system->error("Given scene is not a ScoreAttackScene instance");
        return;
    }
    ScoreAttackScene *self = (ScoreAttackScene *)scene;

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->boardMap);
    MELMapDealloc(self->background);

    GameFlowDeinit(&self->gameFlow);
    CardListDeinit(&self->cards);

    MELAchievementToastAdjustLeft = 0;
    MELAchievementToastAdjustWidth = 0;

    playdate->system->realloc(self, 0);
}

static void init(MELScene * _Nonnull scene) {
    ScoreAttackScene *self = (ScoreAttackScene *)scene;

    MELAchievementToastAdjustLeft = 64;
    MELAchievementToastAdjustWidth = 64;

#if LOG_SCORE_ATTACK_LEVEL_UP
        playdate->system->logToConsole("Starting at level: %d, speed: %f", self->level, (double)GetGameSpeed());
#endif

    // Décors en fond
    // TODO: Faire changer le décors tous les 5 niveaux.
    const MapName mapName = self->backgroundName;
    MELMap *background = self->background;
    for (unsigned int index = 0; index < background->layerCount; index++) {
        MELLayer *layer = background->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);

        LCDSprite *sprite = LayerSpriteConstructorLooping(layer, image, MELIntPointZero);
        LayerSpriteSetCamera(sprite, &self->backgroundCamera);
        playdate->sprite->setZIndex(sprite, 0);
        LCDSpriteRefListPush(&self->super.sprites, sprite);

        sprite = LayerSpriteConstructorLooping(layer, image, MELIntPointMake(background->size.width, 0));
        LayerSpriteSetCamera(sprite, &self->backgroundCamera);
        playdate->sprite->setZIndex(sprite, 0);
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    // Plateau de jeu
    MELMap *boardMap = self->boardMap;
    for (unsigned int index = 0; index < boardMap->layerCount; index++) {
        MELLayer *layer = boardMap->layers + index;
        LCDBitmap *image = loadMapLayer(kBoardMapName, index);
        LCDSprite *sprite = LayerSpriteConstructor(layer, image);
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    MELSpriteInstanceList instances = boardMap->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette == NULL) {
            definition->palette = SpriteNameLoadBitmapTable(instance.name);
        }
        MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
        LCDSprite *sprite = MELSpriteInit(melSprite, definition, instances.memory + index);
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    GameFlowInit(&self->gameFlow, SideCenter, BoardAlloc(SideCenter), self->cards);
    self->gameFlow.actor = ActorMakePlayer();

    // Interface
    ADD_LABEL_AND_COUNTER(score, &self->score, 32, AnimationNameStand, 20);
    ADD_LABEL_AND_COUNTER(nextTarget, &self->nextTarget, 32, AnimationNameRun, 70);

    LCDSprite *levelLabel = ImageConstructorWithSpritePaletteAndAnimation(MELPointMake(60.0f, 130), SpriteNameScoreAttackLabels, AnimationNameWalk, (MELAnimationDirection)currentLanguage);
    LCDSprite *levelCounter = CounterConstructorWithUInt8Pointer(MELPointMake(110.0f, 130), MELHorizontalAlignmentRight, fontRoobert11Bold, &self->level);
    LCDSprite *progressBar = ProgressBarConstructor(MELRectangleMake(10 + 100 / 2, 140, 100, 4), 0, SCORE_ATTACK_CARD_COUNT_TO_LEVEL_UP, &self->removedCards);
    playdate->sprite->setZIndex(levelLabel, ZINDEX_INGAME_UI);
    playdate->sprite->setZIndex(levelCounter, ZINDEX_INGAME_UI);
    playdate->sprite->setZIndex(progressBar, ZINDEX_INGAME_UI);

    self->levelCounter = levelCounter;
    self->nextTargetLabel = nextTargetLabel;
    self->nextTargetCounter = nextTargetCounter;

    // Écoute des événements
    MELEventBusAddListener(EventCenterSideLost, playerLost, self);
    MELEventBusAddListener(EventCenterSideMayLose, playerMayLose, self);
    MELEventBusAddListener(EventCenterSideSentChips, playerSentChips, self);
    MELEventBusAddListener(EventLeftSideRemovedCards, playerRemovedCards, self);
    MELEventBusAddListener(EventLeftSideMadeCombo, playerMadeCombo, self);
    MELEventBusAddListenerVoidPointer(EventLeftSideMadePokerHand, playerMadePokerHand, self);

    // Ajout de Caroline après l'écoute des événements pour bien compter les cheers combo
    self->caroline = CarolineScoreAttackConstructorDefault();

    MusicManagerPlay("musics/performing", 0, 0.25f);

    camera.frame.origin = MELPointZero;
    setRefreshRate(DEFAULT_REFRESH_RATE);
}

static void playerLost(void * _Nullable userdata, int _) {
    ScoreAttackScene *self = userdata;
    const uint32_t score = self->score;

    if (currentSaveGame.scoreAttackRun < kScoreAttackHistoryCount) {
        currentSaveGame.scoreAttackLastScores[currentSaveGame.scoreAttackRun] = score;
    } else {
        const int lastIndex = kScoreAttackHistoryCount - 1;
        memmove(currentSaveGame.scoreAttackLastScores, currentSaveGame.scoreAttackLastScores + 1, lastIndex * sizeof(uint32_t));
        currentSaveGame.scoreAttackLastScores[lastIndex] = score;
    }
    currentSaveGame.scoreAttackRun++;

    // Sauvegarde du score.
    ScoresAdd(ScoreBoardScoreAttack, score);
    SaveGameToDisk(&currentSaveGame);

    MusicManagerPlay("musics/outoftricks", 1, 0.1f);

    MELAchievementToastAdjustLeft = 0;
    MELAchievementToastAdjustWidth = 0;

    unlockAchievement(AchievementScoreAttackFirst);
    if (score >= 100000) {
        unlockAchievement(AchievementScoreAttack100000);
    }
    if (score >= 500000) {
        unlockAchievement(AchievementScoreAttack500000);
    }

    playdate->system->setUpdateCallback(updateLost, self);
}

static void playerMayLose(void * _Nullable userdata, int mayLose) {
    MusicManagerPlay(mayLose
         ? "musics/pinch"
         : "musics/performing", 0, 0.25f);
}

static void playerSentChips(void * _Nullable userdata, int chips) {
    ScoreAttackScene *self = userdata;
    const int cheerCombo = self->cheerCombo;
    const float levelBonus = (1.0f + self->level / 10.0f);
    const float cheeringBonus = 1.0f + cheerCombo * 0.25f;
    self->score += chips * SCORE_ATTACK_CHIP_SCORE * levelBonus * cheeringBonus;
    if (cheerCombo) {
        SamplePlayWithRate(SampleCheerBonus, 1.0f + (cheerCombo - 1) * 0.125f);
    }
}

static void playerMadePokerHand(void * _Nullable userdata, const void * _Nullable pokerHand) {
    ScoreAttackScene *self = userdata;
    self->timeBeforeJump = CARD_DISAPPEAR_DURATION;

    const MELBoolean isCheering = CarolineScoreAttackIsCheering(self->caroline);
    if (isCheering) {
        self->cheerCombo = MELIntMin(self->cheerCombo + 1, SCORE_ATTACK_CHEER_COMBO_MAX);
#if LOG_COMBO
        playdate->system->logToConsole("%dx cheer combo", self->cheerCombo);
#endif
    } else if (self->gameFlow.chainCount == 0) {
#if LOG_COMBO
        playdate->system->logToConsole("cheer combo break");
#endif
        self->cheerCombo = 0;
    }

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

static void playerRemovedCards(void * _Nullable userdata, int cards) {
    ScoreAttackScene *self = userdata;
    if (self->level == SCORE_ATTACK_LEVEL_MAX) {
        return;
    }
    self->removedCards += cards;
    const MELBoolean didLevelUp = self->removedCards >= SCORE_ATTACK_CARD_COUNT_TO_LEVEL_UP;
    while (self->level < SCORE_ATTACK_LEVEL_MAX && self->removedCards >= SCORE_ATTACK_CARD_COUNT_TO_LEVEL_UP) {
        self->level++;
        self->removedCards -= SCORE_ATTACK_CARD_COUNT_TO_LEVEL_UP;
    }
    if (didLevelUp) {
        SamplePlay(SampleLevelUp);
#if LOG_SCORE_ATTACK_LEVEL_UP
        playdate->system->logToConsole("Level Up: %d, speed: %f", self->level, (double)GetGameSpeed());
#endif
        SparksConstructor(MELSpriteGetFrame(self->levelCounter));
    }
}

static void playerMadeCombo(void * _Nullable userdata, int comboCount) {
    if (comboCount == 2) {
        unlockAchievement(AchievementChainX2);
    }
    if (comboCount == 4) {
        unlockAchievement(AchievementChainX2);
    }
}

static void setNextTarget(ScoreAttackScene * _Nonnull self) {
    if (self->nextTargetIndex < scoreAttackScores.count) {
        self->nextTarget = scoreAttackScores.memory[self->nextTargetIndex].super.value;
    } else {
        self->nextTarget = currentSaveGame.scoreAttackPersonalBest;
    }
}

static void setNextTargetOrHideCounter(ScoreAttackScene * _Nonnull self) {
    if (self->nextTargetIndex < 0) {
        playdate->sprite->setVisible(self->nextTargetLabel, false);
        playdate->sprite->setVisible(self->nextTargetCounter, false);
    } else {
        setNextTarget(self);
    }
}

#pragma mark - Boucle de jeu

static int update(void * _Nonnull userdata) {
    ScoreAttackScene *self = userdata;

    if (!playdate->system->getReduceFlashing()) {
        self->backgroundCamera.frame.origin.x += 5.0f * DELTA;
        
        if (self->speedY < 0.0f || self->backgroundCamera.frame.origin.y != 0.0f) {
            self->backgroundCamera.frame.origin.y = MELFloatMin(self->backgroundCamera.frame.origin.y + self->speedY * DELTA, 0.0f);
            self->speedY += DELTA * 500.0f;
        }
        if (self->timeBeforeJump) {
            self->timeBeforeJump = MELFloatMax(self->timeBeforeJump - DELTA, 0.0f);
            if (self->timeBeforeJump == 0.0f) {
                self->speedY = -100.0f;
            }
        }
    }

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;

    const uint8_t oldBoardCount = self->gameFlow.board->count;

    // Gère les boutons et l'IA.
    ActorUpdate(self->gameFlow.actor);

    // Déplace et affiche les sprites.
    playdate->sprite->updateAndDrawSprites();

    const uint8_t boardCount = self->gameFlow.board->count;

    if (boardCount != oldBoardCount) {
        self->score += SCORE_ATTACK_ATTACH_SCORE;
        if (boardCount == 0) {
            // TODO: Faire une animation "Perfect trick!" / kanzen ni kieta!
            self->score += SCORE_ATTACK_BOARD_CLEAR_BONUS * MELIntMax(self->level, 10);
            unlockAchievement(AchievementAllClear);
        }
        if (self->score >= 100000) {
            unlockAchievement(AchievementScoreAttack100000);
        }
        if (self->score >= 500000) {
            unlockAchievement(AchievementScoreAttack500000);
        }
    }

    // Identifie les changements dans le tableau de jeu.
    self->gameFlow.update(&self->gameFlow);

    if (self->nextTargetIndex >= 0 && self->score >= self->nextTarget) {
        self->nextTargetIndex--;
        SamplePlay(SampleOvertake);
        setNextTargetOrHideCounter(self);
    }
    return true;
}

static int updateLost(void * _Nullable userdata) {
    ScoreAttackScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->gameFlow.update(&self->gameFlow);

    if (self->time < 1.0f) {
        self->time += DELTA;
        return true;
    }

    if (MELControllerMake().pressedA || MELControllerMake().pressedB) {
        ScoreAttackSelectScene *scoreAttackSelectScene = ScoreAttackSelectSceneAlloc();
        MELSceneMakeCurrent(&scoreAttackSelectScene->super);
    }
    return true;
}
