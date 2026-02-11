//
//  ranking_board.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 18/06/2023.
//

#include "scores.h"
#include "savegame.h"
#include "achievements.h"

#include "../lib/melice.h"

MELListImplement(Score);

/// Fichier de sauvegarde de la version 1.0
static const char kSaveFile[] = "rankings.data";

/// Version 2
/// Sauvegarde des scores du mode Rally.
/// @since 10/07/2025
static const uint16_t kVersion2 = 2;

static const uint16_t kLatestVersion = kVersion2;
/// Nombre maximum de résultats (mélange les scores en ligne et hors ligne).
static const unsigned int kMaximumScoreCount = 20;

static const char kMagic[] = "PPMScores";
#define kMagicCount 9

static const char kScoreAttackBoardId[] = "scoreattack";
static const char kRallyBoardId[] = "arcade";

static const char * _Nonnull kSalt = "a6229ce1-a3d5-40f4-baec-21cb4c8e9963";
static const int kSaltLength = 36;

ScoreList scoreAttackScores = (ScoreList) {};
PDScore scoreAttackPersonalBest = (PDScore) {};

ScoreList rallyScores = (ScoreList) {};
PDScore rallyPersonalBest = (PDScore) {};

uint32_t scoreLastUpdateDate = 0;
char * _Nullable scoreAttackLastUpdateError = NULL;
char * _Nullable rallyLastUpdateError = NULL;
static volatile unsigned int scoreAttackRequestSentDate = 0;
static volatile unsigned int rallyRequestSentDate = 0;

static Score loadEntry(MELInputStream * _Nonnull inputStream, int version);
static void saveEntry(Score self, MELOutputStream * _Nonnull outputStream);

static void ScoreListClear(ScoreList * _Nonnull self);

void LocalScoresAdd(ScoreList * _Nonnull scores, Score score);

static void scoreAttackScoresWereDownloaded(PDScoresList * _Nullable scoresList, const char * _Nullable errorMessage);
static void scoreAttackScoreWasUploaded(PDScore * _Nullable score, const char * _Nullable errorMessage);
static void rallyScoresWereDownloaded(PDScoresList * _Nullable scoresList, const char * _Nullable errorMessage);
static void rallyScoreWasUploaded(PDScore * _Nullable score, const char * _Nullable errorMessage);

void ScoresAdd(ScoreBoard scoreBoard, uint32_t score) {
    // Meilleur score.
    if (scoreBoard == ScoreBoardScoreAttack && score > currentSaveGame.scoreAttackPersonalBest) {
        currentSaveGame.scoreAttackPersonalBest = score;
    } else if (score > currentSaveGame.rallyPersonalBest) {
        currentSaveGame.rallyPersonalBest = score;
    }

    // Insertion en local.
    ScoreList *scoreList = scoreBoard == ScoreBoardScoreAttack ? &scoreAttackScores : &rallyScores;
    const int rank = ScoreListFindRank(*scoreList, score);
    if (rank >= 0 && rank < kMaximumScoreCount) {
        LocalScoresAdd(scoreList, (Score) {
            .playerUUID = currentSaveGame.uuid,
            .super = {
                .rank = rank + 1,
                .value = score,
                // TODO: Demander le nom du joueur pour l'afficher
                .player = NULL,
            }
        });
    }

    // Sauvegarde.
    LocalScoresSave();

    // Envoi du score au serveur et calcul du personal best.
    if (scoreBoard == ScoreBoardScoreAttack) {
        OnlineScoresAddScoreAttackScore(score);
    } else {
        OnlineScoresAddRallyScore(score);
    }
}

