//
//  ranking_board.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 18/06/2023.
//

#include "rankingboard.h"

#include "../lib/melice.h"

MELListImplement(Score);

static const char kPlayerNameSuffixKatsuo[] = " (katsuo)";
static const char kPlayerNameSuffixSaki[] = " (saki)";
const unsigned int kKatsuoSuffixLength = 9;
const unsigned int kSakiSuffixLength = 7;

static const char kRankingsFileName[] = "rankings.data";
/// **Version 1** : Sauvegarde des scores en ligne.
/// **Version 2** : Sauvegarde du taux de rafraîchissement (30 ou 50 fps).
/// **Version 3** : Sauvegarde du rang.
/// **Version 4** : Ajout du meilleur score local.
/// **Version 5** : Stockage du nombre d'entrées dans les scores locaux.
/// **Version 6** : Stockage des rangs dans les scores locaux. 17/01/2025.
static const int kLastVersion = 6;
#if ENABLE_ONLINE_SCOREBOARDS
static const unsigned int kLocalScoresCount = 10;
#else
static const unsigned int kLocalScoresCount = 20;
#endif
/// Nombre maximum de résultats en ligne (11 scores x 2 listes).
static const unsigned int kOnlineScoresMaximumCount = 22;

static const char kMagic[] = {'K', 'O', 'S'};
static const int kMagicCount = 3;

static const char kSakiBoardId[] = "saki";
static const char kKatsuoBoardId[] = "katsuo";

ScoreList localScores = {};
ScoreList onlineScores = {};
uint32_t localBestScore = 0;
uint32_t scoreLastUpdateDate = 0;
static unsigned int requestSentDate = 0;

Ranks bestRanks[kPlayerCharacterCount] = {};

uint32_t worstScores[kPlayerCharacterCount] = {0, 0};

static volatile PDScoresList * _Nullable sakiScoresList = NULL;
static volatile PDScoresList * _Nullable katsuoScoresList = NULL;
static PDScoresList emptyScoreList = {};

static void createDefaultScores(void);

static Score loadEntry(MELInputStream * _Nonnull inputStream, int version);
static void saveEntry(Score self, MELOutputStream * _Nonnull outputStream);

static void ScoreListClear(ScoreList * _Nonnull self);

static void sakiScoresWasDownloaded(PDScoresList *scores, const char *errorMessage);
static void sakiScoreWasUploaded(PDScore * _Nullable score, const char * _Nullable errorMessage);

static void katsuoScoresWasDownloaded(PDScoresList *scores, const char *errorMessage);
static void katsuoScoreWasUploaded(PDScore * _Nullable score, const char * _Nullable errorMessage);

