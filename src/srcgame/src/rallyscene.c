//
//  rallyscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 20/07/2025.
//

#include "rallyscene.h"

#include "samples.h"
#include "music.h"
#include "carddeck.h"
#include "layersprite.h"
#include "cpu.h"
#include "instantcpu.h"
#include "patterncpu.h"
#include "savegame.h"
#include "opponent_portait.h"
#include "rally.h"
#include "rallyintroscene.h"
#include "rallyscorescene.h"
#include "rallyresultsscene.h"
#include "scores.h"
#include "text.h"
#include "counter.h"
#include "rect.h"
#include "stride.h"

#include "../gen/maps.h"
#include "../gen/spritensselect8x66x5.h"
#include "../gen/spriteopeningcarolinerunning.h"

static const MELLocalizedString kGameOver = (MELLocalizedString) {
    .text = {
        "Game Over",
        NULL,
        NULL,
        NULL
    }
};
static const MELLocalizedString kRetryOrQuit = (MELLocalizedString) {
    .text = {
        "Ⓐ Retry\nⒷ Quit",
        "Ⓐ Réessayer\nⒷ Quitter",
        "Ⓐ 再試する\nⒷ やめる",
    }
};
static const MELLocalizedString kRetryReduceScoreOrQuit = (MELLocalizedString) {
    .text = {
        "Ⓐ Retry\n  score -25%\n\nⒷ Quit",
        "Ⓐ Réessayer\n  score -25%\n\nⒷ Quitter",
        "Ⓐ 再試する\n  スコア-25%\n\nⒷ やめる",
    }
};
static const MELLocalizedString kRetryReduceScoreReduceSpeedOrQuit = (MELLocalizedString) {
    .text = {
        "Ⓐ Retry\n  score -25%\n  speed -25%\n\nⒷ Quit",
        "Ⓐ Réessayer\n  score -25%\n  vitesse -25%\n\nⒷ Quitter",
        "Ⓐ 再試する\n  スコア-25%\n  スピード-25%\n\nⒷ やめる",
    }
};

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static void beforeQuit(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);
static int updateWon(void * _Nullable userdata);
static int updateLost(void * _Nullable userdata);

static void playBackgroundMusic(RallyScene * _Nonnull self);

static void playerWon(void * _Nullable userdata, int _);
static void playerLost(void * _Nullable userdata, int _);
static void playerMayLose(void * _Nullable userdata, int mayLose);
static void playerSentChips(void * _Nullable userdata, int chips);
static void playerSentStar(void * _Nullable userdata, int stars);
static void playerClearedBoard(void * _Nullable userdata, int boardCount);
static void playerMadeCombo(void * _Nullable userdata, int comboCount);
static void playerMadePokerHand(void * _Nullable userdata, const void * _Nullable hand);

RallyScene * _Nonnull RallySceneAlloc(ArcadeGame game) {
    RallyScene *self = new(RallyScene);

    while (!SampleLoadNextSample());

    *self = (RallyScene) {
        .super = {
            .type = SceneTypeRallyGame,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .beforeQuit = beforeQuit,
        },
        .background = MELMapOpen(kMapNameFileNames[MapNameArcade]),
        .backgroundName = MapNameArcade,
        .cards = CardListMakeWithRandomCards(DECK_CARD_COUNT),
        .game = game,
    };
    return self;
}

RallyScene * _Nonnull RallySceneAllocWithSaveState(ArcadeSaveState saveState) {
    RallyScene *self = new(RallyScene);

    while (!SampleLoadNextSample());

    CardList cards = CardListMakeWithInitialCapacity(DECK_CARD_COUNT);
    const unsigned int length = (unsigned int) strlen(saveState.deck);
    for (unsigned int index = 0; index < length; index++) {
        CardListPush(&cards, CardForCharacter(saveState.deck[index]));
    }
    playdate->system->realloc(saveState.deck, 0);

    *self = (RallyScene) {
        .super = {
            .type = SceneTypeRallyGame,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .beforeQuit = beforeQuit,
        },
        .background = MELMapOpen(kMapNameFileNames[MapNameArcade]),
        .backgroundName = MapNameArcade,
        .cards = cards,
        .leftBoard = saveState.leftBoard,
        .rightBoard = saveState.rightBoard,
        .leftDeckIndex = saveState.leftDeckIndex,
        .rightDeckIndex = saveState.rightDeckIndex,
        .game = saveState.game,
    };
    return self;
}

