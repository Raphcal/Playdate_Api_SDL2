//
//  gamescene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 24/01/2023.
//

#include "gamescene.h"

#include "references.h"

#include "titlescene.h"
#include "scoreentryscene.h"
#include "ending1scene.h"
#include "ending2scene.h"
#include "savegame.h"

#include "gui_lives.h"
#include "gui_score.h"
#include "gui_time.h"
#include "camera.h"
#include "gameover.h"
#include "layersprite.h"
#include "layerwhitegrid.h"
#include "cloudtransition.h"
#include "progressbar.h"
#include "counter.h"
#include "image.h"
#include "text.h"
#include "fade.h"
#include "fadeexplosion.h"
#include "transitionout.h"
#include "stride.h"
#include "samples.h"
#include "rankingboard.h"
#include "new_best.h"
#include "explosion.h"

#include "../gen/spritekiken.h"
#include "../gen/spritebikkuri.h"
#include "../gen/spritechuui.h"
#include "../gen/spritego.h"
#include "../gen/spriteclear.h"
#include "../gen/spriteotsukare.h"
#include "../gen/spriterank.h"
#include "../gen/spritewonderfulrank.h"

static const int8_t kLives = 3;
static const float kCheckpointDuration = 30.0f;

typedef struct {
    LCDSprite * _Nullable livesLabel;
    LCDSprite * _Nullable timeLeft;
    LCDSprite * _Nullable maxCombo;
    LCDSprite * _Nullable scoreCounter;
} ClearSprites;

typedef enum {
    TagInitial,
    TagWait,
    TagLives,
    TagLivesRank,
    TagTimeLeft,
    TagTimeLeftRank,
    TagMaxCombo,
    TagMaxComboRank,
    TagScoreCounter,
    // NOTE: TagLivesLabel est placé ici car il n'y a pas d'animation pour lui.
    // Le code saute de TagScoreCounter vers TagClearScreenScoreUpdate dans updateClearScreen.
    // Ce tag ne sert qu'à retrouver le bon sprite dans la liste des sprites affichés.
    TagLivesLabel,
    TagClearScreenScoreUpdate,
} GameSceneTag;

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);
static void playMusic(GameScene * _Nonnull self, WaveType oldWaveType);
static void loadMusic(GameScene * _Nonnull self);
static void fadeOutMusic(GameScene * _Nonnull self);

static void setWave(GameScene * _Nonnull self, Wave * _Nonnull wave);

static void resetCombo(GUICombo * _Nonnull self);

static void showGameOver(GameScene * _Nonnull self);
static int updateGameOver(void * _Nonnull userdata);

static void showWarningSign(GameScene * _Nonnull gameScene, MELBoolean isLastBoss);
static int updateWarningSign(void * _Nonnull userdata);

static void createClearLabels(GameScene * _Nonnull self);
static void createRankLetter(int rankIndex, Rank rank, MELBoolean animate);
static void computeRanks(GameScene * _Nonnull self);
static ClearSprites findClearSprites(GameScene * _Nonnull self);
static void showPressAToContinue(GameScene * _Nonnull self, const ClearSprites sprites);
static int updateWaitClearLabelStrideCompletion(void * _Nonnull userdata);
static int updateWaitScoreCounterStrideCompletion(void * _Nonnull userdata);
static int updateClearScreen(void * _Nonnull userdata);
static int updatePressAToShowEnding(void * _Nonnull userdata);
static int updateGoToEnding(void * _Nonnull userdata);
static LCDSprite * _Nonnull strideTextHorizontal(GameScene * _Nonnull self, const char * _Nonnull text, float left, float centerY, float delay, float duration);

static int updatePause(void * _Nullable userdata);

static void destroyBullets(GameScene * _Nonnull self);

static LCDSprite * _Nullable getPlayerSprite(GameScene * _Nonnull self);

#pragma mark - Fonctions publiques

GameScene * _Nonnull GameSceneAlloc(PlayerCharacter playerCharacter) {
    GameScene *self = playdate->system->realloc(NULL, sizeof(GameScene));
    WaveList waves = WaveListMakeWithInitialCapacity(kMapNameCount);
    WaveRefList checkpoints = WaveRefListMakeWithInitialCapacity(kMapNameCount);
    for (unsigned int index = MapNameWave0Tutorial; index < kMapNameCount; index++) {
        Wave wave = {};
        WaveInit(&wave, (MapName)index);
        if (wave.type != WaveTypeOther) {
            if (wave.startTime >= 0) {
                WaveRefListPush(&checkpoints, waves.memory + waves.count);
            }
            WaveListPush(&waves, wave);
        } else {
            WaveDeinit(&wave);
        }
    }

    while (!SampleLoadNextSample());

    *self = (GameScene) {
        .super = {
            .type = SceneTypeGame,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .save = GameSceneSaveState,
            .sprites = LCDSpriteRefListEmpty,
        },
        .playerCharacter = playerCharacter,
        .lives = kLives,
        .nextOneUp = ONEUP_SCORE,
        .waves = waves,
        .wave = waves.memory,
        .background = MELMapOpen(kMapNameFileNames[MapNameTokyo]),
        .backgroundName = MapNameTokyo,
        .checkpoints = checkpoints,
        .checkpointIndex = 0,
        .lastCheckpointClearTime = 0.0f,
        .filePlayer = playdate->sound->fileplayer->newPlayer(),
        .rank = INT32_MAX,
    };
    return self;
}

GameScene * _Nonnull GameSceneAllocWithTransition(PlayerCharacter playerCharacter) {
    GameScene *self = GameSceneAlloc(playerCharacter);
    self->showTransition = true;
    return self;
}

GameScene * _Nullable GameSceneGetCurrent(void) {
#if CHECK_CLASS_CAST
    if (currentScene->type != SceneTypeGame) {
        playdate->system->error("Expected current scene to be GameScene (%d) but was: %d", SceneTypeGame, currentScene->type);
    }
#endif
    return (GameScene *) currentScene;
}

void GameSceneDestroyBullets(void) {
    destroyBullets(GameSceneGetCurrent());
}