void LocalScoresLoad(void) {
    MELInputStream inputStream = MELInputStreamOpen(kRankingsFileName, kFileReadData);
    if (!inputStream.file) {
        MELInputStreamClose(&inputStream);
        createDefaultScores();
        ScoreListClear(&onlineScores);
        return;
    }
    jmp_buf exceptionHandler = {};
    if (setjmp(exceptionHandler)) {
        createDefaultScores();
        // NOTE: En cas d'erreur de chargement, la méthode ScoreListClear peut tenter de libérer des adresses mémoires erronnées.
        onlineScores.count = 0;
        scoreLastUpdateDate = 0;
        MELInputStreamClose(&inputStream);
        playdate->file->unlink(kRankingsFileName, false);
        return;
    }
    inputStream.exceptionHandler = &exceptionHandler;

    // Chargement du numéro de version.
    int version;
    char magic[kMagicCount];
    MELInputStreamRead(&inputStream, magic, kMagicCount);
    if (!strncmp(magic, kMagic, kMagicCount)) {
        version = MELInputStreamReadInt(&inputStream);
    } else {
        // Pas d'en-tête, version 0. Retour au début du fichier.
        // TODO: Afficher une erreur en cas de version 0 ?
        version = 0;
        MELInputStreamSeek(&inputStream, 0, MELInputStreamSeekFromStart);
    }

    if (version > kLastVersion) {
        playdate->system->logToConsole("Unsupported score file version, last supported version is %d but was: %d", kLastVersion, version);
        createDefaultScores();
        MELInputStreamClose(&inputStream);
        return;
    } else if (version < kLastVersion) {
        playdate->system->logToConsole("Loading scores from an older version, last version is %d, given: %d", kLastVersion, version);
    }
    if (version >= 5) {
        localScores.count = MELInputStreamReadUInt32(&inputStream);
    } else {
        localScores.count = kLocalScoresCount;
    }
    ScoreListEnsureCapacity(&localScores, localScores.count);

    if (version >= 3) {
        for (unsigned int index = 0; index < localScores.count; index++) {
            localScores.memory[index] = loadEntry(&inputStream, version);
        }
    } else {
        for (unsigned int index = 0; index < kLocalScoresCount; index++) {
            Score score = loadEntry(&inputStream, version);
            score.super.rank = index + 1;

            localScores.memory[index] = score;
        }
    }

    ScoreListClear(&onlineScores);
    scoreLastUpdateDate = 0;
    if (version >= 1) {
        scoreLastUpdateDate = MELInputStreamReadUInt32(&inputStream);
        const uint32_t count = MELInputStreamReadUInt32(&inputStream);
        if (count > kOnlineScoresMaximumCount) {
            longjmp(*inputStream.exceptionHandler, 2);
        }
        onlineScores.count = count;
        ScoreListEnsureCapacity(&onlineScores, onlineScores.count);
        if (version >= 3) {
            for (unsigned int index = 0; index < onlineScores.count; index++) {
                const Score score = loadEntry(&inputStream, version);
                onlineScores.memory[index] = score;

                if (score.super.rank <= 20) {
                    worstScores[score.character] = score.super.value;
                }
            }
        } else {
            for (unsigned int index = 0; index < onlineScores.count; index++) {
                Score score = loadEntry(&inputStream, version);
                score.super.rank = index + 1;

                onlineScores.memory[index] = score;
                worstScores[score.character] = score.super.value;
            }
        }
    }
    if (version >= 2) {
        /* const int refreshRate = */ MELInputStreamReadByte(&inputStream);
    }
    if (version >= 4) {
        localBestScore = MELInputStreamReadUInt32(&inputStream);
    }
    MELInputStreamClose(&inputStream);
}

void LocalScoresSave(void) {
    MELOutputStream outputStream = MELOutputStreamOpen(kRankingsFileName);
    // En-tête
    MELOutputStreamWrite(&outputStream, kMagic, kMagicCount);
    MELOutputStreamWriteInt(&outputStream, kLastVersion);

    // Version 5 : sauvegarde du nombre d'entrées.
    MELOutputStreamWriteUInt32(&outputStream, localScores.count);

    // Scores locaux
    for (unsigned int index = 0; index < localScores.count; index++) {
        saveEntry(localScores.memory[index], &outputStream);
    }

    // Version 1
    // Date de dernière mise à jour des scores en ligne
    MELOutputStreamWriteUInt32(&outputStream, scoreLastUpdateDate);

    // Scores distants
    MELOutputStreamWriteUInt32(&outputStream, onlineScores.count);
    for (unsigned int index = 0; index < onlineScores.count; index++) {
        saveEntry(onlineScores.memory[index], &outputStream);
    }

    // Version 2
    // Refresh rate
    MELOutputStreamWriteByte(&outputStream, REFRESH_RATE);

    // Version 4
    MELOutputStreamWriteUInt32(&outputStream, localBestScore);
    MELOutputStreamClose(&outputStream);

    playdate->system->logToConsole("Saved");
}

Score LocalScoresAdd(Score score) {
    const unsigned int index = score.super.rank - 1;
    if (index >= localScores.count) {
        // Non classé.
        return (Score) {};
    }
    Score lastScore = localScores.memory[localScores.count - 1];
    for (unsigned int i = localScores.count - 1; i > index; i--) {
        localScores.memory[i].super.value = localScores.memory[i - 1].super.value;
        localScores.memory[i].super.player = localScores.memory[i - 1].super.player;
        localScores.memory[i].character = localScores.memory[i - 1].character;
        localScores.memory[i].ranks = localScores.memory[i - 1].ranks;
    }
    localScores.memory[index] = score;
    LocalScoresSave();
    return lastScore;
}

void OnlineScoresLoad(void) {
    const unsigned int now = playdate->system->getSecondsSinceEpoch(NULL);
    const unsigned int eightHours = 60 * 60 * 8;
    if (scoreLastUpdateDate > 0 && now - scoreLastUpdateDate < eightHours) {
        // Un rechargement a déjà eu lieu aujourd'hui, rien à faire.
        playdate->system->logToConsole("Not loading online scores because last load happened before 8 hours.");
        return;
    }
    if (OnlineScoresAreLoading()) {
        playdate->system->logToConsole("Already loading.");
        return;
    }
    playdate->system->logToConsole("Loading online scores...");
    requestSentDate = playdate->system->getCurrentTimeMilliseconds();
    playdate->scoreboards->getScores(kSakiBoardId, sakiScoresWasDownloaded);
    playdate->scoreboards->getScores(kKatsuoBoardId, katsuoScoresWasDownloaded);
}

