//
//  gamescene.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 24/01/2023.
//

#ifndef gamescene_h
#define gamescene_h

#include "common.h"

#include "player.h"
#include "wave.h"
#include "gui_combo.h"
#include "rankingboard.h"

typedef struct {
    LCDSprite * _Nullable fadeSprite;
    float time;
    MELBoolean changedMusic;
} GameOverData;

typedef enum {
    WarningSignStateDrawBlackBox,
    WarningSignStateShowWarning,
    WarningSignStateEraseBlackBox,
    WarningSignStateGoToGameScene,
} WarningSignState;

typedef struct {
    WarningSignState state;
    float time;
    float targetVolume;
    // NOTE: Les images proviennent des tables attachées aux définitions et ne sont pas libérées.
    LCDBitmap * _Nonnull kiken;
    LCDBitmap * _Nonnull chuui;
    LCDBitmap * _Nonnull bikkuri;
} WarningSignData;

typedef struct {
    MELScene super;
    FilePlayer * _Nullable filePlayer;
    WarningSignData * _Nullable warningSign;
    GameOverData * _Nullable gameOver;
    // TODO: Ajouter 2 listes (enemies et throwns) pour optimiser les collisions ? Ou faire une grille ?
    PlayerCharacter playerCharacter;
    LCDSprite * _Nullable playerSprite;
    Player * _Nullable player;
    MELMap * _Nullable background;
    MapName backgroundName;
    LCDSpriteRefList layers;
    LCDSpriteRefList crystals;
    LCDSpriteRefList others;
    LCDBitmap * _Nullable cloud;
    GUICombo combo;
    unsigned int score;
    unsigned int nextOneUp;
    int rank;
    Ranks ranks;
    int8_t lives;
    int8_t deathCount;
    float time;
    float lastCheckpointClearTime;
    WaveList waves;
    Wave * _Nullable wave;
    WaveRefList checkpoints;
    unsigned int checkpointIndex;
    float filePlayerOffset;
    MELBoolean showTransition;
    MELBoolean isLoadingSave;
} GameScene;

GameScene * _Nonnull GameSceneAlloc(PlayerCharacter playerCharacter);
GameScene * _Nonnull GameSceneAllocWithTransition(PlayerCharacter playerCharacter);
GameScene * _Nullable GameSceneGetCurrent(void);

void GameSceneRestoreState(GameScene * _Nonnull self);

/**
 * Détruit tous les tirs présents à l'écran.
 */
void GameSceneDestroyBullets(void);
void GameSceneScoreAdd(int value, int combo);
void GameSceneComboIncrement(void);
void GameSceneComboFillTimer(void);
void GameSceneLifeSubstractOne(void);
void GameSceneRestartAfterDeath(void);
void GameSceneNextWave(void);
void GameSceneNextWaveWithGameScene(GameScene * _Nonnull self);
void GameSceneFadeToClearScreen(void);

void MELSpriteDeallocFromGameSceneOthers(LCDSprite * _Nonnull sprite);

#endif /* gamescene_h */