void GameSceneScoreAdd(int value, int combo) {
    if (currentScene->type != SceneTypeGame) {
        return;
    }
    GameScene *self = (GameScene *) currentScene;
    if (self->gameOver) {
        // Quand GameOver est affiché, le score ne doit plus être modifié.
        return;
    }

    const float comboMultiplier = 1.0f + self->combo.combo / 10.0f;

    const unsigned int oldScore = self->score;
    unsigned int score = oldScore + value * (combo + 1) * comboMultiplier;
    const unsigned int nextOneUp = self->nextOneUp;
    if (score >= nextOneUp) {
        self->nextOneUp = nextOneUp + ONEUP_SCORE;
        if (self->lives < MAXIMUM_LIFE_COUNT) {
            self->lives++;
            SamplePlay(SampleOneUp);
        } else {
            score += MAXIMUM_LIFE_SCORE_BONUS;
        }
    }
    self->score = score;

#if ENABLE_ONLINE_SCOREBOARDS
    // Chargement du moins bon score en ligne pour le personnage actuel pour éviter d'afficher
    // "bravo tu es dans le top" si le joueur ne verra pas son score dans le top à la fin de
    // la partie.
    const uint32_t worstOnlineScore = worstScores[self->playerCharacter];
    if (onlineScores.count && worstOnlineScore && score >= worstOnlineScore) {
        const int oldRank = self->rank;
        const int rank = ScoreListFindRank(onlineScores, score);
        self->rank = rank;
        switch (rank) {
            case 0:
                if (oldRank > 0) {
                    NewBestConstructorWithMessage(NewBestMessageWorldRecord);
                    return;
                }
                break;
            case 1:
                if (oldRank > 1) {
                    NewBestConstructorWithMessage(NewBestMessage2ndPlace);
                    return;
                }
                break;
            case 2:
                if (oldRank > 2) {
                    NewBestConstructorWithMessage(NewBestMessage3rdPlace);
                    return;
                }
                break;
            case 3 ... 9:
                if (oldRank > 9) {
                    NewBestConstructorWithMessage(NewBestMessageTop10Reached);
                    return;
                }
                break;
            case 10 ... 19:
                if (oldRank > 19) {
                    NewBestConstructorWithMessage(NewBestMessageTop20Reached);
                    return;
                }
                break;
            default:
                break;
        }
    }
    if (localBestScore && oldScore <= localBestScore && score > localBestScore) {
        NewBestConstructorWithMessage(NewBestMessagePersonalBest);
    }
#else
    if (localScores.count && score >= localScores.memory[localScores.count - 1].super.value) {
        const int oldRank = self->rank;
        const int rank = ScoreListFindRank(localScores, score);
        self->rank = rank;
        switch (rank) {
            case 0:
                if (oldRank > 0) {
                    NewBestConstructorWithMessage(NewBestMessageYouAreFirst);
                    return;
                }
                break;
            case 1:
                if (oldRank > 1) {
                    NewBestConstructorWithMessage(NewBestMessage2ndPlace);
                    return;
                }
                break;
            case 2:
                if (oldRank > 2) {
                    NewBestConstructorWithMessage(NewBestMessage3rdPlace);
                    return;
                }
                break;
            case 3 ... 9:
                if (oldRank > 9) {
                    NewBestConstructorWithMessage(NewBestMessageTop10Reached);
                    return;
                }
                break;
            case 10 ... 19:
                if (oldRank > 19) {
                    NewBestConstructorWithMessage(NewBestMessageTop20Reached);
                    return;
                }
                break;
            default:
                break;
        }
    }
#endif
}

void GameSceneComboIncrement(void) {
    if (currentScene->type != SceneTypeGame) {
        return;
    }
    GameScene *self = (GameScene *) currentScene;
    if (self->wave->type != WaveTypeNormal) {
        // Comptage des combos que pendant les vagues normales.
        return;
    }
    GUICombo combo = self->combo;
    combo.timer = COMBO_DURATION;
    if (combo.combo == 0) {
        GUIComboAnimateAppear(combo);
    }
    combo.combo = MELIntMin(combo.combo + 1, 999);
    if (combo.combo > combo.maxCombo) {
        combo.maxCombo = combo.combo;
    }

    CounterSetValue(combo.counter, combo.combo);
    ProgressBarSetProgress(combo.progressBar, 1.0f);
    self->combo = combo;
}

void GameSceneComboFillTimer(void) {
    if (currentScene->type != SceneTypeGame) {
        return;
    }
    GameScene *self = (GameScene *) currentScene;
    GUICombo combo = self->combo;
    if (combo.timer > 0.0f) {
        combo.timer = COMBO_DURATION;
        ProgressBarSetProgress(combo.progressBar, 1.0f);
        self->combo = combo;
    }
}

void GameSceneLifeSubstractOne(void) {
    GameScene *self = GameSceneGetCurrent();
    self->lives--;
    self->deathCount++;
}

void GameSceneRestartAfterDeath(void) {
    GameScene *self = GameSceneGetCurrent();

    const MELBoolean isGameOver = self->lives <= 0;

    if (isGameOver && self->wave->mapName == MapNameWaveBoss && MELSceneFindSpriteByName(SpriteNameGunnerBot)) {
        // Mort pendant le boss optionnel, affichage de la fin.
        GameSceneFadeToClearScreen();
        return;
    } else if (isGameOver) {
        showGameOver(self);

#if ENABLE_ONLINE_SCOREBOARDS
        // Publication du score.
        OnlineScoresAdd(self->playerCharacter, self->score);
#endif
        return;
    }
    PlayerReset(self->player, self->playerSprite);
    resetCombo(&self->combo);
}

void GameSceneNextWave(void) {
    GameScene *self = GameSceneGetCurrent();
    setWave(self, self->wave + 1);
}

void GameSceneNextWaveWithGameScene(GameScene * _Nonnull self) {
    setWave(self, self->wave + 1);
}

void GameSceneFadeToClearScreen(void) {
    GameScene *self = GameSceneGetCurrent();
    fadeOutMusic(self);
    // Fait sortir le joueur vers la droite.
    // Si les vies == 0, le joueur est invisible donc pas de problème.
    // Permet de restaurer correctement l'état au chargement.
    PlayerSetClearState(self->player, self->playerSprite, self->lives > 0);
    self->checkpointIndex = 0;

    LCDSpriteRefList sprites = self->others;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        if (playdate->sprite->getZIndex(sprite) == ZINDEX_GUI) {
            MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
            MELRectangle frame = melSprite->frame;
            StrideSpriteTo(sprite, (MELPoint) { .x = frame.origin.x, .y = frame.origin.y - 88 }, 0.5f, 0.5f);
        }
    }
    loadSpriteClearPalette();
    loadSpriteOtsukarePalette();
    LCDSprite *clear = StrideConstructor(&spriteClear, MELPointMake(350, -spriteClear.size.height / 2), MELPointMake(350, LCD_ROWS / 2), 1.0f, 0.5f);
    LCDSprite *otsukare = StrideConstructor(&spriteOtsukare, MELPointMake(305, -spriteOtsukare.size.height / 2), MELPointMake(305, LCD_ROWS / 2), 1.25f, 0.5f);
    LCDSpriteRefListPush(&self->others, clear);
    LCDSpriteRefListPush(&self->others, otsukare);
    playdate->sprite->setZIndex(clear, ZINDEX_GUI);
    playdate->sprite->setZIndex(otsukare, ZINDEX_GUI);

    computeRanks(self);

    // TODO: Couper la musique et jouer un jingle.

    playdate->system->setUpdateCallback(updateWaitClearLabelStrideCompletion, self);
}

