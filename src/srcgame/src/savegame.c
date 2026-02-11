//
//  savegame.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/11/2023.
//

#include "savegame.h"

#include "gamescene.h"
#include "carddeck.h"
#include "../lib/camera.h"

/**
 * Version initiale.
 * @since 23/01/2024
 */
// NOTE: Il s'agit d'une #define pour éviter un warning "variable inutilisée".
#define kVersion1 1
/**
 * Sauvegarde du temps de jeu et du nombre de victoires/défaites.
 * @since 24/05/2024
 */
static const uint16_t kVersion2 = 2;
/**
 * Sauvegarde de la carte représentant le joueur.
 * @since 25/06/2024
 */
static const uint16_t kVersion3 = 3;
/**
 * Sauvegarde de stats sur le nombre de cartes/jetons supprimées, nombre de mains, meilleur combo.
 * @since 06/07/2024
 */
static const uint16_t kVersion4 = 4;
/**
 * Ajout de l'histoire de Mélusine.
 */
static const uint16_t kVersion5 = 5;
/**
 * Ajout d'un UUID pour identifier les utilisateurs locaux dans le tableau des hiscores
 * et sauvegarde de quelques personal bests.
 * @since 19/11/2024
 */
static const uint16_t kVersion6 = 6;

/**
 * Stockage de 20 scores pour le mode score attack.
 * @since 27/11/2024
 */
static const uint16_t kVersion7 = 7;

/**
 * Sauvegarde de la date de fin d'une histoire plutôt qu'un simple booléen.
 * @since 05/06/2025
 */
static const uint16_t kVersion8 = 8;

/**
 * Sauvegarde de la date de déblocage des succès.
 * @since 01/07/2025
 */
static const uint16_t kVersion9 = 9;

/**
 * Ajout des données du mode rally/arcade.
 * @since 22/07/2025
 */
static const uint16_t kVersion10 = 10;

/**
 * Ajout de succès pour le mode arcade.
 * @since 06/08/2025
 */
static const uint16_t kVersion11 = 11;

/**
 * Sauvegarde de l'état en mode arcade.
 * @since 09/08/2025
 */
static const uint16_t kVersion12 = 12;

/**
 * Sauvegarde des nouveaux champs dans ArcadeGame (bestWinStreak et continueCount).
 * @since 02/09/2025
 */
static const uint16_t kVersion13 = 13;

static const uint16_t kLatestVersion = kVersion13;

#define kMagicLength 6
static const char kMagic[] = "PoPoMa";

static const char * _Nonnull kSalt = "d13e533f-5e5a-4505-80d6-2b15b4df11a5";
static const int kSaltLength = 36;

static const int kVersion6ScoreAttackHistoryCount = 10;

SaveGame currentSaveGame = (SaveGame) {
    .index = kNoSaveSelected,
};

MELListImplement(SaveGame);

void SaveGameCreate(SaveGameList * _Nonnull saveGames) {
    CardDeck deck = (CardDeck) {};
    for (unsigned int index = 0; index < saveGames->count; index++) {
        CardDeckPickCard(&deck, saveGames->memory[index].card);
    }
    const int selection = saveGames->count;
    SaveGameListPush(saveGames, (SaveGame) {
        .uuid = MELUUIDMakeV7(),
        .index = selection,
        .language = currentLanguage,
        .card = CardDeckPickRandomCard(&deck),
    });
    SaveGameToDisk(saveGames->memory + selection);
    CardDeckDeinit(&deck);
}