#pragma mark - Fonctions privées

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeRallyGame) {
        playdate->system->error("Given scene is not a RallyScene instance");
        return;
    }
    RallyScene *self = (RallyScene *)scene;

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->background);

    GameFlowDeinit(&self->leftPlayerGameFlow);
    GameFlowDeinit(&self->rightPlayerGameFlow);

    CardListDeinit(&self->cards);

    if (self->dialogBorder) {
        MELNineSliceDealloc(self->dialogBorder);
        self->dialogBorder = NULL;
    }

    MELAchievementToastAdjustLeft = 0;
    MELAchievementToastAdjustWidth = 0;

    playdate->system->realloc(self, 0);
}

static void initGameFlows(RallyScene * _Nonnull self, Board * _Nonnull rightBoard) {
    GameFlowInit(&self->leftPlayerGameFlow, SideLeft, BoardAlloc(SideLeft), self->cards);
    GameFlowInit(&self->rightPlayerGameFlow, SideRight, rightBoard, self->cards);
    self->leftPlayerGameFlow.actor = ActorMakePlayer();

    self->leftPlayerGameFlow.cardIndex = self->leftDeckIndex;
    self->rightPlayerGameFlow.cardIndex = self->rightDeckIndex;

    if (self->leftBoard) {
        GameFlowPopulateWithCards(&self->leftPlayerGameFlow, self->leftBoard, (unsigned int) strlen(self->leftBoard));
        playdate->system->realloc(self->leftBoard, 0);
        self->leftBoard = NULL;
    }
    if (self->rightBoard) {
        GameFlowPopulateWithCards(&self->rightPlayerGameFlow, self->rightBoard, (unsigned int) strlen(self->rightBoard));
        playdate->system->realloc(self->rightBoard, 0);
        self->rightBoard = NULL;
    }

    const Encounter encounter = kRallyEncounters.memory[self->game.stage];
    Actor *cpuActor = encounter.actor;
    self->rightPlayerGameFlow.actor = cpuActor->alloc(cpuActor);

    if (self->game.loseStreak && cpuActor->alloc == InstantCpuAlloc) {
        InstantCpu *instantCpu = (InstantCpu *) self->rightPlayerGameFlow.actor;
        instantCpu->miss = MELIntMin(instantCpu->miss + self->game.loseStreak * 10, 75);
    } else if (self->game.loseStreak && cpuActor->alloc == PatternCpuAlloc) {
        PatternCpu *patternCpu = (PatternCpu *) self->rightPlayerGameFlow.actor;
        patternCpu->super.miss = MELIntMin(patternCpu->super.miss + self->game.loseStreak * 10, 75);
    }
}