void LocalScoresLoad(void) {
    MELInputStream inputStream = MELInputStreamOpen(kSaveFile, kFileReadData);
    if (!inputStream.file) {
        MELInputStreamClose(&inputStream);
        ScoreListClear(&scoreAttackScores);
        ScoreListClear(&rallyScores);
        return;
    }
    jmp_buf exceptionHandler = {};
    if (setjmp(exceptionHandler)) {
        // NOTE: En cas d'erreur de chargement, la méthode ScoreListClear peut tenter de libérer des adresses mémoires erronnées.
        scoreAttackScores.count = 0;
        scoreLastUpdateDate = 0;
        MELInputStreamClose(&inputStream);
        playdate->file->unlink(kSaveFile, false);
        return;
    }
    inputStream.exceptionHandler = &exceptionHandler;

    // Chargement du numéro de version.
    int version = 0;
    char magic[kMagicCount];
    MELInputStreamRead(&inputStream, magic, kMagicCount);
    if (!strncmp(magic, kMagic, kMagicCount)) {
        version = MELInputStreamReadInt(&inputStream);
    }

    if (version == 0 || version > kLatestVersion) {
        playdate->system->logToConsole("Unsupported score file version, last supported version is %d but was: %d", kLatestVersion, version);
        scoreAttackScores.count = 0;
        MELInputStreamClose(&inputStream);
        return;
    } else if (version < kLatestVersion) {
        playdate->system->logToConsole("Loading scores from an older version, last version is %d, given: %d", kLatestVersion, version);
    }

    MELInputStreamStartHash(&inputStream, kSalt, kSaltLength);

    ScoreListClear(&scoreAttackScores);
    ScoreListClear(&rallyScores);

    // Date de dernière mise à jour des scores en ligne
    /* scoreLastUpdateDate = */ MELInputStreamReadUInt32(&inputStream);

    // Meilleur score sur la console.
    scoreAttackPersonalBest.player = MELInputStreamReadNullableString(&inputStream);
    scoreAttackPersonalBest.value = MELInputStreamReadUInt32(&inputStream);
    scoreAttackPersonalBest.rank = MELInputStreamReadUInt32(&inputStream);

    // Lecture des scores
    const uint32_t count = MELInputStreamReadUInt32(&inputStream);
    if (count > kMaximumScoreCount) {
        playdate->system->logToConsole("Bad maximum count. Expected at most: %d, but was: %d", kMaximumScoreCount, count);
        longjmp(*inputStream.exceptionHandler, 2);
    }
    scoreAttackScores.count = count;
    ScoreListEnsureCapacity(&scoreAttackScores, count);
    for (unsigned int index = 0; index < count; index++) {
        const Score score = loadEntry(&inputStream, version);
        scoreAttackScores.memory[index] = score;
    }

    if (version >= kVersion2) {
        // Meilleur score sur la console.
        rallyPersonalBest.player = MELInputStreamReadNullableString(&inputStream);
        rallyPersonalBest.value = MELInputStreamReadUInt32(&inputStream);
        rallyPersonalBest.rank = MELInputStreamReadUInt32(&inputStream);

        // Lecture des scores
        const uint32_t count = MELInputStreamReadUInt32(&inputStream);
        if (count > kMaximumScoreCount) {
            playdate->system->logToConsole("Bad maximum count. Expected at most: %d, but was: %d", kMaximumScoreCount, count);
            longjmp(*inputStream.exceptionHandler, 2);
        }
        rallyScores.count = count;
        ScoreListEnsureCapacity(&rallyScores, count);
        for (unsigned int index = 0; index < count; index++) {
            const Score score = loadEntry(&inputStream, version);
            rallyScores.memory[index] = score;
        }
    }

    if (!MELInputStreamEndAndValidateHash(&inputStream)) {
        playdate->system->logToConsole("Unable to load scores, hash does not match.");

        ScoreListClear(&scoreAttackScores);
        ScoreListClear(&rallyScores);
        scoreLastUpdateDate = 0;
        MELInputStreamClose(&inputStream);
        playdate->file->unlink(kSaveFile, false);
        return;
    }

    MELInputStreamClose(&inputStream);
}