void OnlineScoresAdd(PlayerCharacter character, uint32_t score) {
    // Mets la date de dernière mise à jour à zéro pour forcer un rechargement.
    scoreLastUpdateDate = 0;
    playdate->system->logToConsole("Uploading score %u...", score);
    requestSentDate = playdate->system->getCurrentTimeMilliseconds();
    if (character == PlayerCharacterSaki) {
        playdate->scoreboards->addScore(kSakiBoardId, score, sakiScoreWasUploaded);
    } else {
        playdate->scoreboards->addScore(kKatsuoBoardId, score, katsuoScoreWasUploaded);
    }
    if (score > localBestScore) {
        localBestScore = score;
        LocalScoresSave();
    }
}

MELBoolean OnlineScoresAreLoading(void) {
    return requestSentDate != 0;
}

/**
 * Cherche l'indice où insérer le score du joueur.
 *
 * @param scores Tableau des scores triés du plus grand au plus petit.
 * @param userScore Score de l'utilisateur.
 * @returns `count` si le joueur n'est pas dans le classement, l'indice où insérer son score sinon.
 */
int ScoreListFindRank(ScoreList scores, unsigned int userScore) {
    int startIndex = 0;
    int endIndex = scores.count - 1;

    while (startIndex <= endIndex) {
        const int index = (startIndex + endIndex) / 2;
        const uint32_t centerValue = scores.memory[index].super.value;

        if (userScore == centerValue) {
            return index;
        } else if (userScore < centerValue) {
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
}

#pragma mark - Gestion des scores locaux

static void createDefaultScores(void) {
    ScoreListEnsureCapacity(&localScores, kLocalScoresCount);
    localScores.count = kLocalScoresCount;
    localScores.memory[0] = (Score) {
        .super = {
            .player = MELStringCopy("rca"),
            .value = 200000,
            .rank = 1,
        },
        .character = PlayerCharacterKatsuo,
    };
    localScores.memory[1] = (Score) {
        .super = {
            .player = MELStringCopy("amu"),
            .value = 100000,
            .rank = 2,
        },
        .character = PlayerCharacterSaki,
    };
    localScores.memory[2] = (Score) {
        .super = {
            .player = MELStringCopy("katsuo"),
            .value = 90000,
            .rank = 3,
        },
        .character = PlayerCharacterKatsuo,
    };
    localScores.memory[3] = (Score) {
        .super = {
            .player = MELStringCopy("saki"),
            .value = 80000,
            .rank = 4,
        },
        .character = PlayerCharacterSaki,
    };
    localScores.memory[4] = (Score) {
        .super = {
            .player = MELStringCopy("kuro"),
            .value = 70000,
            .rank = 5,
        },
        .character = PlayerCharacterKatsuo,
    };
    localScores.memory[5] = (Score) {
        .super = {
            .player = MELStringCopy("obi"),
            .value = 60000,
            .rank = 6,
        },
        .character = PlayerCharacterSaki,
    };
    localScores.memory[6] = (Score) {
        .super = {
            .player = MELStringCopy("thanks"),
            .value = 50000,
            .rank = 7,
        },
        .character = PlayerCharacterKatsuo,
    };
    localScores.memory[7] = (Score) {
        .super = {
            .player = MELStringCopy("for"),
            .value = 25000,
            .rank = 8,
        },
        .character = PlayerCharacterSaki,
    };
    localScores.memory[8] = (Score) {
        .super = {
            .player = MELStringCopy("playing"),
            .value = 10000,
            .rank = 9,
        },
        .character = PlayerCharacterKatsuo,
    };
    localScores.memory[9] = (Score) {
        .super = {
            .player = MELStringCopy("!!!"),
            .value = 5000,
            .rank = 10,
        },
        .character = PlayerCharacterSaki,
    };
#if !ENABLE_ONLINE_SCOREBOARDS
    for (unsigned int index = 10; index < kLocalScoresCount; index++) {
        localScores.memory[index] = (Score) {
            .super = {
                .player = MELStringCopy("---"),
                .value = 5000,
                .rank = index + 1,
            },
            .character = (index % 2) == 0 ? PlayerCharacterKatsuo : PlayerCharacterSaki,
        };
    }
#endif
}

static Score loadEntry(MELInputStream * _Nonnull inputStream, int version) {
    Score entry;
    unsigned int nameLength;
    entry.super.player = MELInputStreamReadUTF8String(inputStream, &nameLength);
    entry.super.value = MELInputStreamReadUInt32(inputStream);
    if (version >= 1) {
        entry.character = (PlayerCharacter) MELInputStreamReadByte(inputStream);
    } else {
        if (MELStringEndsWith(entry.super.player, kPlayerNameSuffixSaki)) {
            entry.character = PlayerCharacterSaki;
            entry.super.player[nameLength - kSakiSuffixLength] = '\0';
        } else if (MELStringEndsWith(entry.super.player, kPlayerNameSuffixKatsuo)) {
            entry.character = PlayerCharacterKatsuo;
            entry.super.player[nameLength - kKatsuoSuffixLength] = '\0';
        } else {
            playdate->system->error("Bad player name for version 0. Should ends with (saki) or (katsuo) but was: %s", entry.super.player);
            entry.character = PlayerCharacterKatsuo;
        }
    }
    if (version >= 3) {
        entry.super.rank = MELInputStreamReadUInt32(inputStream);
    }
    if (version >= 6) {
        entry.ranks.lives = MELInputStreamReadUInt8(inputStream);
        entry.ranks.timeLeft = MELInputStreamReadUInt8(inputStream);
        entry.ranks.combo = MELInputStreamReadUInt8(inputStream);
    } else {
        entry.ranks = (Ranks) {};
    }
    return entry;
}

static void saveEntry(Score entry, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteUTF8String(outputStream, entry.super.player);
    MELOutputStreamWriteUInt32(outputStream, entry.super.value);
    // Version 1
    MELOutputStreamWriteByte(outputStream, entry.character);
    // Version 3
    MELOutputStreamWriteUInt32(outputStream, entry.super.rank);
    // Version 6
    MELOutputStreamWriteUInt8(outputStream, entry.ranks.lives);
    MELOutputStreamWriteUInt8(outputStream, entry.ranks.timeLeft);
    MELOutputStreamWriteUInt8(outputStream, entry.ranks.combo);
}

#pragma mark - Gestion des scores en ligne

static void freeSakiAndKatsuScoresList(void) {
    if (sakiScoresList != &emptyScoreList) {
        playdate->scoreboards->freeScoresList((PDScoresList*) sakiScoresList);
    }
    if (katsuoScoresList != &emptyScoreList) {
        playdate->scoreboards->freeScoresList((PDScoresList*) katsuoScoresList);
    }
    sakiScoresList = NULL;
    katsuoScoresList = NULL;
}

static void mergeScores(void) {
    if (sakiScoresList == NULL || katsuoScoresList == NULL) {
        // Téléchargement des scores non terminé.
        return;
    }
#if DEBUG
    playdate->system->logToConsole("Score download took: %fs", (playdate->system->getCurrentTimeMilliseconds() - requestSentDate) / 1000.0);
#endif
    // Fin du chargement, remise de la date d'envoi à zéro.
    requestSentDate = 0;

    if (sakiScoresList == &emptyScoreList || katsuoScoresList == &emptyScoreList || (sakiScoresList->lastUpdated < scoreLastUpdateDate && katsuoScoresList->lastUpdated < scoreLastUpdateDate)) {
        // Pas de changement.
        freeSakiAndKatsuScoresList();
        return;
    }

    // Conservation de la date de mise à jour le plus petite entre Saki et Katsuo.
    scoreLastUpdateDate = MELIntMax(scoreLastUpdateDate, MELIntMin(sakiScoresList->lastUpdated, katsuoScoresList->lastUpdated));

    ScoreListClear(&onlineScores);

    const unsigned int sakiScoreCount = sakiScoresList->count;
    const unsigned int katsuoScoreCount = katsuoScoresList->count;
    ScoreListEnsureCapacity(&onlineScores, sakiScoreCount + katsuoScoreCount);

    PDScore *sakiScores = sakiScoresList->scores;
    PDScore *katsuoScores = katsuoScoresList->scores;

    // Fusion des scores
    unsigned int sakiIndex = 0;
    unsigned int katsuoIndex = 0;
    while (sakiIndex < sakiScoreCount && katsuoIndex < katsuoScoreCount) {
        Score score = (Score) {};
        PDScore pdScore;
        if (sakiScores[sakiIndex].value > katsuoScores[katsuoIndex].value) {
            pdScore = sakiScores[sakiIndex++];
            score.character = PlayerCharacterSaki;
        } else {
            pdScore = katsuoScores[katsuoIndex++];
            score.character = PlayerCharacterKatsuo;
        }
        score.super.value = pdScore.value;
        // TODO: Copier les noms en minuscule (et limiter la taille à 15) !!!
        score.super.player = MELStringCopy(pdScore.player);

        score.super.rank = pdScore.rank <= 10 ? onlineScores.count + 1 : pdScore.rank;

        ScoreListPush(&onlineScores, score);
    }
    for (; sakiIndex < sakiScoreCount; sakiIndex++) {
        PDScore pdScore = sakiScores[sakiIndex];
        ScoreListPush(&onlineScores, (Score) {
            .super = {
                .value = pdScore.value,
                .player = MELStringCopy(pdScore.player),
                .rank = pdScore.rank <= 10 ? onlineScores.count + 1 : pdScore.rank,
            },
            .character = PlayerCharacterSaki,
        });
    }
    for (; katsuoIndex < katsuoScoreCount; katsuoIndex++) {
        PDScore pdScore = katsuoScores[katsuoIndex];
        ScoreListPush(&onlineScores, (Score) {
            .super = {
                .value = pdScore.value,
                .player = MELStringCopy(pdScore.player),
                .rank = pdScore.rank <= 10 ? onlineScores.count + 1 : pdScore.rank,
            },
            .character = PlayerCharacterKatsuo,
        });
    }

    // Libération des ressources.
    freeSakiAndKatsuScoresList();

    // NOTE: Retrait de la sauvegarde des scores dans ce thread pour éviter des erreurs de multithreading. Le nom du fichier à ouvrir est un buffer partagé utilisé par la méthode "open". Ce qui peut provoquer l'écriture dans un fichier complètement différent.
}

static void updateWorstScoreForCharacter(volatile PDScoresList * _Nonnull scores, PlayerCharacter character) {
    for (int index = scores->count - 1; index >= 0; index--) {
        const PDScore score = scores->scores[index];
        if (score.rank <= 10) {
            worstScores[character] = score.value;
            return;
        }
    }
}

static void sakiScoresWasDownloaded(PDScoresList * _Nullable scores, const char * _Nullable errorMessage) {
    if (errorMessage) {
        playdate->system->logToConsole("Unable to load online scores for Saki: %s", errorMessage);
    }
    sakiScoresList = scores != NULL ? scores : &emptyScoreList;
    updateWorstScoreForCharacter(sakiScoresList, PlayerCharacterSaki);
    mergeScores();
}

static void katsuoScoresWasDownloaded(PDScoresList * _Nullable scores, const char *errorMessage) {
    if (errorMessage) {
        playdate->system->logToConsole("Unable to load online scores for Katsuo: %s", errorMessage);
    }
    katsuoScoresList = scores != NULL ? scores : &emptyScoreList;
    updateWorstScoreForCharacter(katsuoScoresList, PlayerCharacterKatsuo);
    mergeScores();
}

static void sakiScoreWasUploaded(PDScore * _Nullable score, const char * _Nullable errorMessage) {
#if DEBUG
    playdate->system->logToConsole("Score upload took: %fs", (playdate->system->getCurrentTimeMilliseconds() - requestSentDate) / 1000.0);
#endif
    requestSentDate = 0;
    if (errorMessage) {
        playdate->system->logToConsole("Unable to upload online score for Saki: %s", errorMessage);
    }
    if (score) {
        playdate->system->logToConsole("Saki score was uploaded. Rank %d", score->rank);
        playdate->scoreboards->freeScore(score);
    }
    OnlineScoresLoad();
}

static void katsuoScoreWasUploaded(PDScore * _Nullable score, const char * _Nullable errorMessage) {
#if DEBUG
    playdate->system->logToConsole("Score upload took: %fs", (playdate->system->getCurrentTimeMilliseconds() - requestSentDate) / 1000.0);
#endif
    requestSentDate = 0;
    if (errorMessage) {
        playdate->system->logToConsole("Unable to upload online score for Katsuo: %s", errorMessage);
    }
    if (score) {
        playdate->system->logToConsole("Katsuo score was uploaded. Rank %d", score->rank);
        playdate->scoreboards->freeScore(score);
    }
    OnlineScoresLoad();
}