static void init(MELScene * _Nonnull scene) {
    RallyScene *self = (RallyScene *)scene;

    MELAchievementToastAdjustLeft = 64;
    MELAchievementToastAdjustWidth = 64;

    const MapName mapName = self->backgroundName;
    MELMap *background = self->background;
    for (unsigned int index = 0; index < background->layerCount; index++) {
        MELLayer *layer = background->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);
        LCDSpriteRefListPush(&self->super.sprites, LayerSpriteConstructor(layer, image));
    }

    self->speed = 1.0f / self->game.speed;

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
        MELSpriteUpdate(sprite);

        if (instance.name == SpriteNamePortaits) {
            const Encounter encounter = kRallyEncounters.memory[self->game.stage];
            const Character character = encounter.opponent;
            MELSpriteSetAnimationAndDirection(melSprite, CharacterAnimationName[character], CharacterAnimationDirection[character]);
            OpponentPortaitConfigure(melSprite, sprite, rightBoard);
            playdate->sprite->setZIndex(sprite, ZINDEX_PORTAITS);
        } else if (instance.name == SpriteNameScoreAttackLabels) {
            playdate->sprite->setZIndex(sprite, ZINDEX_INGAME_UI);
        }
    }

    LCDSprite *scoreCounter = CounterConstructorWithUInt32Pointer(MELPointMake(258.0f, 170 + 18), MELHorizontalAlignmentRight, fontRoobert11Bold, &self->game.score);
    playdate->sprite->setZIndex(scoreCounter, ZINDEX_INGAME_UI);

    LCDSprite *timeCounter = CounterConstructorWithUInt32Pointer(MELPointMake(258.0f, 206 + 18), MELHorizontalAlignmentRight, fontRoobert11Bold, &self->timeInSeconds);
    playdate->sprite->setZIndex(timeCounter, ZINDEX_INGAME_UI);

    initGameFlows(self, rightBoard);

    MELEventBusAddListener(EventLeftSideLost, playerLost, self);
    MELEventBusAddListener(EventRightSideLost, playerWon, self);
    MELEventBusAddListener(EventLeftSideMayLose, playerMayLose, self);
    MELEventBusAddListener(EventLeftSideSentChips, playerSentChips, self);
    MELEventBusAddListener(EventLeftSideSentStar, playerSentStar, self);
    MELEventBusAddListener(EventLeftSideClearedBoard, playerClearedBoard, self);
    MELEventBusAddListener(EventLeftSideMadeCombo, playerMadeCombo, self);
    MELEventBusAddListenerVoidPointer(EventLeftSideMadePokerHand, playerMadePokerHand, self);

    playBackgroundMusic(self);

    camera.frame.origin = MELPointZero;
    setRefreshRate(DEFAULT_REFRESH_RATE);
}

#if ENABLE_ARCADE_BOARD_SERIALIZATION
static char * _Nullable serializeBoard(Board * _Nonnull board) {
    MELCharList result = MELCharListEmpty;
    char line[BOARD_COLUMNS];
    Card card;
    for (unsigned int y = 0; y < BOARD_ROWS + BOARD_HIDDEN_ROWS; y++) {
        unsigned int count = 0;
        for (unsigned int x = 0; x < BOARD_COLUMNS; x++) {
            const MELIntPoint location = (MELIntPoint) {
                .x = x,
                .y = y,
            };
            if (BoardLocationIsEmpty(board, location)) {
                line[x] = '_';
            } else if (BoardCardAtLocation(board, MELIntPointMake(x, y), &card)) {
                line[x] = CardToCharacter(card);
                count++;
            } else {
                line[x] = 'z';
                count++;
            }
        }
        if (count > 0) {
            MELCharListAddAll(&result, (MELCharList) {
                .count = BOARD_COLUMNS,
                .memory = line,
            });
        }
    }
    if (result.count) {
        MELCharListPush(&result, '\0');
    }
    return result.memory;
}
#endif

static void beforeQuit(MELScene * _Nonnull scene) {
    RallyScene *self = (RallyScene *)scene;

    if (!self->earnedScore && self->game.stage == 0) {
        return;
    }
    if (self->earnedScore) {
        // Victoire
        ArcadeGame game = self->game;
        game.score = self->baseScore + self->earnedScore;
        
        if (ArcadeGameIsEnding(game)) {
            ScoresAdd(ScoreBoardRally, game.score);
            return;
        } else {
            if ((game.winStreak % 3) == 0) {
                game.speed += 0.5f;
            }
            currentSaveGame.arcadeSaveState = (ArcadeSaveState) {
                .game = game,
            };
        }
    } else if (self->baseScore) {
        // Défaite
        ArcadeGame game = self->game;
        game.score = self->baseScore * 0.75f;
        currentSaveGame.arcadeSaveState = (ArcadeSaveState) {
            .game = game,
        };
    } else {
#if ENABLE_ARCADE_BOARD_SERIALIZATION
        char *deck = playdate->system->realloc(NULL, (DECK_CARD_COUNT + 1) * sizeof(char));
        for (unsigned int index = 0; index < DECK_CARD_COUNT; index++) {
            deck[index] = CardToCharacter(self->cards.memory[index]);
        }
        deck[DECK_CARD_COUNT] = '\0';
        
        // TODO: Vérifier l'état du flow et revenir en arrière de 2 cartes si besoin ?
        
        currentSaveGame.arcadeSaveState = (ArcadeSaveState) {
            .game = self->game,
            .deck = deck,
            .leftDeckIndex = self->leftPlayerGameFlow.cardIndex,
            .rightDeckIndex = self->rightPlayerGameFlow.cardIndex,
            .leftBoard = serializeBoard(self->leftPlayerGameFlow.board),
            .rightBoard = serializeBoard(self->rightPlayerGameFlow.board),
        };
#else
        currentSaveGame.arcadeSaveState = (ArcadeSaveState) {
            .game = self->game,
        };
#endif
    }
    SaveGameToDisk(&currentSaveGame);
}