void GameSceneRestoreState(GameScene * _Nonnull self) {
    if (self->player->state == PlayerStateClear) {
        computeRanks(self);
        // GameSceneFadeToClearScreen mets checkpointIndex à 0 donc il faut vérifier avant la bonne valeur.
        const MELBoolean isClearDone = self->checkpointIndex == TagClearScreenScoreUpdate;
        GameSceneFadeToClearScreen();
        if (isClearDone) {
            self->checkpointIndex = TagClearScreenScoreUpdate;

            createClearLabels(self);
            const ClearSprites sprites = findClearSprites(self);
            StrideSkip(sprites.livesLabel);
            StrideSkip(sprites.timeLeft);
            StrideSkip(sprites.maxCombo);
            StrideSkip(sprites.scoreCounter);
            createRankLetter(kRankLives, self->ranks.lives, false);
            createRankLetter(kRankTimeLeft, self->ranks.timeLeft, false);
            createRankLetter(kRankMaxCombo, self->ranks.combo, false);
            CountUpdateAnchorWithCurrentOrigin(sprites.scoreCounter);
            showPressAToContinue(self, sprites);
        }
    } else {
        if (MELSceneFindSpriteByName(SpriteNameGunnerBot) != NULL) {
            self->wave->type = WaveTypeOther;
        }
        playdate->system->setUpdateCallback(updatePause, self);
        playdate->system->resetElapsedTime();
        playdate->sprite->updateAndDrawSprites();
    }
}

void MELSpriteDeallocFromGameSceneOthers(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    GameScene *gameScene = GameSceneGetCurrent();
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("MELSpriteDeallocFromGameSceneOthers(%x, %x): %d", sprite, self, self->definition.name);
    int index = LCDSpriteRefListRemoveSwapEntry(&gameScene->others, sprite);
    if (index < 0) {
        playdate->system->logToConsole("LCDSpriteRefListRemoveSwapEntry: sprite %x not found (%d)", sprite, index);
    }
#else
    LCDSpriteRefListRemoveSwapEntry(&gameScene->others, sprite);
#endif
    MELAnimationDealloc(self->animation);
    self->animation = NULL;
    if (self->hitbox != NULL) {
        MELHitboxDeinit(self->hitbox);
        playdate->system->realloc(self->hitbox, 0);
        self->hitbox = NULL;
    }
    playdate->system->realloc(self, 0);
    playdate->sprite->removeSprite(sprite);
    playdate->sprite->freeSprite(sprite);
}

#pragma mark - Fonctions privées

static LCDSprite * _Nullable getPlayerSprite(GameScene * _Nonnull self) {
    LCDSpriteRefList sprites = self->super.sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.type == MELSpriteTypePlayer) {
            return sprite;
        }
    }
    return NULL;
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeGame) {
        playdate->system->error("Given scene is not a GameScene instance");
        return;
    }
    GameScene *self = (GameScene *)scene;

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    LCDSpriteRefListDeallocReverse(&self->crystals);
    LCDSpriteRefListDeallocReverse(&self->others);

    // Désallocation des champs de GameScene
    if (self->gameOver) {
        playdate->system->realloc(self->gameOver, 0);
        self->gameOver = NULL;
    }
    if (self->warningSign) {
        playdate->system->realloc(self->warningSign, 0);
        self->warningSign = NULL;
    }
    playdate->sound->fileplayer->stop(self->filePlayer);
    playdate->sound->fileplayer->freePlayer(self->filePlayer);
    self->playerSprite = NULL;
    if (self->background) {
        MELMapDealloc(self->background);
        self->background = NULL;
    }
    LCDSpriteRefListDeallocReverse(&self->layers);
    if (self->cloud) {
        playdate->graphics->freeBitmap(self->cloud);
    }
    WaveListDeinitWithDeinitFunction(&self->waves, WaveDeinit);
    self->wave = NULL;
    WaveRefListDeinit(&self->checkpoints);
    playdate->system->realloc(self, 0);
}

static void init(MELScene * _Nonnull scene) {
    GameScene *self = (GameScene *)scene;
    if (!self->isLoadingSave) {
        WaveInstantiate(self->wave);

        LCDSprite *playerSprite = getPlayerSprite(self);
        self->playerSprite = playerSprite;
        Player *player = self->player = playdate->sprite->getUserdata(playerSprite);
        PlayerSetCharacter(player, playerSprite, self->playerCharacter);

        playMusic(self, WaveTypeOther);
    } else {
        loadMusic(self);
    }

    MELMap *background = self->background;
    const MapName mapName = self->backgroundName;
    LCDSpriteRefList layers = self->layers;
    for (unsigned int index = 0; index < background->layerCount; index++) {
        MELLayer *layer = background->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);

        LCDSpriteRefListPush(&layers, LayerSpriteConstructor(layer, image, false));
        LCDSpriteRefListPush(&layers, LayerSpriteConstructor(layer, image, true));
    }
    self->layers = layers;

    LayerWhiteGridConstructor();

    GUILivesConstructor(&self->lives);
    GUIScoreConstructor(&self->score);
    GUITimeConstructor(&self->time);
    GUIComboInit(&self->combo);

    if (self->showTransition) {
        loadSpriteGoPalette();
        TransitionOutConstructor(&spriteGo);
    }

    camera.frame.origin = MELPointZero;
    playdate->system->resetElapsedTime();
}

// TODO: Remplacer cette fonction par un booléen qui vaut false avant l'updateSprite et ajouter un setStillAlive avant les appels à MELSpriteDraw.
static MELBoolean anEnemyIsStillAlive(GameScene * _Nonnull self) {
    LCDSpriteRefList sprites = self->super.sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.type == MELSpriteTypeEnemy) {
            return true;
        }
    }
    return false;
}