void SaveGameToDisk(SaveGame * _Nonnull self) {
    char path[kFileNameLength] = "save0.data";
    path[kFileNameNumberIndex] = '0' + self->index;
    MELOutputStream outputStream = MELOutputStreamOpen(path);

    MELOutputStreamWrite(&outputStream, kMagic, kMagicLength * sizeof(char));
    MELOutputStreamWriteUInt16(&outputStream, kLatestVersion);

    MELOutputStreamStartHash(&outputStream, kSalt, kSaltLength);

    // Version 6
    MELOutputStreamWriteUUID(&outputStream, self->uuid);

    MELOutputStreamWrite(&outputStream, self->name, kSaveGameNameLength * sizeof(char));

    // Version 3
    MELOutputStreamWriteByte(&outputStream, self->card.rank);
    MELOutputStreamWriteByte(&outputStream, self->card.suit);

    MELOutputStreamWriteByte(&outputStream, currentLanguage);
    MELOutputStreamWriteByte(&outputStream, self->lastCharacter);
    for (unsigned int story = 0; story < kStoryCount; story++) {
        MELOutputStreamWriteUInt32(&outputStream, self->completion[story]);
    }
    for (unsigned int story = 0; story < kStoryCount; story++) {
        MELOutputStreamWriteInt8(&outputStream, self->progression[story]);
    }
    MELOutputStreamWriteUInt32(&outputStream, self->schoolProgression);

    // Version 2
    MELOutputStreamWriteFloat(&outputStream, self->playtime);
    MELOutputStreamWriteUInt16(&outputStream, self->wins);
    MELOutputStreamWriteUInt16(&outputStream, self->loses);

    // Version 4
    MELOutputStreamWriteUInt32(&outputStream, self->removedCards);
    MELOutputStreamWriteUInt32(&outputStream, self->removedChips);
    MELOutputStreamWriteUInt32(&outputStream, self->sentChips);
    MELOutputStreamWriteUInt32(&outputStream, self->handCount[0]);
    MELOutputStreamWriteUInt32(&outputStream, self->handCount[1]);
    MELOutputStreamWriteUInt32(&outputStream, self->handCount[2]);
    MELOutputStreamWriteByte(&outputStream, self->bestCombo);

    // Version 6
    MELOutputStreamWriteUInt16(&outputStream, self->scoreAttackRun);
    MELOutputStreamWriteUInt32(&outputStream, self->scoreAttackPersonalBest);
    MELOutputStreamWrite(&outputStream, self->scoreAttackLastScores, kScoreAttackHistoryCount * sizeof(uint32_t));

    // Version 9
    for (unsigned int index = 0; index < AchievementCount; index++) {
        MELOutputStreamWriteUInt32(&outputStream, self->achievementUnlockDate[index]);
    }

    // Version 10
    MELOutputStreamWriteUInt16(&outputStream, self->rallyRun);
    MELOutputStreamWriteUInt8(&outputStream, self->rallyBestStage);
    MELOutputStreamWriteUInt32(&outputStream, self->rallyPersonalBest);
    MELOutputStreamWrite(&outputStream, self->rallyLastScores, kScoreAttackHistoryCount * sizeof(uint32_t));

    // Version 12
    const ArcadeSaveState saveState = self->arcadeSaveState;
    MELOutputStreamWriteUInt32(&outputStream, saveState.game.score);
    MELOutputStreamWriteUInt32(&outputStream, saveState.game.deathCount);
    MELOutputStreamWriteFloat(&outputStream, saveState.game.speed);
    MELOutputStreamWriteUInt8(&outputStream, saveState.game.stage);
    MELOutputStreamWriteUInt8(&outputStream, saveState.game.winStreak);
    MELOutputStreamWriteUInt8(&outputStream, saveState.game.loseStreak);

    // Version 13
    MELOutputStreamWriteFloat(&outputStream, saveState.game.time);
    MELOutputStreamWriteUInt8(&outputStream, saveState.game.bestWinStreak);
    MELOutputStreamWriteUInt32(&outputStream, saveState.game.continueCount);

    // Version 12
    const MELBoolean hasBoard = saveState.leftBoard != NULL;
    MELOutputStreamWriteBoolean(&outputStream, hasBoard);
    if (hasBoard) {
        MELOutputStreamWriteUTF8String(&outputStream, saveState.leftBoard);
        MELOutputStreamWriteUTF8String(&outputStream, saveState.rightBoard);
        MELOutputStreamWriteUTF8String(&outputStream, saveState.deck);
        MELOutputStreamWriteUInt32(&outputStream, saveState.leftDeckIndex);
        MELOutputStreamWriteUInt32(&outputStream, saveState.rightDeckIndex);
    }

    MELOutputStreamEndHash(&outputStream);

    MELOutputStreamClose(&outputStream);
    playdate->system->logToConsole("Game %d has been saved.", self->index);
}

