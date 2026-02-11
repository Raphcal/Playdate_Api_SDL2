//
//  savegame.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/11/2023.
//

#include "savegame.h"

#include "gamescene.h"
#include "scoreentryscene.h"
#include "ending1scene.h"
#include "ending2scene.h"
#include "camera.h"

#include "../gen/spritekiken.h"
#include "../gen/spritechuui.h"
#include "../gen/spritebikkuri.h"

/**
 * Version initiale.
 * @since 25/11/2023
 */
#define kVersion1 1
/**
 * Ajout des rangs.
 * @since 17/01/2025
 */
static const uint16_t kVersion2 = 2;
/**
 * Ajout du nombre de morts.
 * @since 22/01/2025
 */
static const uint16_t kVersion3 = 3;
static const uint16_t kLatestVersion = kVersion3;

static const char kFileName[] = "save-state.data";

#define kMagicLength 8
static const char kMagic[] = "KUROSAVE";

static const char * _Nonnull kSalt = "byRCA";
static const int kSaltLength = 5;

static void GameSceneLoadState(MELInputStream * _Nonnull inputStream, int version);
static void ScoreEntrySceneLoadState(MELInputStream * _Nonnull inputStream, int version);
static void StorySceneLoadState(MELInputStream * _Nonnull inputStream, int version);

void SaveState(void) {
    if (!currentScene || !currentScene->save) {
        return;
    }
    MELOutputStream outputStream = MELOutputStreamOpen(kFileName);
    MELOutputStreamWrite(&outputStream, kMagic, kMagicLength * sizeof(char));
    MELOutputStreamWriteUInt16(&outputStream, kLatestVersion);

    MELOutputStreamStartHash(&outputStream, kSalt, kSaltLength);

    MELOutputStreamWriteByte(&outputStream, currentScene->type);

    currentScene->save(currentScene, &outputStream);

    MELOutputStreamEndHash(&outputStream);

    MELOutputStreamClose(&outputStream);
    playdate->system->logToConsole("State saved.");
}

MELBoolean LoadState(void) {
    MELInputStream inputStream = MELInputStreamOpen(kFileName, kFileReadData);
    if (!inputStream.file) {
        MELInputStreamClose(&inputStream);
        return false;
    }

    jmp_buf exceptionHandler = {};
    if (setjmp(exceptionHandler)) {
        if (currentScene != NULL) {
            playdate->sprite->removeAllSprites();
            currentScene->dealloc(currentScene);
            currentScene = NULL;
        }
        MELInputStreamClose(&inputStream);
        playdate->file->unlink(kFileName, false);
        return false;
    }
    inputStream.exceptionHandler = &exceptionHandler;

    char magic[kMagicLength + 1] = {};
    MELInputStreamRead(&inputStream, magic, kMagicLength);
    if (strncmp(magic, kMagic, kMagicLength)) {
        playdate->system->logToConsole("Bad file header: %s", magic);
        MELInputStreamClose(&inputStream);
        playdate->file->unlink(kFileName, false);
        return false;
    }
    uint16_t version = MELInputStreamReadUInt16(&inputStream);
    if (version > kLatestVersion) {
        playdate->system->logToConsole("Latest supported version is %d, but file version is: %d", kLatestVersion, version);
        MELInputStreamClose(&inputStream);
        playdate->file->unlink(kFileName, false);
        return false;
    }

    MELInputStreamStartHash(&inputStream, kSalt, kSaltLength);

    const SceneType sceneType = MELInputStreamReadByte(&inputStream);
    switch (sceneType) {
        case SceneTypeGame:
            GameSceneLoadState(&inputStream, version);
            break;
        case SceneTypeScoreEntry:
            ScoreEntrySceneLoadState(&inputStream, version);
            break;
        case SceneTypeStory:
            StorySceneLoadState(&inputStream, version);
            break;
        default:
            playdate->system->logToConsole("Unable to load state, unsupported scene type: %d", sceneType);
            MELInputStreamClose(&inputStream);
            playdate->file->unlink(kFileName, false);
            return false;
    }

    if (currentScene == NULL) {
        MELInputStreamClose(&inputStream);
        playdate->file->unlink(kFileName, false);
        return false;
    }

    if (!MELInputStreamEndAndValidateHash(&inputStream)) {
        playdate->system->logToConsole("Unable to load game, hash does not match.");
        MELInputStreamClose(&inputStream);
        playdate->file->unlink(kFileName, false);

        playdate->sprite->removeAllSprites();
        currentScene->dealloc(currentScene);
        currentScene = NULL;
        return false;
    }
    MELInputStreamClose(&inputStream);
    playdate->file->unlink(kFileName, false);

    MELSceneAddOrRemoveBackToTitleMenuItem();
    return true;
}