static MELBoolean isTimeForNextCheckpoint(GameScene * _Nonnull self, float time) {
    const unsigned int nextCheckpointIndex = self->checkpointIndex;
    const MELBoolean currentWaveIsCheckpoint = self->wave->startTime != 0;
    const MELBoolean thereIsANextCheckpoint = nextCheckpointIndex < self->checkpoints.count;
    const float timeBeforeNext = !currentWaveIsCheckpoint && thereIsANextCheckpoint
        ? self->checkpoints.memory[nextCheckpointIndex]->startTime
        : kCheckpointDuration;
    const float elapsedTime = time - self->lastCheckpointClearTime;
    return elapsedTime >= timeBeforeNext;
}

static void setWave(GameScene * _Nonnull self, Wave * _Nonnull wave) {
    Wave *lastWave = self->waves.memory + self->waves.count;
    if (wave >= lastWave) {
        // Fin du jeu.
        return;
    }
    Wave *oldWave = self->wave;
    if (oldWave->startTime || oldWave->type == WaveTypeBoss) {
        // Sauvegarde du temps de clear du checkpoint.
        self->lastCheckpointClearTime = self->time;
    }
    const int checkpointIndex = self->checkpointIndex;
    const Wave *nextCheckpoint = self->checkpoints.memory[checkpointIndex];
    if (wave >= nextCheckpoint) {
        self->checkpointIndex = checkpointIndex + 1;
    }
    self->wave = wave;
    switch (wave->type) {
        case WaveTypeBoss:
            // TODO: trouver un meilleur test pour pouvoir gérer le true last boss
            showWarningSign(self, (lastWave - wave) == 1);
            break;
        case WaveTypeBackground:
            if (!self->cloud) {
                self->cloud = LCDBitmapLoadOrError("gui-cloud");
            }
            CloudTransitionConstructor(wave->mapName, self);
            break;
        default:
            WaveInstantiate(wave);
            break;
    }
    playMusic(self, oldWave->type);
}

static void resetCombo(GUICombo * _Nonnull self) {
    GUICombo combo = *self;
    combo.timer = 0.0f;
    combo.combo = 0;
    GUIComboAnimateDisappear(combo);
    *self = combo;
}

static void playMusic(GameScene * _Nonnull self, WaveType oldWaveType) {
    const struct playdate_sound_fileplayer *filePlayerAPI = playdate->sound->fileplayer;
    switch (self->wave->type) {
        case WaveTypeTutorial:
            filePlayerAPI->loadIntoPlayer(self->filePlayer, "audio/music-theme");
            filePlayerAPI->play(self->filePlayer, 0);
            filePlayerAPI->setLoopRange(self->filePlayer, 0.0f, 3.200136f);
            break;
        case WaveTypeNormal:
            if (oldWaveType == WaveTypeTutorial) {
                filePlayerAPI->setLoopRange(self->filePlayer, 0.0f, 0.0f);
            } else if (oldWaveType == WaveTypeOther) {
                filePlayerAPI->loadIntoPlayer(self->filePlayer, "audio/music-theme");
                filePlayerAPI->play(self->filePlayer, 0);
            }
            break;
        case WaveTypeBoss:
            if (oldWaveType == WaveTypeOther) {
                // Permet de jouer la musique du boss au chargement de GameScene en cas de reprise d'une partie.
                // Pendant le déroulement normal du jeu, le changement de musique est fait dans WarningSignScene.
                filePlayerAPI->loadIntoPlayer(self->filePlayer, "audio/music-boss");
                filePlayerAPI->play(self->filePlayer, 0);
            }
            break;
        default:
            // Aucun traitement.
            break;
    }
}

static void loadMusic(GameScene * _Nonnull self) {
    const struct playdate_sound_fileplayer *filePlayerAPI = playdate->sound->fileplayer;
    switch (self->wave->mapName) {
        case MapNameWave0Tutorial:
            filePlayerAPI->loadIntoPlayer(self->filePlayer, "audio/music-theme");
            filePlayerAPI->setLoopRange(self->filePlayer, 0.0f, 3.200136f);
            break;
        case MapNameWaveBoss:
            filePlayerAPI->loadIntoPlayer(self->filePlayer, "audio/music-boss");
            break;
        default:
            filePlayerAPI->loadIntoPlayer(self->filePlayer, "audio/music-theme");
            break;
    }
}

static void fadeOutMusic(GameScene * _Nonnull self) {
    const struct playdate_sound_fileplayer *filePlayerAPI = playdate->sound->fileplayer;
    filePlayerAPI->fadeVolume(self->filePlayer, 0, 0, 44000, NULL, NULL);
}

#pragma mark - Boucle de jeu