static void playBackgroundMusic(RallyScene * _Nonnull self) {
    const ArcadeGame game = self->game;
    switch (game.stage) {
        case 0 ... 4:
            MusicManagerPlay("musics/learning", 0, 0.25f);
            break;
        case 5 ... 10:
            MusicManagerPlay("musics/performing", 0, 0.25f);
            break;
        case 11 ... 14:
            MusicManagerPlay("musics/boss", 0, 0.25f);
            break;
        default:
            MusicManagerPlay("musics/performinghard", 0, 0.25f);
            break;
    }
    if ((game.stage % 5) >= 3 && game.speed > 1.0f) {
        MusicManagerSetRate(1.25f);
    }
}

static void restart(RallyScene * _Nonnull self) {
    ArcadeGame game = self->game;
    game.score = self->baseScore * 0.75f;
    game.continueCount++;
    RallyScene *rallyScene = RallySceneAlloc(game);
    MELSceneMakeCurrent(&rallyScene->super);
}

#pragma mark - Événements

static void playerWon(void * _Nullable userdata, int _) {
    RallyScene *self = userdata;
    const int nextStage = self->game.stage + 1;
    if (nextStage > currentSaveGame.rallyBestStage) {
        currentSaveGame.rallyBestStage = nextStage;
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

    self->dialogBorder = MELNineSliceMakeWithBitmapTable(spriteNsSelect8x66x5.palette, 0, (MELIntRectangle) {
        .origin = { .x = 8, .y = 6 },
        .size = { .width = 4, .height = 5 }
    });

    // Déverrouille le succès de fin après la victoire sur le stage 19.
    if (self->game.stage == kRallyEncounters.count - 2) {
        unlockAchievement(AchievementArcadeEnd);
    }

    // Calcul du bonus
    ArcadeGame game = self->game;
    game.time += self->time;
    game.stage++;
    game.winStreak++;
    game.bestWinStreak = MELIntMax(game.bestWinStreak, game.winStreak);
    game.loseStreak = 0;

    self->bonusScore = self->earnedScore = (MELFloatMax(18000.0f - self->time * 100, 0.0f) + 5000.0f) * game.speed;
    self->baseScore = game.score;

    OnlineScoresAddRallyScore(game.score + self->earnedScore);

    self->game = game;
    self->time = 0.0f;
    playdate->system->setUpdateCallback(updateWon, self);
}

static void playerLost(void * _Nullable userdata, int _) {
    RallyScene *self = userdata;
    currentSaveGame.loses++;
    SaveGameToDisk(&currentSaveGame);

    MusicManagerPlay("musics/outoftricks", 1, 0.1f);

    MELAchievementToastAdjustLeft = 0;
    MELAchievementToastAdjustWidth = 0;
    unlockAchievement(AchievementGameDefeat1);

    self->dialogBorder = MELNineSliceMakeWithBitmapTable(spriteNsSelect8x66x5.palette, 0, (MELIntRectangle) {
        .origin = { .x = 8, .y = 6 },
        .size = { .width = 4, .height = 5 }
    });

    ArcadeGame game = self->game;
    self->baseScore = game.score;

    if (game.loseStreak == 0) {
        unlockAchievement(AchievementArcadeFirst);
        ScoresAdd(ScoreBoardRally, game.score);
    }
    // NOTE: Pas la peine d'envoyer le score dans le cas inverse car il est inférieur au score précédent. Le score est d'ailleurs déjà envoyé en ligne.

    if (game.stage > 0) {
        game.deathCount++;
        game.winStreak = 0;
        game.loseStreak++;
        if (self->game.speed > 1.0f && game.loseStreak % 2 == 0) {
            game.speed = MELFloatMax(game.speed - 0.25f, 1.0f);
        }
    }
    game.time += self->time;
    self->game = game;
    self->time = 0.0f;

    playdate->system->setUpdateCallback(updateLost, self);
}

static void playerMayLose(void * _Nullable userdata, int mayLose) {
    RallyScene *self = userdata;
    self->pinch = self->pinch || mayLose;
    if (mayLose) {
        MusicManagerPlay("musics/pinch", 0, 0.25f);
    } else {
        playBackgroundMusic(self);
    }
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

static float customEasing(float from, float to, float value) {
    const float x = MELProgress(from, to, value);
    if (x < 0.33f) {
        return MELEaseInOut(0, 0.33f, x) * 0.45f;
    } else if (x < 0.66f) {
        return MELProgress(0.33f, 0.66f, x) * 0.1f + 0.45f;
    } else {
        return MELEaseInOut(0.66f, 1.0f, x) * 0.45f + 0.55f;
    }
}

#pragma mark - Boucle de jeu

static int update(void * _Nonnull userdata) {
    RallyScene *self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;

    self->time += DELTA;
    self->timeInSeconds = self->time;

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

static int updateSpeedUp(void * _Nullable userdata) {
    RallyScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->leftPlayerGameFlow.update(&self->leftPlayerGameFlow);
    self->rightPlayerGameFlow.update(&self->rightPlayerGameFlow);

    const float time = self->time;
    self->time = time + DELTA;

    MELController controller = MELControllerMake();
    
    if (time >= 4.0f || (time > 0.1f && controller.pressedA)) {
        RallyIntroScene *rallyIntroScene = RallyIntroSceneAlloc(self->game);
        MELSceneMakeCurrent(&rallyIntroScene->super);
    }
    return true;
}

static int updateWon(void * _Nullable userdata) {
    RallyScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->leftPlayerGameFlow.update(&self->leftPlayerGameFlow);
    self->rightPlayerGameFlow.update(&self->rightPlayerGameFlow);

    const float time = self->time;
    self->time = time + DELTA;

    if (time < 1.5f) {
        if (time < 0.5f && self->time >= 0.5f) {
            LCDSprite *dialogSprite = MELDialogMake((MELDialogConfig) {
                .frame = {
                    .origin = {
                        .x = 70,
                        .y = 162
                    },
                    .size = {
                        .width = 132,
                        .height = 126
                    }
                },
                .border = self->dialogBorder,
            });
            LCDSpriteRefListPush(&self->super.sprites, dialogSprite);
        }
        const float left = 16.0f;
        if (time < 0.75f && self->time >= 0.75f) {
            LCDSprite *timeSprite = TextConstructorArgInt(MELPointMake(left, 124), smallFont, kDrawModeCopy, MELHorizontalAlignmentLeft, "Time: %u", self->timeInSeconds);
            playdate->sprite->setZIndex(timeSprite, 99);
        }
        if (time < 1.0f && self->time >= 1.0f) {
            LCDSprite *speedSprite = TextConstructorArgFloat(MELPointMake(left, 146), smallFont, kDrawModeCopy, MELHorizontalAlignmentLeft, "Speed: x%.2f", self->game.speed);
            playdate->sprite->setZIndex(speedSprite, 99);
        }
        if (time < 1.25f && self->time >= 1.25f) {
            LCDSprite *bonusSprite = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, "Bonus:", 6);
            MELAlignmentSet(bonusSprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentMiddle, MELPointMake(left, 168));
            LCDSprite *counterSprite = CounterConstructorWithUInt32Pointer(MELPointMake(124, 198), MELHorizontalAlignmentRight, currentFont, &self->bonusScore);
            playdate->sprite->setZIndex(bonusSprite, 99);
            playdate->sprite->setZIndex(counterSprite, 99);
        }
        return true;
    }

    const MELBoolean pressedA = MELControllerMake().pressedA;

    if (self->bonusScore && time >= 2.0f) {
        // Ajout du bonus au score.
        if (self->lastSound <= 0.0f) {
            const uint8_t sound = self->sound;
            SamplePlay(sound ? SampleCounter2 : SampleCounter1);
            self->sound = !sound;
            self->lastSound += 0.1f;
        }
        self->lastSound -= DELTA;
        const float progress = MELProgress(2.0f, 3.0f, time);
        self->bonusScore = self->earnedScore * (1.0f - progress);
        self->game.score = self->baseScore + self->earnedScore * progress;
    }

    if (self->bonusScore && pressedA) {
        // Termine le décompte du score.
        self->bonusScore = 0;
        self->game.score = self->baseScore + self->earnedScore;
    }
    else if (pressedA) {
        // Passe à la scène suivante.
        ArcadeGame game = self->game;
        MELScene *nextScene;
        if (!ArcadeGameIsEnding(game)) {
            if ((game.winStreak % 3) == 0) {
                game.speed += 0.5f;

                LCDSprite *sprite = RectConstructor(96.0f, 0.5f);
                playdate->sprite->setZIndex(sprite, 100);

                loadSpriteOpeningCarolineRunningPalette();
                sprite = StrideConstructor(&spriteOpeningCarolineRunning,
                    MELPointMake(-spriteOpeningCarolineRunning.size.width / 2, LCD_ROWS / 2 - spriteOpeningCarolineRunning.size.height / 2),
                    MELPointMake(LCD_COLUMNS + spriteOpeningCarolineRunning.size.width / 2, LCD_ROWS / 2 - spriteOpeningCarolineRunning.size.height / 2),
                    0.5f, 2.8f);
                StrideSetEasingFunction(sprite, customEasing);
                StrideSetDestroyWhenStrideEnds(sprite, true);
                playdate->sprite->setZIndex(sprite, 101);
                LCDSpriteRefListPush(&self->super.sprites, sprite);

                sprite = TextConstructor(MELPointZero, fontMagic, kDrawModeFillWhite, "SPEED UP!", 9);
                MELAlignmentSet(sprite, MELHorizontalAlignmentRight, MELVerticalAlignmentTop, MELPointMake(0, LCD_ROWS / 2 + 8));
                StrideSpriteBy(sprite, MELPointMake(LCD_COLUMNS + LCDSpriteGetFrame(sprite).size.width, 0), 0.6f, 3.0f);
                StrideSetEasingFunction(sprite, customEasing);
                StrideSetDestroyWhenStrideEnds(sprite, true);
                playdate->sprite->setZIndex(sprite, 101);

                unlockAchievement(AchievementArcadeSpeedUp);
                SamplePlay(SampleLevelUp);
                self->game = game;
                self->time = 0.0f;
                playdate->system->setUpdateCallback(updateSpeedUp, self);
                return true;
            }
            RallyIntroScene *rallyIntroScene = RallyIntroSceneAlloc(game);
            nextScene = &rallyIntroScene->super;
        } else {
            unlockAchievement(AchievementArcadeFirst);
            ScoresAdd(ScoreBoardRally, game.score);
            RallyResultsScene *resultsScene = RallyResultsSceneAlloc(self->game, true);
            nextScene = &resultsScene->super;
        }
        MELSceneMakeCurrent(nextScene);
    }
    return true;
}

static int updateReduceScore(void * _Nullable userdata) {
    RallyScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->leftPlayerGameFlow.update(&self->leftPlayerGameFlow);
    self->rightPlayerGameFlow.update(&self->rightPlayerGameFlow);

    const float time = self->time;
    self->time = time + DELTA;

    MELController controller = MELControllerMake();
    
    if (time >= 2.0f || (time > 0.1f && controller.pressedA)) {
        restart(self);
    } else {
        if (time < 1.0f && self->lastSound <= 0.0f) {
            const uint8_t sound = self->sound;
            SamplePlay(sound ? SampleCounter2 : SampleCounter1);
            self->sound = !sound;
            self->lastSound += 0.1f;
        }
        self->lastSound -= DELTA;
        const float progress = MELProgress(0.0f, 1.0f, time);
        self->game.score = self->baseScore - progress * self->baseScore / 4;
    }
    return true;
}

static int updateLost(void * _Nullable userdata) {
    RallyScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->leftPlayerGameFlow.update(&self->leftPlayerGameFlow);
    self->rightPlayerGameFlow.update(&self->rightPlayerGameFlow);

    const float time = self->time;
    self->time = time + DELTA;
    if (time < 1.5f) {
        if (time < 0.5f && self->time >= 0.5f) {
            const MELBoolean reduceSpeed = self->game.speed > 1.0f && self->game.loseStreak % 2 == 0;
            const int lineCount = self->game.stage == 0
                ? 2
                : (reduceSpeed ? 5 : 4);
            const int height = 8 + 6 + playdate->graphics->getFontHeight(currentFont) + 8 + playdate->graphics->getFontHeight(smallFont) * lineCount + 6 + 8;

            LCDSprite *dialogSprite = MELDialogMake((MELDialogConfig) {
                .frame = {
                    .origin = {
                        .x = 70,
                        .y = 162
                    },
                    .size = {
                        .width = 140,
                        .height = height
                    }
                },
                .border = self->dialogBorder,
            });
            self->dialog = dialogSprite;
            LCDSpriteRefListPush(&self->super.sprites, dialogSprite);
        }
        if (time < 0.75f && self->time >= 0.75f) {
            const MELRectangle dialogFrame = LCDSpriteGetFrame(self->dialog);
            const float top = dialogFrame.origin.y - dialogFrame.size.height / 2 + 12;

            LCDSprite *gameOver = TextConstructor(MELPointZero, currentFont, kDrawModeCopy, MELLocalizedStringGetText(kGameOver), 9);
            MELAlignmentSet(gameOver, MELHorizontalAlignmentCenter, MELVerticalAlignmentTop, MELPointMake(70, top));
            playdate->sprite->setZIndex(gameOver, 99);
            playdate->sprite->setTag(gameOver, 1);
        }
        if (time < 1.0f && self->time >= 1.0f) {
            const MELRectangle dialogFrame = LCDSpriteGetFrame(self->dialog);
            const float top = dialogFrame.origin.y - dialogFrame.size.height / 2 + 12 + playdate->graphics->getFontHeight(currentFont) + 8;

            MELLocalizedString text;
            if (self->game.stage == 0) {
                text = kRetryOrQuit;
            } else if (self->speed > 1.0f && self->game.loseStreak % 2 == 0) {
                text = kRetryReduceScoreReduceSpeedOrQuit;
            } else {
                text = kRetryReduceScoreOrQuit;
            }
            LCDSprite *retryOrQuit = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, MELLocalizedStringGetText(text), 99);
            MELAlignmentSet(retryOrQuit, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop, MELPointMake(12, top));
            playdate->sprite->setZIndex(retryOrQuit, 99);
            playdate->sprite->setTag(retryOrQuit, 1);
        }
        return true;
    }

    const MELController controller = MELControllerMake();
    if (controller.pressedA && self->game.score > 0) {
        self->time = 0.0f;

        const LCDSpriteRefList sprites = self->super.sprites;
        for (int index = sprites.count - 1; index >= 0; index--) {
            LCDSprite *sprite = sprites.memory[index];
            if (playdate->sprite->getTag(sprite) == 1) {
                MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
                melSprite->class->destroy(sprite);
            }
        }
        MELDialogClose(self->dialog);

        playdate->system->setUpdateCallback(updateReduceScore, self);
    } else if (controller.pressedA) {
        restart(self);
    } else if (controller.pressedB && self->game.stage > 0) {
        RallyResultsScene *resultsScene = RallyResultsSceneAlloc(self->game, false);
        MELSceneMakeCurrent(&resultsScene->super);
    } else if (controller.pressedB) {
        RallyScoreScene *scoreScene = RallyScoreSceneAlloc();
        MELSceneMakeCurrent(&scoreScene->super);
    }
    return true;
}