#pragma mark - GameScene

void GameSceneSaveState(MELScene * _Nonnull scene, MELOutputStream * _Nonnull outputStream) {
    GameScene *self = (GameScene *)scene;

    MELOutputStreamWriteBoolean(outputStream, self->gameOver != NULL);

    // Personnage
    MELOutputStreamWriteByte(outputStream, self->playerCharacter);
    // Score
    MELOutputStreamWriteUInt32(outputStream, self->score);
    if (self->gameOver) {
        // Pas la peine de restaurer l'écran "Game Over", affichage du high score seulement si nécessaire.
        return;
    }
    MELOutputStreamWriteUInt32(outputStream, self->nextOneUp);

    // Rangs (version 2)
    MELOutputStreamWriteUInt8(outputStream, self->ranks.lives);
    MELOutputStreamWriteUInt8(outputStream, self->ranks.timeLeft);
    MELOutputStreamWriteUInt8(outputStream, self->ranks.combo);

    // Vies
    MELOutputStreamWriteByte(outputStream, self->lives);

    // Nombre de morts (Version 3)
    MELOutputStreamWriteInt8(outputStream, self->deathCount);

    // Temps
    MELOutputStreamWriteFloat(outputStream, self->time);
    MELOutputStreamWriteFloat(outputStream, self->lastCheckpointClearTime);

    // Combo
    MELOutputStreamWriteUInt16(outputStream, self->combo.combo);
    MELOutputStreamWriteUInt16(outputStream, self->combo.maxCombo);
    MELOutputStreamWriteFloat(outputStream, self->combo.timer);

    // Musique
    if (self->filePlayer) {
        // TODO: Sauvegarder le morceau ?
        const float offset = playdate->sound->fileplayer->getOffset(self->filePlayer);
        MELOutputStreamWriteFloat(outputStream, offset);
    } else {
        MELOutputStreamWriteFloat(outputStream, 0.0f);
    }

    // Caméra
    MELOutputStreamWritePoint(outputStream, camera.frame.origin);

    // Vague
    const int waveIndex = (int) (self->wave - self->waves.memory);
    MELOutputStreamWriteInt(outputStream, waveIndex);
    MELOutputStreamWriteUInt32(outputStream, self->checkpointIndex);

    // Fond
    MELOutputStreamWriteByte(outputStream, self->backgroundName);

    // Sprites
    LCDSprite *playerSprite = self->playerSprite;
    LCDSpriteRefList sprites = self->super.sprites;
#if LOG_SAVES
    playdate->system->logToConsole("Player index: %d", LCDSpriteRefListIndexOf(sprites, playerSprite));
    playdate->system->logToConsole("%d sprites to save", sprites.count);
#endif
    MELOutputStreamWriteUInt32(outputStream, sprites.count);
    // Sauvegarde toujours le joueur en premier pour éviter les problèmes de dépendances au chargement.
    MELSpriteSave(playerSprite, outputStream);
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
#if LOG_SAVES
        const int32_t cursor = outputStream->count;
        playdate->system->logToConsole("Saving sprite %d: %x, is player: %s", index, sprite, sprite == playerSprite ? "true" : "false");
#endif
        if (sprite != playerSprite) {
            MELSpriteSave(sprites.memory[index], outputStream);
        }
#if LOG_SAVES
        const int32_t total = outputStream->count < cursor
            ? 4096 + outputStream->count
            : outputStream->count - cursor;
        playdate->system->logToConsole("  - %d byte(s) written", total);
#endif
    }

    // Cristaux
    LCDSpriteRefList crystals = self->crystals;
    MELOutputStreamWriteUInt32(outputStream, crystals.count);
    for (unsigned int index = 0; index < crystals.count; index++) {
        MELSpriteSave(crystals.memory[index], outputStream);
    }

    // Warning Sign
    MELOutputStreamWriteBoolean(outputStream, self->warningSign != NULL);
    if (self->warningSign) {
        WarningSignData *warningSign = self->warningSign;
        MELOutputStreamWriteByte(outputStream, warningSign->state);
        MELOutputStreamWriteFloat(outputStream, warningSign->time);
        MELOutputStreamWriteFloat(outputStream, warningSign->targetVolume);
    }
}