static int update(void * _Nonnull userdata) {
    GameScene *self = userdata;
    // NOTE: Gérer les collisions ici pour tous les sprites en une seule fois ? Ou plutôt des grilles de sprites pour réduire le nombre de tests de collision ?
    // TODO: Faire 2 grilles de sprites et mettre à jour la grille dans l'update de chaque sprite puis inverser les grilles après updateAndDrawSprites pour l'update suivant ? Simplement remettre à 0 le count des listes pour éviter de tout réallouer à chaque passage.

    DELTA = MELFloatMin(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();

    playdate->sprite->updateAndDrawSprites();
#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    camera.frame.origin.x += DELTA * CAMERA_SPEED;

    float time = self->time;
    Wave *wave = self->wave;
    const WaveType waveType = wave->type;
    if (waveType & (WaveTypeNormal | WaveTypeBoss)) {
        // TODO: Faire clignoter le compteur quand le temps est à zéro mais ne pas arrêter la partie. Charger la mauvaise fin à la place.
        self->time = time += DELTA;

        GUICombo combo = self->combo;
        if (combo.timer > 0.0f && waveType != WaveTypeBoss) {
            const float timer = combo.timer -= DELTA;
            ProgressBarSetProgress(combo.progressBar, timer / COMBO_DURATION);
            if (timer <= 0.0f) {
                resetCombo(&combo);
            }
            self->combo = combo;
        }
    }

    // Pas sûr, peut-être juste un soucis de durée pendant les checkpoints.
    const MELBoolean stillAlive = anEnemyIsStillAlive(self);
    if (!(waveType & (WaveTypeBoss | WaveTypeOther)) && stillAlive && isTimeForNextCheckpoint(self, time)) {
#if LOG_WAVE_CHANGES
        playdate->system->logToConsole("Time up! Go to next checkpoint.");
#endif
        LCDSpriteRefList sprites = self->super.sprites;
        for (unsigned int index = 0; index < sprites.count; index++) {
            LCDSprite *sprite = sprites.memory[index];
            MELSprite *melSprite = playdate->sprite->getUserdata(sprites.memory[index]);
            if (melSprite->class->withdraw) {
                melSprite->class->withdraw(sprite);
            }
        }
        const unsigned int checkpointIndex = self->checkpointIndex;
        Wave *nextWave;
        if (checkpointIndex < self->checkpoints.count) {
            nextWave = self->checkpoints.memory[checkpointIndex];
            self->checkpointIndex = checkpointIndex + 1;
        } else {
            nextWave = wave + 1;
        }
        setWave(self, nextWave);
    } else if (!(waveType & (WaveTypeBoss | WaveTypeOther)) && !stillAlive) {
#if LOG_WAVE_CHANGES
        playdate->system->logToConsole("Destroyed everything. Next wave.");
#endif
        setWave(self, wave + 1);
    }
#if DEBUG
    else if (MELControllerMake().pressedB) {
        GameSceneFadeToClearScreen();
    }
#endif

    return true;
}

#pragma mark - Gestion du warning

static const int kWarningSignLineWidth = 96;
static const int kWarningSignLineX = (LCD_COLUMNS - kWarningSignLineWidth) / 2;

static const int kWarningSignKanjiWidth = 64;
static const int kWarningSignKanjiX = (LCD_COLUMNS - kWarningSignKanjiWidth) / 2;

static const int kWarningSignBikkuriHeight = 54;
static const int kWarningSignKikenHeight = 96;
static const int kWarningSignChuuiHeight = 96;
static const int kWarningSignSpacing = 16;

static const int kWarningSignTotalHeight = kWarningSignBikkuriHeight + kWarningSignSpacing + kWarningSignKikenHeight + kWarningSignSpacing + kWarningSignBikkuriHeight + kWarningSignSpacing + kWarningSignChuuiHeight + kWarningSignSpacing;

static void destroyBullets(GameScene * _Nonnull self) {
    LCDSpriteRefList sprites = self->super.sprites;
    for (int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.type == MELSpriteTypeBullet) {
            playdate->sprite->setUpdateFunction(sprite, melSprite->class->destroy);
        }
    }
}

static void showWarningSign(GameScene * _Nonnull self, MELBoolean isLastBoss) {
    loadSpriteKikenPalette();
    loadSpriteChuuiPalette();
    loadSpriteBikkuriPalette();

    WarningSignData *warningSign = playdate->system->realloc(NULL, sizeof(WarningSignData));
    *warningSign = (WarningSignData) {
        .state = WarningSignStateDrawBlackBox,
        .time = 0.0f,
        .targetVolume = isLastBoss ? 0.0f : 0.5f,
        .kiken = playdate->graphics->getTableBitmap(spriteKiken.palette, 0),
        .chuui = playdate->graphics->getTableBitmap(spriteChuui.palette, 0),
        .bikkuri = playdate->graphics->getTableBitmap(spriteBikkuri.palette, 0),
    };
    self->warningSign = warningSign;
    playdate->system->setUpdateCallback(updateWarningSign, self);

    // Suppression des tirs avant d'afficher le warning
    destroyBullets(self);
}

static void drawWarningSign(WarningSignData * _Nonnull self, GameScene * _Nonnull gameScene) {
    const float time = self->time;
    switch (self->state) {
        case WarningSignStateDrawBlackBox: {
            const float duration = 0.5f;
            const float progress = MELEaseIn(0, duration, time);
            const int y = progress * LCD_ROWS;
            playdate->graphics->fillRect(kWarningSignLineX, 0, kWarningSignLineWidth, y, kColorBlack);

            const float volume = 1.0f + (self->targetVolume - 1.0f) * progress;
            playdate->sound->fileplayer->setVolume(gameScene->filePlayer, volume, volume);

            if (time >= duration) {
                self->time = 0.0f;
                self->state = WarningSignStateShowWarning;
                SamplePlay(SampleWarningBoss);
            }
        } break;
        case WarningSignStateShowWarning: {
            const float duration = 4.0f;
            const float progress = MELFloatMin(time / duration, 1.0f) * 2.0f;
            // TODO: Faire une seule image pour simplifier le code.
            playdate->graphics->fillRect(kWarningSignLineX, 0, kWarningSignLineWidth, LCD_ROWS, kColorBlack);
            playdate->graphics->setDrawMode(kDrawModeInverted);
            const int bikkuri1Y = -kWarningSignBikkuriHeight + kWarningSignTotalHeight * progress;
            playdate->graphics->drawBitmap(self->bikkuri, kWarningSignKanjiX, bikkuri1Y, kBitmapUnflipped);
            const int kikenY = -kWarningSignBikkuriHeight - kWarningSignSpacing - kWarningSignKikenHeight + kWarningSignTotalHeight * progress;
            playdate->graphics->drawBitmap(self->kiken, kWarningSignKanjiX, kikenY, kBitmapUnflipped);
            const int bikkuri2Y = -kWarningSignBikkuriHeight - kWarningSignSpacing - kWarningSignKikenHeight - kWarningSignSpacing - kWarningSignBikkuriHeight + kWarningSignTotalHeight * progress;
            playdate->graphics->drawBitmap(self->bikkuri, kWarningSignKanjiX, bikkuri2Y, kBitmapUnflipped);
            const int chuuiY = -kWarningSignBikkuriHeight - kWarningSignSpacing - kWarningSignKikenHeight - kWarningSignSpacing - kWarningSignBikkuriHeight - kWarningSignSpacing - kWarningSignChuuiHeight + kWarningSignTotalHeight * progress;
            playdate->graphics->drawBitmap(self->chuui, kWarningSignKanjiX, chuuiY, kBitmapUnflipped);
            playdate->graphics->setDrawMode(kDrawModeCopy);
            if (time >= duration) {
                self->time = 0.0f;
                self->state = WarningSignStateEraseBlackBox;

                if (self->targetVolume == 0.0f) {
                    const struct playdate_sound_fileplayer *filePlayerAPI = playdate->sound->fileplayer;
                    filePlayerAPI->stop(gameScene->filePlayer);
                    filePlayerAPI->loadIntoPlayer(gameScene->filePlayer, "audio/music-boss");
                    filePlayerAPI->play(gameScene->filePlayer, 0);
                }
            }
        } break;
        case WarningSignStateEraseBlackBox: {
            const float duration = 0.5f;
            const float progress = MELEaseOut(0, duration, time);
            const int y = progress * LCD_ROWS;
            playdate->graphics->fillRect(kWarningSignLineX, y, kWarningSignLineWidth, LCD_ROWS - y, kColorBlack);

            const float volume = self->targetVolume + (1.0f - self->targetVolume) * progress;
            playdate->sound->fileplayer->setVolume(gameScene->filePlayer, volume, volume);

            if (time >= duration) {
                self->time = 0.0f;
                self->state = WarningSignStateGoToGameScene;
            }
        } break;
        default:
            playdate->system->setUpdateCallback(update, gameScene);
            WaveInstantiate(gameScene->wave);
            gameScene->warningSign = NULL;
            playdate->system->realloc(self, 0);
            break;
    }
}