void LocalScoresSave(void) {
    playdate->system->logToConsole("Will save local scores!");
    MELOutputStream outputStream = MELOutputStreamOpen(kSaveFile);
    // En-tête
    MELOutputStreamWrite(&outputStream, kMagic, kMagicCount);
    MELOutputStreamWriteInt(&outputStream, kLatestVersion);

    MELOutputStreamStartHash(&outputStream, kSalt, kSaltLength);

    // Date de dernière mise à jour des scores en ligne
    MELOutputStreamWriteUInt32(&outputStream, scoreLastUpdateDate);

    // Meilleur score sur la console.
    MELOutputStreamWriteNullableString(&outputStream, scoreAttackPersonalBest.player);
    MELOutputStreamWriteUInt32(&outputStream, scoreAttackPersonalBest.value);
    MELOutputStreamWriteUInt32(&outputStream, scoreAttackPersonalBest.rank);

    // Écriture des scores
    MELOutputStreamWriteUInt32(&outputStream, scoreAttackScores.count);
    for (unsigned int index = 0; index < scoreAttackScores.count; index++) {
        saveEntry(scoreAttackScores.memory[index], &outputStream);
    }

    // Version 2 : scores du mode Rally
    // Meilleur score sur la console.
    MELOutputStreamWriteNullableString(&outputStream, rallyPersonalBest.player);
    MELOutputStreamWriteUInt32(&outputStream, rallyPersonalBest.value);
    MELOutputStreamWriteUInt32(&outputStream, rallyPersonalBest.rank);

    // Écriture des scores
    MELOutputStreamWriteUInt32(&outputStream, rallyScores.count);
    for (unsigned int index = 0; index < rallyScores.count; index++) {
        saveEntry(rallyScores.memory[index], &outputStream);
    }

    MELOutputStreamEndHash(&outputStream);
    MELOutputStreamClose(&outputStream);

    playdate->system->logToConsole("Saved scores");
}

void LocalScoresAdd(ScoreList * _Nonnull scores, Score score) {
    const unsigned int index = score.super.rank - 1;
    if (index >= kMaximumScoreCount) {
        // Non classé.
        return;
    }

    scores->count = MELIntMin(scores->count + 1, kMaximumScoreCount);
    ScoreListEnsureCapacity(scores, scores->count);

    const ScoreList list = *scores;
    for (int i = list.count - 1; i > index; i--) {
        list.memory[i] = list.memory[i - 1];
        list.memory[i].super.rank = i + 1;
    }

    list.memory[index] = score;
}

void OnlineScoresLoad(void) {
#if ENABLE_SCOREBOARD_API
    const unsigned int now = playdate->system->getSecondsSinceEpoch(NULL);
    const unsigned int thirtyMinutes = 30 * 60;
    if (scoreLastUpdateDate > 0 && now - scoreLastUpdateDate < thirtyMinutes) {
        // Un rechargement a déjà eu lieu récemment, rien à faire.
        playdate->system->logToConsole("Not loading online scores because last load happened before 30 minutes.");
        return;
    }
    if (OnlineScoresAreLoading()) {
        playdate->system->logToConsole("Already loading.");
        return;
    }
    playdate->system->logToConsole("Loading online scoreAttackScores...");
    rallyRequestSentDate = scoreAttackRequestSentDate = playdate->system->getCurrentTimeMilliseconds();
    if (scoreAttackLastUpdateError) {
        playdate->system->realloc(scoreAttackLastUpdateError, 0);
    }
    scoreAttackLastUpdateError = NULL;
    if (rallyLastUpdateError) {
        playdate->system->realloc(rallyLastUpdateError, 0);
    }
    rallyLastUpdateError = NULL;
#if MOCK_SCOREBOARD_API
    static const int scoreCount = 10;
    PDScore *mockScores = playdate->system->realloc(NULL, sizeof(PDScore) * scoreCount);

    static const char *kMockPlayerNames[] = {
        "chrome",
        "hekhound",
        "Daeke",
        "Dean_0",
        "7618390003094360",
        "SpeedFighter64",
        "Drawskull",
        "liammiller18",
        "pringus",
        "R2D21999",
        "other",
    };
    const int max = scoreCount - (scoreAttackPersonalBest.player != NULL);
    for (unsigned int index = 0; index < max; index++) {
        const uint32_t value = 5000 * (scoreCount - index);
        mockScores[index] = (PDScore) {
            .player = MELStringCopy(kMockPlayerNames[index]),
            .value = value + MELRandomInt(value - 1),
            .rank = index + 1
        };
    }
    if (scoreAttackPersonalBest.player) {
        unsigned int index = 0;
        for (; index < scoreCount; index++) {
            if (scoreAttackPersonalBest.value > mockScores[index].value || (index == scoreCount - 1)) {
                memmove(mockScores + index + 1, mockScores + index, sizeof(PDScore) * (scoreCount - 1 - index));
                mockScores[index] = (PDScore) {
                    .player = MELStringCopy(scoreAttackPersonalBest.player),
                    .value = scoreAttackPersonalBest.value,
                    .rank = index + 1
                };
                break;
            }
        }
        for (index++; index < scoreCount; index++) {
            mockScores[index].rank++;
        }
    }
    PDScoresList *list = playdate->system->realloc(NULL, sizeof(PDScoresList));
    *list = (PDScoresList) {
        .count = scoreCount,
        .boardID = MELStringCopy(kScoreAttackBoardId),
        .lastUpdated = playdate->system->getSecondsSinceEpoch(NULL),
        .scores = mockScores,
    };
    scoreAttackScoresWereDownloaded(list, NULL);
#else
    playdate->scoreboards->getScores(kScoreAttackBoardId, scoreAttackScoresWereDownloaded);
    playdate->scoreboards->getScores(kRallyBoardId, rallyScoresWereDownloaded);
#endif /* MOCK_SCOREBOARD_API */
#else
    // Scoreboard is disabled.
#endif /* ENABLE_SCOREBOARD_API */
}