static MELBoolean ReadSavedGame(MELInputStream * _Nonnull inputStream, SaveGame * _Nonnull out) {
    SaveGame self = (SaveGame) {};

    jmp_buf exceptionHandler = {};
    if (setjmp(exceptionHandler)) {
        // Exception
        return false;
    }
    inputStream->exceptionHandler = &exceptionHandler;

    char magic[kMagicLength + 1] = {};
    MELInputStreamRead(inputStream, magic, kMagicLength);
    if (strncmp(magic, kMagic, kMagicLength)) {
        playdate->system->logToConsole("Bad file header: %s", magic);
        return false;
    }
    uint16_t version = MELInputStreamReadUInt16(inputStream);
    if (version > kLatestVersion) {
        playdate->system->logToConsole("Latest supported version is %d, but file version is: %d", kLatestVersion, version);
        return false;
    }

    MELInputStreamStartHash(inputStream, kSalt, kSaltLength);

    if (version >= kVersion6) {
        self.uuid = MELInputStreamReadUUID(inputStream);
    } else {
        self.uuid = MELUUIDMakeV7();
    }

    MELInputStreamRead(inputStream, self.name, kSaveGameNameLength * sizeof(char));

    if (version >= kVersion3) {
        self.card.rank = MELInputStreamReadByte(inputStream);
        self.card.suit = MELInputStreamReadByte(inputStream);
    } else {
        self.card.rank = MELRandomInt(RankCount);
        self.card.suit = MELRandomInt(SuitCount);
    }

    self.language = MELInputStreamReadByte(inputStream);
    self.lastCharacter = MELInputStreamReadByte(inputStream);

    const uint32_t now = playdate->system->getSecondsSinceEpoch(NULL);
    const int storyCount = version < kVersion5 ? 5 : kStoryCount;
    if (version < kVersion8) {
        for (unsigned int story = 0; story < storyCount; story++) {
            const MELBoolean completed = MELInputStreamReadBoolean(inputStream);
            self.completion[story] = completed ? now : 0;
        }
    } else {
        for (unsigned int story = 0; story < kStoryCount; story++) {
            self.completion[story] = MELInputStreamReadUInt32(inputStream);
        }
    }
    for (unsigned int story = 0; story < storyCount; story++) {
        self.progression[story] = MELInputStreamReadInt8(inputStream);
    }
    self.schoolProgression = MELInputStreamReadUInt32(inputStream);

    if (version >= kVersion2) {
        self.playtime = MELInputStreamReadFloat(inputStream);
        self.wins = MELInputStreamReadUInt16(inputStream);
        self.loses = MELInputStreamReadUInt16(inputStream);
    }

    if (version >= kVersion4) {
        self.removedCards = MELInputStreamReadUInt32(inputStream);
        self.removedChips = MELInputStreamReadUInt32(inputStream);
        self.sentChips = MELInputStreamReadUInt32(inputStream);
        self.handCount[0] = MELInputStreamReadUInt32(inputStream);
        self.handCount[1] = MELInputStreamReadUInt32(inputStream);
        self.handCount[2] = MELInputStreamReadUInt32(inputStream);
        self.bestCombo = MELInputStreamReadByte(inputStream);
    }

    if (version >= kVersion6) {
        self.scoreAttackRun = MELInputStreamReadUInt16(inputStream);
        self.scoreAttackPersonalBest = MELInputStreamReadUInt32(inputStream);
    }
    if (version == kVersion6) {
        MELInputStreamRead(inputStream, self.scoreAttackLastScores, kVersion6ScoreAttackHistoryCount * sizeof(uint32_t));
        self.scoreAttackRun = MELIntMin(self.scoreAttackRun, kVersion6ScoreAttackHistoryCount);
    } else if (version >= kVersion7) {
        MELInputStreamRead(inputStream, self.scoreAttackLastScores, kScoreAttackHistoryCount * sizeof(uint32_t));
    }

    if (version >= kVersion11) {
        for (unsigned int index = 0; index < AchievementCount; index++) {
            self.achievementUnlockDate[index] = MELInputStreamReadUInt32(inputStream);
        }
    } else if (version >= kVersion9) {
        for (unsigned int index = 0; index < AchievementArcadeFirst; index++) {
            self.achievementUnlockDate[index] = MELInputStreamReadUInt32(inputStream);
        }
        for (unsigned int index = AchievementGameVictory1; index < AchievementCount; index++) {
            self.achievementUnlockDate[index] = MELInputStreamReadUInt32(inputStream);
        }
    } else {
        if (self.completion[StoryNameDove]) {
            self.achievementUnlockDate[AchievementStoryCeresEnd] = self.completion[StoryNameDove];
        }
        if (self.completion[StoryNameCaroline]) {
            self.achievementUnlockDate[AchievementStoryCarolineEnd] = self.completion[StoryNameCaroline];
        }
        if (self.completion[StoryNameClovis]) {
            self.achievementUnlockDate[AchievementStoryClovisEnd] = self.completion[StoryNameClovis];
        }
        if (self.completion[StoryNameBelleplume]) {
            self.achievementUnlockDate[AchievementStoryBelleplumeEnd] = self.completion[StoryNameBelleplume];
        }
        if (self.completion[StoryNameCrocoford]) {
            self.achievementUnlockDate[AchievementStoryCrocofordEnd] = self.completion[StoryNameCrocoford];
        }
        if (self.completion[StoryNameMelusine]) {
            self.achievementUnlockDate[AchievementStoryMelusineEnd] = self.completion[StoryNameMelusine];
        }
        if (self.scoreAttackRun >= 1) {
            self.achievementUnlockDate[AchievementScoreAttackFirst] = now;
        }
        if (self.scoreAttackPersonalBest >= 100000) {
            self.achievementUnlockDate[AchievementScoreAttack100000] = now;
        }
        if (self.wins >= 1) {
            self.achievementUnlockDate[AchievementGameVictory1] = now;
        }
        if (self.wins >= 20) {
            self.achievementUnlockDate[AchievementGameVictory20] = now;
        }
        if (self.loses >= 1) {
            self.achievementUnlockDate[AchievementGameDefeat1] = now;
        }
        if (self.bestCombo >= 2) {
            self.achievementUnlockDate[AchievementChainX2] = now;
        }
        if (self.bestCombo >= 4) {
            self.achievementUnlockDate[AchievementChainX4] = now;
        }
    }

    if (version >= kVersion10) {
        self.rallyRun = MELInputStreamReadUInt16(inputStream);
        self.rallyBestStage = MELInputStreamReadUInt8(inputStream);
        self.rallyPersonalBest = MELInputStreamReadUInt32(inputStream);
        MELInputStreamRead(inputStream, self.rallyLastScores, kScoreAttackHistoryCount * sizeof(uint32_t));
    }

    if (version >= kVersion12) {
        self.arcadeSaveState.game.score = MELInputStreamReadUInt32(inputStream);
        self.arcadeSaveState.game.deathCount = MELInputStreamReadUInt32(inputStream);
        self.arcadeSaveState.game.speed = MELInputStreamReadFloat(inputStream);
        self.arcadeSaveState.game.stage = MELInputStreamReadUInt8(inputStream);
        self.arcadeSaveState.game.winStreak = MELInputStreamReadUInt8(inputStream);
        self.arcadeSaveState.game.loseStreak = MELInputStreamReadUInt8(inputStream);

        if (version >= kVersion13) {
            self.arcadeSaveState.game.time = MELInputStreamReadFloat(inputStream);
            self.arcadeSaveState.game.bestWinStreak = MELInputStreamReadUInt8(inputStream);
            self.arcadeSaveState.game.continueCount = MELInputStreamReadUInt32(inputStream);
        } else {
            self.arcadeSaveState.game.bestWinStreak = self.arcadeSaveState.game.winStreak;
            self.arcadeSaveState.game.continueCount = self.arcadeSaveState.game.deathCount;
        }
        
        const MELBoolean hasBoard = MELInputStreamReadBoolean(inputStream);
        if (hasBoard) {
            self.arcadeSaveState.leftBoard = MELInputStreamReadUTF8String(inputStream, NULL);
            self.arcadeSaveState.rightBoard = MELInputStreamReadUTF8String(inputStream, NULL);
            self.arcadeSaveState.deck = MELInputStreamReadUTF8String(inputStream, NULL);
            self.arcadeSaveState.leftDeckIndex = MELInputStreamReadUInt32(inputStream);
            self.arcadeSaveState.rightDeckIndex = MELInputStreamReadUInt32(inputStream);
        }
    }

    if (!MELInputStreamEndAndValidateHash(inputStream)) {
        playdate->system->logToConsole("Bad hash! Save file may be corrupted.");
        return false;
    }

    *out = self;
    return true;
}