static int updateWarningSign(void * _Nonnull userdata) {
    GameScene *gameScene = userdata;
    WarningSignData *self = gameScene->warningSign;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();
#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    camera.frame.origin.x += DELTA * CAMERA_SPEED;

    self->time += DELTA;
    drawWarningSign(self, gameScene);
    return true;
}

#pragma mark - Gestion du GameOver

static void showGameOver(GameScene * _Nonnull self) {
    GameOverData *gameOver = playdate->system->realloc(NULL, sizeof(GameOverData));
    self->gameOver = gameOver;

    // Calcul du rang
    self->ranks.lives = RankF;
    self->ranks.timeLeft = RankF;
    self->ranks = (Ranks) {
        .lives = RankF,
        .timeLeft = RankF,
        .combo = RankF + MELIntMin(RankMaxValue * self->combo.maxCombo / RANK_CHOU_S_COMBO, RankMaxValue)
    };

    // Sauvegarde du meilleur rang pour le personnage.
    bestRanks[self->playerCharacter].combo = MELIntMax(bestRanks[self->playerCharacter].combo, self->ranks.combo);

    // Fondu et affichage du logo
    LCDSprite *fadeSprite = FadeConstructor();
    GameoverConstructorNoArgs();

    *gameOver = (GameOverData) {
        .fadeSprite = fadeSprite,
        .changedMusic = false,
        .time = 0.0f,
    };

    playdate->system->setUpdateCallback(updateGameOver, self);
}

static int updateGameOver(void * _Nonnull userdata) {
    GameScene *gameScene = userdata;
    GameOverData *self = gameScene->gameOver;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    const float duration = 1.0f;
    float time;
    self->time = time = MELFloatMin(self->time + DELTA, duration);
    const float progress = MELEaseInOut(0, duration, time);

    Fade *fade = playdate->sprite->getUserdata(self->fadeSprite);
    fade->opacity = progress * 20.0f;

    if (!self->changedMusic) {
        const float volumeChangeProgress = MELEaseInOut(0, 0.2f, time);
        const float volume = 1.0f - progress * 1.0f;
        const struct playdate_sound_fileplayer *filePlayerAPI = playdate->sound->fileplayer;
        filePlayerAPI->setVolume(gameScene->filePlayer, volume, volume);

        if (volumeChangeProgress == 1.0f) {
            filePlayerAPI->stop(gameScene->filePlayer);
            filePlayerAPI->loadIntoPlayer(gameScene->filePlayer, "audio/music-gameover");
            filePlayerAPI->play(gameScene->filePlayer, 1);
            self->changedMusic = true;
        }
    }

    PDButtons pressed;
    playdate->system->getButtonState(NULL, &pressed, NULL);
    if (progress == 1.0f && (pressed & (kButtonA | kButtonB))) {
        ScoreEntryScene *scoreEntryScene = ScoreEntryAlloc(gameScene->player->character, gameScene->score, gameScene->ranks);
        if (scoreEntryScene) {
            MELSceneMakeCurrent(&scoreEntryScene->super);
        } else {
            TitleScene *titleScene = TitleSceneAlloc();
            MELSceneMakeCurrent(&titleScene->super);
        }
        return true;
    }

    playdate->sprite->updateAndDrawSprites();

#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif
    return true;
}

#pragma mark - Affichage de "clear" et décompte des points bonus

static void createClearLabels(GameScene * _Nonnull self) {
    const float left = 26.0f;
    const float centerY = 82.0f;
    const float spacing = 32.0f;

    LCDSprite *livesLabel = strideTextHorizontal(self, "lives lost:", left, centerY - spacing, 0.0f, 1.0f);
    LCDSprite *timeLife = strideTextHorizontal(self, "time left:", left, centerY, 0.15f, 1.0f);
    LCDSprite *maxCombo = strideTextHorizontal(self, "max combo:", left, centerY + spacing, 0.3f, 1.0f);

    MELSprite *score = playdate->system->realloc(NULL, sizeof(MELSprite));
    LCDSprite *scoreSprite = ImageConstructorWithSelfDontPush(score, MELPointZero, LCDBitmapLoadOrError("gui-score"));
    LCDSpriteRefListPush(&self->others, scoreSprite);
    score->frame.origin = (MELPoint) {
        .x = -LCD_COLUMNS,
        .y = 156
    };
    StrideSpriteTo(scoreSprite, (MELPoint) {
        .x = left + SCORE_LABEL_WIDTH / 2.0f,
        .y = 156
    }, 0.45f, 1.0f);

    LCDSprite *scoreCounterSprite = CounterConstructor((MELPoint) {
        .x = -LCD_COLUMNS,
        .y = 188
    }, kCounterAlignmentLeft, numbers32Font, self->score);
    MELSprite *scoreCounter = playdate->sprite->getUserdata(scoreCounterSprite);
    StrideSpriteTo(scoreCounterSprite, (MELPoint) {
        .x = left + scoreCounter->frame.size.width / 2.0f,
        .y = 188
    }, 0.45f, 1.0f);

    playdate->sprite->setZIndex(livesLabel, ZINDEX_GUI);
    playdate->sprite->setZIndex(timeLife, ZINDEX_GUI);
    playdate->sprite->setZIndex(maxCombo, ZINDEX_GUI);
    playdate->sprite->setZIndex(scoreSprite, ZINDEX_GUI);
    playdate->sprite->setZIndex(scoreCounterSprite, ZINDEX_GUI);

    playdate->sprite->setTag(livesLabel, TagLivesLabel);
    playdate->sprite->setTag(timeLife, TagTimeLeft);
    playdate->sprite->setTag(maxCombo, TagMaxCombo);
    playdate->sprite->setTag(scoreCounterSprite, TagScoreCounter);
}