static void GameSceneLoadState(MELInputStream * _Nonnull inputStream, int version) {
    const MELBoolean isGameOver = MELInputStreamReadBoolean(inputStream);
    if (isGameOver) {
        ScoreEntrySceneLoadState(inputStream, version);
        return;
    }

    // Personnage
    const PlayerCharacter character = MELInputStreamReadByte(inputStream);

    GameScene *self = GameSceneAlloc(character);
    currentScene = &self->super;

    // Score
    self->score = MELInputStreamReadUInt32(inputStream);
    self->nextOneUp = MELInputStreamReadUInt32(inputStream);

    // Rangs
    Ranks ranks = (Ranks) {};
    if (version >= kVersion2) {
        ranks.lives = MELInputStreamReadUInt8(inputStream);
        ranks.timeLeft = MELInputStreamReadUInt8(inputStream);
        ranks.combo = MELInputStreamReadUInt8(inputStream);
    }

    // Vies
    self->lives = MELInputStreamReadByte(inputStream);

    if (version >= kVersion3) {
        self->deathCount = MELInputStreamReadInt8(inputStream);
    }

    // Temps
    self->time = MELInputStreamReadFloat(inputStream);
    self->lastCheckpointClearTime = MELInputStreamReadFloat(inputStream);

    // Combo
    self->combo.combo = MELInputStreamReadUInt16(inputStream);
    self->combo.maxCombo = MELInputStreamReadUInt16(inputStream);
    self->combo.timer = MELInputStreamReadFloat(inputStream);

    // Musique
    self->filePlayerOffset = MELInputStreamReadFloat(inputStream);

    // Caméra
    const MELPoint cameraOrigin = MELInputStreamReadPoint(inputStream);

    // Vague
    WaveList waves = self->waves;
    const int waveIndex = MELInputStreamReadInt(inputStream);
    self->wave = waves.memory + waveIndex;
    self->checkpointIndex = MELInputStreamReadUInt32(inputStream);

    // Fond
    const MapName backgroundName = MELInputStreamReadByte(inputStream);
    if (backgroundName != self->backgroundName) {
        if (self->background) {
            MELMapDealloc(self->background);
        }
        self->background = MELMapOpen(kMapNameFileNames[backgroundName]);
        self->backgroundName = backgroundName;
    }

    // Initialisation pour permettre de charger les décors et les instances.
    self->isLoadingSave = true;
    self->super.init(&self->super);
    camera.frame.origin = cameraOrigin;

    // Sprites
    const unsigned int spriteCount = MELInputStreamReadUInt32(inputStream);
#if LOG_SAVES
    playdate->system->logToConsole("%d sprites to load", spriteCount);
#endif
    LCDSpriteRefListEnsureCapacity(&self->super.sprites, self->super.sprites.count + spriteCount);
    for (unsigned int index = 0; index < spriteCount; index++) {
#if LOG_SAVES
        const int32_t cursor = inputStream->cursor;
        playdate->system->logToConsole("Loading sprite %d", index);
#endif
        LCDSprite *sprite = MELSpriteLoad(inputStream);
        if (sprite) {
#if LOG_SAVES
            playdate->system->logToConsole("  - sprite %x, is player: %s", sprite, sprite == self->playerSprite ? "true" : "false");
#endif
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
            MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
            playdate->system->logToConsole("Push GameSceneLoadState(%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
            LCDSpriteRefListPush(&self->super.sprites, sprite);
        } else {
            playdate->system->logToConsole("  - Unable to load sprite %d", index);
        }
#if LOG_SAVES
        const int32_t total = inputStream->cursor < cursor
            ? 4096 + inputStream->cursor
            : inputStream->cursor - cursor;
        playdate->system->logToConsole("  - %d byte(s) read", total);
#endif
    }
#if LOG_SAVES
    playdate->system->logToConsole("total: %d sprite(s) loaded", currentScene->sprites.count);
    playdate->system->logToConsole("Player index: %d", LCDSpriteRefListIndexOf(self->super.sprites, self->playerSprite));
#endif

    // Bonus (cristaux)
    const unsigned int crystalCount = MELInputStreamReadUInt32(inputStream);
    LCDSpriteRefListEnsureCapacity(&self->crystals, crystalCount);
    LCDSpriteRefList crystals = self->crystals;
    for (unsigned int index = 0; index < crystalCount; index++) {
        LCDSprite *sprite = MELSpriteLoad(inputStream);
        LCDSpriteRefListPush(&crystals, sprite);
    }
    self->crystals = crystals;

    // Warning sign
    const MELBoolean hasWarningSign = MELInputStreamReadBoolean(inputStream);
    if (hasWarningSign) {
        loadSpriteKikenPalette();
        loadSpriteChuuiPalette();
        loadSpriteBikkuriPalette();
        const WarningSignState state = MELInputStreamReadByte(inputStream);
        const float time = MELInputStreamReadFloat(inputStream);
        const float targetVolume = MELInputStreamReadFloat(inputStream);
        WarningSignData *warningSign = playdate->system->realloc(NULL, sizeof(WarningSignData));
        *warningSign = (WarningSignData) {
            .state = state,
            .time = time,
            .targetVolume = targetVolume,
            .kiken = playdate->graphics->getTableBitmap(spriteKiken.palette, 0),
            .chuui = playdate->graphics->getTableBitmap(spriteChuui.palette, 0),
            .bikkuri = playdate->graphics->getTableBitmap(spriteBikkuri.palette, 0),
        };
        self->warningSign = warningSign;
    }

    // Configure la méthode update.
    GameSceneRestoreState(self);
}

#pragma mark - ScoreEntryScene

void ScoreEntrySceneSaveState(MELScene * _Nonnull scene, MELOutputStream * _Nonnull outputStream) {
    ScoreEntryScene *self = (ScoreEntryScene *)scene;

    Score score = self->score;
    MELOutputStreamWriteByte(outputStream, score.character);
    MELOutputStreamWriteUInt32(outputStream, score.super.value);

    // Version 2
    MELOutputStreamWriteUInt8(outputStream, score.ranks.lives);
    MELOutputStreamWriteUInt8(outputStream, score.ranks.timeLeft);
    MELOutputStreamWriteUInt8(outputStream, score.ranks.combo);
}

static void ScoreEntrySceneLoadState(MELInputStream * _Nonnull inputStream, int version) {
    const PlayerCharacter character = MELInputStreamReadByte(inputStream);
    const uint32_t score = MELInputStreamReadUInt32(inputStream);

    Ranks ranks = (Ranks) {};
    if (version >= kVersion2) {
        ranks.lives = MELInputStreamReadUInt8(inputStream);
        ranks.timeLeft = MELInputStreamReadUInt8(inputStream);
        ranks.combo = MELInputStreamReadUInt8(inputStream);
    }

    ScoreEntryScene *self = ScoreEntryAlloc(character, score, ranks);
    if (self != NULL) {
        currentScene = &self->super;
        playdate->system->setUpdateCallback(self->super.update, self);
        self->super.init(&self->super);
    }
}

#pragma mark - StoryScene

void StorySceneSaveState(MELScene * _Nonnull scene, MELOutputStream * _Nonnull outputStream) {
    StoryScene *self = (StoryScene *)scene;

    MELOutputStreamWriteByte(outputStream, self->mapName);
    MELOutputStreamWriteByte(outputStream, self->character);
    MELOutputStreamWriteUInt32(outputStream, self->score);

    // Version 2
    MELOutputStreamWriteUInt8(outputStream, self->ranks.lives);
    MELOutputStreamWriteUInt8(outputStream, self->ranks.timeLeft);
    MELOutputStreamWriteUInt8(outputStream, self->ranks.combo);

    MELOutputStreamWriteByte(outputStream, self->link);
    MELOutputStreamWritePoint(outputStream, camera.frame.origin);
}

static void StorySceneLoadState(MELInputStream * _Nonnull inputStream, int version) {
    StoryScene *self;

    const MapName mapName = (MapName) MELInputStreamReadByte(inputStream);
    const PlayerCharacter character = (PlayerCharacter) MELInputStreamReadByte(inputStream);
    const unsigned int score = MELInputStreamReadUInt32(inputStream);

    Ranks ranks = (Ranks) {};
    if (version >= kVersion2) {
        ranks.lives = MELInputStreamReadUInt8(inputStream);
        ranks.timeLeft = MELInputStreamReadUInt8(inputStream);
        ranks.combo = MELInputStreamReadUInt8(inputStream);
    }

    switch (mapName) {
        case MapNameEnding1:
            self = Ending1SceneSceneAlloc(character, score, ranks);
            break;
        case MapNameEnding2:
            self = Ending2SceneSceneAlloc(character, score, ranks);
            break;
        default:
            playdate->system->logToConsole("Unsupported StoryScene map: %d", mapName);
            return;
    }
    currentScene = &self->super;
    self->super.init(&self->super);
    self->link = MELInputStreamReadByte(inputStream);
    camera.frame.origin = MELInputStreamReadPoint(inputStream);
    playdate->system->setUpdateCallback(self->super.update, self);
}