void SavedGameLoad(SaveGameList * _Nonnull self) {
    self->count = 0;
    int index = 0;
    char path[] = "save0.data";
    MELInputStream inputStream = MELInputStreamOpen(path, kFileReadData);
    while (inputStream.file) {
        SaveGame saveGame;
        const MELBoolean success = ReadSavedGame(&inputStream, &saveGame);
        MELInputStreamClose(&inputStream);
        if (!success) {
            saveGame = (SaveGame) {};

            // TODO: Afficher visuellement "corrompu" dans le jeu plutôt que faire une copie.
            char newPath[] = "save0.corrupted";
            newPath[kFileNameNumberIndex] = '0' + index;
            playdate->file->rename(path, newPath);
        }
        saveGame.index = index;
        SaveGameListPush(self, saveGame);

        index++;
        path[kFileNameNumberIndex] = '0' + index;
        inputStream = MELInputStreamOpen(path, kFileReadData);
    }
    MELInputStreamClose(&inputStream);
}

SaveGame SavedGameLoadFirst(void) {
    SaveGame saveGame = (SaveGame) {
        .language = MELLanguageEnglish,
    };
    MELInputStream inputStream = MELInputStreamOpen("save0.data", kFileReadData);
    if (inputStream.file) {
        SaveGame result;
        const MELBoolean success = ReadSavedGame(&inputStream, &result);
        if (success) {
            saveGame = result;
        }
    }
    MELInputStreamClose(&inputStream);
    return saveGame;
}