static ClearSprites findClearSprites(GameScene * _Nonnull self) {
    LCDSprite *livesLabel = NULL;
    LCDSprite *timeLeft = NULL;
    LCDSprite *maxCombo = NULL;
    LCDSprite *scoreCounter = NULL;

    LCDSpriteRefList sprites = self->others;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        switch (playdate->sprite->getTag(sprite)) {
        case TagTimeLeft:
            timeLeft = sprite;
            break;
        case TagMaxCombo:
            maxCombo = sprite;
            break;
        case TagScoreCounter:
            scoreCounter = sprite;
            break;
        case TagLivesLabel:
            livesLabel = sprite;
            break;
        default:
            break;
        }
    }
    return (ClearSprites) {
        .livesLabel = livesLabel,
        .timeLeft = timeLeft,
        .maxCombo = maxCombo,
        .scoreCounter = scoreCounter,
    };
}

static void showPressAToContinue(GameScene * _Nonnull self, const ClearSprites sprites) {
    const int timeLeft = MELIntMax(GAME_TOTAL_TIME_IN_SECONDS - self->time, 0);
    TextSetWithFormatAndInt(sprites.livesLabel, romajiFont, kDrawModeCopy, "lives lost: %d", self->deathCount);
    TextSetWithFormatAndInt(sprites.timeLeft, romajiFont, kDrawModeCopy, "time left: %d", timeLeft);
    TextSetWithFormatAndInt(sprites.maxCombo, romajiFont, kDrawModeCopy, "max combo: %d", self->combo.maxCombo);

    MELPoint origin = (MELPoint) {
        .x = LCD_COLUMNS - 92.0f,
        .y = LCD_ROWS - FONT_ROMAJI_HEIGHT / 2 - GUI_PADDING
    };
    LCDSprite *aToContinue = TextConstructor(origin, romajiFont, kDrawModeCopy, "\a continue", 10);
    MELSprite *melSprite = playdate->sprite->getUserdata(aToContinue);
    melSprite->frame.origin = origin;
    playdate->sprite->setZIndex(aToContinue, ZINDEX_GUI);
    playdate->sprite->moveTo(aToContinue, origin.x, origin.y);
    playdate->sprite->setUpdateFunction(aToContinue, MELSpriteNoopUpdate);

    playdate->system->setUpdateCallback(updatePressAToShowEnding, self);
}

static int updateWaitClearLabelStrideCompletion(void * _Nonnull userdata) {
    GameScene *self = userdata;
    playdate->sprite->updateAndDrawSprites();
    camera.frame.origin.x += DELTA * CAMERA_SPEED;

#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    LCDSprite *clear = NULL;
    LCDSpriteRefList sprites = self->others;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.name == SpriteNameClear) {
            clear = sprite;
        }
    }

    MELController controller = MELControllerMake();
    if (controller.pressedA || controller.pressedB) {
        StrideSkip(clear);
    }
    if (StrideIsDone(clear)) {
        createClearLabels(self);
        playdate->system->setUpdateCallback(updateWaitScoreCounterStrideCompletion, self);

        if (controller.pressedA || controller.pressedB) {
            // Le test d'appuie sur A ou B à l'intérieur de cette méthode va bypasser l'animation.
            updateWaitScoreCounterStrideCompletion(self);
        }
    }
    return true;
}

static int updateWaitScoreCounterStrideCompletion(void * _Nonnull userdata) {
    GameScene *self = userdata;
    playdate->sprite->updateAndDrawSprites();
    camera.frame.origin.x += DELTA * CAMERA_SPEED;

#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    const ClearSprites sprites = findClearSprites(self);

    MELController controller = MELControllerMake();
    if (controller.pressedA || controller.pressedB) {
        StrideSkip(sprites.livesLabel);
        StrideSkip(sprites.timeLeft);
        StrideSkip(sprites.maxCombo);
        StrideSkip(sprites.scoreCounter);
    }
    if (StrideIsDone(sprites.scoreCounter)) {
        self->checkpointIndex = TagInitial;
        self->combo.timer = 0.0f;
        CountUpdateAnchorWithCurrentOrigin(sprites.scoreCounter);
        playdate->system->setUpdateCallback(updateClearScreen, self);
    }
    return true;
}

static void computeRanks(GameScene * _Nonnull self) {
    const int timeLeft = MELIntMax(GAME_TOTAL_TIME_IN_SECONDS - self->time, 0);
    const Ranks ranks = (Ranks) {
        .lives = RankF + MELIntMax(RankMaxValue - self->deathCount, 0),
        .timeLeft = RankF + MELIntMin(RankMaxValue * timeLeft / RANK_CHOU_S_TIME_LEFT, RankMaxValue),
        .combo = RankF + MELIntMin(RankMaxValue * self->combo.maxCombo / RANK_CHOU_S_COMBO, RankMaxValue),
    };
    self->ranks = ranks;

    const PlayerCharacter character = self->playerCharacter;
    bestRanks[character].lives = MELIntMax(bestRanks[character].lives, ranks.lives);
    bestRanks[character].timeLeft = MELIntMax(bestRanks[character].timeLeft, ranks.timeLeft);
    bestRanks[character].combo = MELIntMax(bestRanks[character].combo, ranks.combo);
}

static void updateRank(LCDSprite * _Nonnull sprite) {
    MELSpriteDraw(playdate->sprite->getUserdata(sprite), sprite);
}

static void createRankLetter(int rankIndex, Rank rank, MELBoolean animate) {
    const MELPoint center = MELPointMake(230.0f, 50.0f + 32.0f * rankIndex);
    if (animate) {
        SamplePlay(rank >= RankS ? SampleOneUp : SampleRank);
        if (rank >= RankA) {
            loadSpriteWonderfulRankPalette();
            ExplosionConstructorWithDefinition(center, &spriteWonderfulRank);
        }
    }
    loadSpriteRankPalette();
    MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
    LCDSprite *sprite = MELSpriteInitWithCenter(melSprite, &spriteRank, center);
    playdate->sprite->setZIndex(sprite, ZINDEX_GUI);
    playdate->sprite->setUpdateFunction(sprite, updateRank);
    MELSpriteSetAnimation(melSprite, rank - RankF);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
}