void OnlineScoresAddScoreAttackScore(uint32_t score) {
#if ENABLE_SCOREBOARD_API
    // Mets la date de dernière mise à jour à zéro pour forcer un rechargement.
    scoreLastUpdateDate = 0;
    playdate->system->logToConsole("Uploading score attack score %u...", score);
    scoreAttackRequestSentDate = playdate->system->getCurrentTimeMilliseconds();

    if (scoreAttackLastUpdateError) {
        playdate->system->realloc(scoreAttackLastUpdateError, 0);
    }
    scoreAttackLastUpdateError = NULL;

    playdate->scoreboards->addScore(kScoreAttackBoardId, score, scoreAttackScoreWasUploaded);
#endif // ENABLE_SCOREBOARD_API

#if MOCK_SCOREBOARD_API
    PDScore *result = new(PDScore);
    *result = (PDScore) {
        .value = score,
        .player = MELStringCopy("1234567890123456"),
        .rank = 10
    };
    scoreAttackScoreWasUploaded(result, NULL);
#endif // MOCK_SCOREBOARD_API
}

void OnlineScoresAddRallyScore(uint32_t score) {
#if ENABLE_SCOREBOARD_API
    // Mets la date de dernière mise à jour à zéro pour forcer un rechargement.
    scoreLastUpdateDate = 0;
    playdate->system->logToConsole("Uploading rally score %u...", score);
    rallyRequestSentDate = playdate->system->getCurrentTimeMilliseconds();

    if (rallyLastUpdateError) {
        playdate->system->realloc(rallyLastUpdateError, 0);
    }
    rallyLastUpdateError = NULL;

    playdate->scoreboards->addScore(kRallyBoardId, score, rallyScoreWasUploaded);
#endif // ENABLE_SCOREBOARD_API

#if MOCK_SCOREBOARD_API
    PDScore *result = new(PDScore);
    *result = (PDScore) {
        .value = score,
        .player = MELStringCopy("1234567890123456"),
        .rank = 10
    };
    rallyScoreWasUploaded(result, NULL);
#endif // MOCK_SCOREBOARD_API
}

MELBoolean OnlineScoresScoreAttackIsLoading(void) {
    return scoreAttackRequestSentDate != 0;
}
MELBoolean OnlineScoresRallyIsLoading(void) {
    return rallyRequestSentDate != 0;
}
MELBoolean OnlineScoresAreLoading(void) {
    return scoreAttackRequestSentDate != 0 || rallyRequestSentDate != 0;
}

/**
 * Cherche l'indice où insérer le score du joueur.
 *
 * @param scores Tableau des scores triés du plus grand au plus petit.
 * @param userScore Score de l'utilisateur.
 * @returns `count` si le joueur n'est pas dans le classement, l'indice où insérer son score sinon.
 */
int ScoreListFindRank(const ScoreList scores, uint32_t userScore) {
    int startIndex = 0;
    int endIndex = scores.count - 1;

    while (startIndex <= endIndex) {
        const int index = (startIndex + endIndex) / 2;
        const uint32_t centerValue = scores.memory[index].super.value;

        if (userScore < centerValue) {
            startIndex = index + 1;
        } else {
            endIndex = index - 1;
        }
    }
    return startIndex;
}