void SaveGameDelete(SaveGameList * _Nonnull self, unsigned int indexToDelete) {
    char path[kFileNameLength] = "save0.data";
    path[kFileNameNumberIndex] = '0' + indexToDelete;

    // Fait une copie de l'ancienne sauvegarde plutôt que de supprimer réellement.
    char deletedPath[] = "save0.deleted";
    deletedPath[kFileNameNumberIndex] = '0' + indexToDelete;
    playdate->file->rename(path, deletedPath);

    char newPath[kFileNameLength] = "save0.data";
    newPath[kFileNameNumberIndex] = '0' + indexToDelete;

    const int count = self->count;
    for (int index = indexToDelete + 1; index < count; index++) {
        self->memory[index].index--;
        path[kFileNameNumberIndex] = '0' + index;
        playdate->file->rename(path, newPath);

        newPath[kFileNameNumberIndex] = '0' + index;
    }
    SaveGameListRemove(self, indexToDelete);
}

MELBoolean isLessonDone(uint8_t index) {
    return (currentSaveGame.schoolProgression & (1 << index)) != 0;
}
void markLessonDone(uint8_t index) {
    currentSaveGame.schoolProgression = currentSaveGame.schoolProgression | (1 << index);
    SaveGameToDisk(&currentSaveGame);
}

void unlockAchievement(Achievement achievement) {
    unlockAchievementAt(achievement, playdate->system->getSecondsSinceEpoch(NULL));
}

void unlockAchievementAt(Achievement achievement, uint32_t date) {
    uint32_t currentDate = currentSaveGame.achievementUnlockDate[achievement];
    if (!currentDate) {
        currentDate = currentSaveGame.achievementUnlockDate[achievement] = date;
    }
    MELAchievementGrantAt(achievement, currentDate);
}