static int updateClearScreen(void * _Nonnull userdata) {
    GameScene *self = userdata;
    playdate->sprite->updateAndDrawSprites();
    camera.frame.origin.x += DELTA * CAMERA_SPEED;

#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    const int lives = self->lives;
    const int timeLeft = MELIntMax(GAME_TOTAL_TIME_IN_SECONDS - self->time, 0);
    const int maxCombo = self->combo.maxCombo;
    const unsigned int score = self->score;

    const ClearSprites sprites = findClearSprites(self);

    // Décompte des points
    const float duration = 0.5f;
    // Calcul du temps en centisecondes pour le son "bip bip bip"
    const int oldTime = (int)(self->combo.timer * 10.0f);
    float time = self->combo.timer += DELTA;
    const int newTime = (int)(time * 10.0f);

    const float progress = MELEaseInOut(0.0f, duration, time);
    int value;
    LCDSprite *ranks[kRankCount] = {};
    switch (self->checkpointIndex) {
        case TagLives:
            value = self->deathCount * progress;
            TextSetWithFormatAndInt(sprites.livesLabel, romajiFont, kDrawModeCopy, "lives lost: %d", value);
            CounterSetValue(sprites.scoreCounter, score
                            + (lives * progress) * LIFE_LEFT_SCORE_BONUS);
            if (newTime != oldTime) {
                SamplePlay(SampleCounter);
            }
            break;
        case TagLivesRank:
            if (MELSceneFindSpritesByName(SpriteNameRank, ranks, kRankCount) == kRankLives) {
                createRankLetter(kRankLives, self->ranks.lives, true);
            }
            break;
        case TagTimeLeft:
            value = timeLeft * progress;
            TextSetWithFormatAndInt(sprites.timeLeft, romajiFont, kDrawModeCopy, "time left: %d", value);
            CounterSetValue(sprites.scoreCounter, score
                            + lives * LIFE_LEFT_SCORE_BONUS
                            + value * SECOND_LEFT_SCORE_BONUS);
            if (newTime != oldTime) {
                SamplePlay(SampleCounter);
            }
            break;
        case TagTimeLeftRank:
            if (MELSceneFindSpritesByName(SpriteNameRank, ranks, kRankCount) == kRankTimeLeft) {
                createRankLetter(kRankTimeLeft, self->ranks.timeLeft, true);
            }
            break;
        case TagMaxCombo:
            value = maxCombo * progress;
            TextSetWithFormatAndInt(sprites.maxCombo, romajiFont, kDrawModeCopy, "max combo: %d", value);
            CounterSetValue(sprites.scoreCounter, score
                            + lives * LIFE_LEFT_SCORE_BONUS
                            + timeLeft * SECOND_LEFT_SCORE_BONUS
                            + value * COMBO_SCORE_BONUS);
            if (newTime != oldTime) {
                SamplePlay(SampleCounter);
            }
            break;
        case TagMaxComboRank:
            if (MELSceneFindSpritesByName(SpriteNameRank, ranks, kRankCount) == kRankMaxCombo) {
                createRankLetter(kRankMaxCombo, self->ranks.combo, true);
            }
            break;
        default:
            // Pas d'action.
            break;
    }

    if (time >= duration) {
        self->checkpointIndex++;
        self->combo.timer = 0.0f;
    }

    MELController controller = MELControllerMake();
    if (self->checkpointIndex == TagScoreCounter || controller.pressedA || controller.pressedB) {
        self->checkpointIndex = TagClearScreenScoreUpdate;
        self->score += lives * LIFE_LEFT_SCORE_BONUS + timeLeft * SECOND_LEFT_SCORE_BONUS + maxCombo * COMBO_SCORE_BONUS;
#if ENABLE_ONLINE_SCOREBOARDS
        OnlineScoresAdd(self->playerCharacter, self->score);
#endif
        CounterSetValue(sprites.scoreCounter, self->score);

        showPressAToContinue(self, sprites);
    }

    return true;
}

static int updatePressAToShowEnding(void * _Nonnull userdata) {
    playdate->sprite->updateAndDrawSprites();
    camera.frame.origin.x += DELTA * CAMERA_SPEED;

#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    MELController controller = MELControllerMake();
    if (controller.pressedA || controller.pressedB) {
        updateGoToEnding(userdata);
    }
    return true;
}

static LCDSprite * _Nonnull strideTextHorizontal(GameScene * _Nonnull self, const char * _Nonnull text, float left, float centerY, float delay, float duration) {
    MELPoint origin = (MELPoint) {
        .x = -LCD_COLUMNS,
        .y = centerY
    };
    LCDSprite *sprite = TextConstructorDontPush(origin, romajiFont, kDrawModeCopy, text, 99);
    LCDSpriteRefListPush(&self->others, sprite);
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    StrideSpriteTo(sprite, (MELPoint) { .x = left + melSprite->frame.size.width / 2.0f, .y = centerY }, delay, duration);
    return sprite;
}

#pragma mark - Affichage de la fin

static int updateGoToEnding(void * _Nonnull userdata) {
    GameScene *self = userdata;
    StoryScene *scene = self->time < GAME_TOTAL_TIME_IN_SECONDS && self->lives > 0
        ? Ending2SceneSceneAlloc(self->playerCharacter, self->score, self->ranks)
        : Ending1SceneSceneAlloc(self->playerCharacter, self->score, self->ranks);
    MELSceneMakeCurrent(&scene->super);
    return false;
}

#pragma mark - Pause

static int updatePause(void * _Nullable userdata) {
    GameScene *self = userdata;
    playdate->sprite->drawSprites();

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    if (self->warningSign) {
        drawWarningSign(self->warningSign, self);
    }

    // NOTE: Pas utile car les sons sont déjà chargés dans GameSceneAlloc
    SampleLoadNextSample();

    const uint32_t timeInMillis = playdate->system->getCurrentTimeMilliseconds();
    const MELBoolean isVisible = (timeInMillis / 500) % 2;
    if (isVisible) {
        const int width = playdate->graphics->getTextWidth(romajiFont, "pause", 5, kASCIIEncoding, 0);
        playdate->graphics->setDrawMode(kDrawModeCopy);
        playdate->graphics->setFont(romajiFont);
        playdate->graphics->drawText("pause", 5, kASCIIEncoding, (LCD_COLUMNS - width) / 2, LCD_ROWS / 2);
    }

    MELController controller = MELControllerMake();
    if (controller.pressedA || controller.pressedB) {
        playdate->sound->fileplayer->play(self->filePlayer, 0);
        playdate->sound->fileplayer->setOffset(self->filePlayer, self->filePlayerOffset);
        playdate->system->setUpdateCallback(self->warningSign
            ? updateWarningSign
            : update, self);
    }
    return true;
}