#pragma mark - Méthodes privées

static void ScoreListClear(ScoreList * _Nonnull self) {
    Score *memory = self->memory;
    const unsigned int count = self->count;
    for (unsigned int index = 0; index < count; index++) {
        playdate->system->realloc(memory[index].super.player, 0);
        memory[index].super.player = NULL;
    }
    self->count = 0;

    scoreAttackPersonalBest.player = NULL;
    scoreAttackPersonalBest.value = 0;
    scoreAttackPersonalBest.rank = 0;
}

#pragma mark - Gestion des scores locaux

static Score loadEntry(MELInputStream * _Nonnull inputStream, int version) {
    Score entry;
    entry.playerUUID = MELInputStreamReadUUID(inputStream);
    entry.super.player = MELInputStreamReadNullableString(inputStream);
    entry.super.value = MELInputStreamReadUInt32(inputStream);
    entry.super.rank = MELInputStreamReadUInt32(inputStream);
    return entry;
}

static void saveEntry(Score entry, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteUUID(outputStream, entry.playerUUID);
    MELOutputStreamWriteNullableString(outputStream, entry.super.player);
    MELOutputStreamWriteUInt32(outputStream, entry.super.value);
    MELOutputStreamWriteUInt32(outputStream, entry.super.rank);
}

#pragma mark - Gestion des scores en ligne

static void scoresWereDownloaded(ScoreList * _Nonnull scores, PDScore * _Nonnull personalBest, PDScoresList * _Nullable scoresList) {
    // Combine les nouveaux scores avec ceux enregistrés.
    ScoreList localScores = ScoreListMakeWithInitialCapacity(scores->count);
    for (unsigned int index = 0; index < scores->count; index++) {
        const Score score = scores->memory[index];
        // Ne conserve que les scores des joueurs locaux.
        const MELBoolean isLocalPlayer = !MELUUIDIsEmpty(score.playerUUID);
        if (isLocalPlayer && score.super.value > 0) {
            ScoreListPush(&localScores, score);
        } else if (score.super.player) {
            playdate->system->logToConsole("Will free name of player %d at address %x", index, score.super.player);
            playdate->system->realloc(score.super.player, 0);
            scores->memory[index].super.player = NULL;
        }
    }
    scores->count = 0;

    unsigned int downloadedIndex = 0;
    unsigned int localIndex = 0;
    for (unsigned int index = 0; index < kMaximumScoreCount; index++) {
        Score localScore = (Score) {};
        PDScore downloadedScore = (PDScore) {};
        const MELBoolean localHasNext = localIndex < localScores.count;
        const MELBoolean downloadedHasNext = downloadedIndex < scoresList->count;
        if (!localHasNext && !downloadedHasNext) {
            // Fin, on a inséré tous les scores.
            break;
        }
        if (localHasNext) {
            localScore = localScores.memory[localIndex];
        }
        if (downloadedHasNext) {
            downloadedScore = scoresList->scores[downloadedIndex];
        }
        if (localHasNext && localScore.super.value >= downloadedScore.value) {
            localScore.super.rank = 0;
            if (localScore.super.player) {
                playdate->system->realloc(localScore.super.player, 0);
                localScore.super.player = NULL;
            }
            if (localScore.super.value == downloadedScore.value
                    && personalBest->player
                    && MELNullableStringEquals(personalBest->player, downloadedScore.player)) {
                localScore.super.rank = downloadedScore.rank;
                localScore.super.player = MELStringCopy(downloadedScore.player);
                // C'est le score d'un joueur local, on saute !
                // Il peut arriver qu'on ne détecte pas bien ce cas en cas de suppression des sauvegardes mais bon ça devrait être relativement rare et acceptable. Cela se corrigera tout seul après le score envoyé.
                downloadedIndex++;
            }
            ScoreListPush(scores, localScore);
            localIndex++;
        } else {
            ScoreListPush(scores, (Score) {
                .super = {
                    .rank = downloadedScore.rank,
                    .value = downloadedScore.value,
                    .player = MELStringCopy(downloadedScore.player),
                }
            });
            downloadedIndex++;
        }
    }
    for (; localIndex < localScores.count; localIndex++) {
        const Score score = localScores.memory[localIndex];
        if (score.super.player) {
            playdate->system->realloc(score.super.player, 0);
        }
    }

    // Conservation de la date de mise à jour.
    scoreLastUpdateDate = scoresList->lastUpdated;

    // Libération des ressources.
    playdate->scoreboards->freeScoresList(scoresList);
    ScoreListDeinit(&localScores);
}

static void scoreAttackScoresWereDownloaded(PDScoresList * _Nullable scoresList, const char * _Nullable errorMessage) {
#if DEBUG
    playdate->system->logToConsole("Score attack scores download took: %fs", (playdate->system->getCurrentTimeMilliseconds() - scoreAttackRequestSentDate) / 1000.0);
#endif
    if (scoresList == NULL || errorMessage != NULL) {
        playdate->system->logToConsole("Unable to load score attack scores: %s", errorMessage);
        if (scoresList) {
            playdate->scoreboards->freeScoresList(scoresList);
        }
        if (scoreAttackLastUpdateError) {
            playdate->system->realloc(scoreAttackLastUpdateError, 0);
        }
        scoreAttackLastUpdateError = MELStringCopy(errorMessage);
        scoreAttackRequestSentDate = 0;
        return;
    }

    // Fin du chargement, remise de la date d'envoi à zéro.
    scoreAttackRequestSentDate = 0;
    scoresWereDownloaded(&scoreAttackScores, &scoreAttackPersonalBest, scoresList);
}

static void rallyScoresWereDownloaded(PDScoresList * _Nullable scoresList, const char * _Nullable errorMessage) {
#if DEBUG
    playdate->system->logToConsole("Rally scores download took: %fs", (playdate->system->getCurrentTimeMilliseconds() - rallyRequestSentDate) / 1000.0);
#endif
    if (scoresList == NULL || errorMessage != NULL) {
        playdate->system->logToConsole("Unable to load rally scores: %s", errorMessage);
        if (scoresList) {
            playdate->scoreboards->freeScoresList(scoresList);
        }
        if (rallyLastUpdateError) {
            playdate->system->realloc(rallyLastUpdateError, 0);
        }
        rallyLastUpdateError = MELStringCopy(errorMessage);
        rallyRequestSentDate = 0;
        return;
    }

    // Fin du chargement, remise de la date d'envoi à zéro.
    rallyRequestSentDate = 0;
    scoresWereDownloaded(&rallyScores, &rallyPersonalBest, scoresList);
}

static void scoreWasUploaded(ScoreList * _Nonnull scores, PDScore * _Nonnull personalBest, PDScore * _Nullable score, const char * _Nullable errorMessage) {
    if (errorMessage) {
        playdate->system->logToConsole("Unable to upload online score: %s", errorMessage);
        return;
    }
    if (score) {
        playdate->system->logToConsole("Score was uploaded. Rank %d", score->rank);
        if (personalBest->player && score->player) {
            playdate->system->realloc(personalBest->player, 0);
            personalBest->player = NULL;
        }
        if (score->player) {
            personalBest->player = MELStringCopy(score->player);
        }
        if (score->value >= personalBest->value) {
            personalBest->value = score->value;
            personalBest->rank = score->rank;
        }
        playdate->scoreboards->freeScore(score);
    }
    OnlineScoresLoad();
}

static void scoreAttackScoreWasUploaded(PDScore * _Nullable score, const char * _Nullable errorMessage) {
#if DEBUG
    playdate->system->logToConsole("Score upload took: %fs", (playdate->system->getCurrentTimeMilliseconds() - scoreAttackRequestSentDate) / 1000.0);
#endif
    scoreAttackRequestSentDate = 0;

    scoreWasUploaded(&scoreAttackScores, &scoreAttackPersonalBest, score, errorMessage);
}

static void rallyScoreWasUploaded(PDScore * _Nullable score, const char * _Nullable errorMessage) {
#if DEBUG
    playdate->system->logToConsole("Score upload took: %fs", (playdate->system->getCurrentTimeMilliseconds() - rallyRequestSentDate) / 1000.0);
#endif
    rallyRequestSentDate = 0;

    scoreWasUploaded(&rallyScores, &rallyPersonalBest, score, errorMessage);
}
