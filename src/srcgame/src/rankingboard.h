//
//  ranking_board.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 18/06/2023.
//

#ifndef rankingboard_h
#define rankingboard_h

#include "common.h"

#include "player.h"

typedef enum {
    RankNone,
    RankF,
    RankE,
    RankD,
    RankC,
    RankB,
    RankA,
    RankS,
    RankChouS,
} Rank;
#define RankMaxValue 7

typedef struct {
    Rank lives;
    Rank timeLeft;
    Rank combo;
} Ranks;

#define kRankCount 3

typedef struct {
    PDScore super;
    PlayerCharacter character;
    Ranks ranks;
} Score;

MELListDefine(Score);

extern ScoreList localScores;
extern ScoreList onlineScores;
extern uint32_t scoreLastUpdateDate;
extern uint32_t localBestScore;

/**
 * Scores à battre pour être dans le top en ligne.
 */
extern uint32_t worstScores[kPlayerCharacterCount];

/**
 * Meilleurs rangs atteints par le joueur. Les rangs de 0 à 2 sont pour Katsuo et les suivants pour Saki.
 * L'ordre est : vies restantes, meilleur temps restant, max hits.
 *
 * Une valeur de 0 masque la lettre du rang. Les valeurs de 1 à 6 sont F à A, 7 vaut S.
 */
extern Ranks bestRanks[kPlayerCharacterCount];
#define kRankLives 0
#define kRankTimeLeft 1
#define kRankMaxCombo 2

void LocalScoresLoad(void);
Score LocalScoresAdd(Score score);
void LocalScoresSave(void);

void OnlineScoresLoad(void);
void OnlineScoresAdd(PlayerCharacter character, uint32_t score);
MELBoolean OnlineScoresAreLoading(void);

/**
 * Cherche l'indice où insérer le score du joueur.
 *
 * @param scores Tableau des scores triés du plus grand au plus petit.
 * @param userScore Score de l'utilisateur.
 * @returns `count` si le joueur n'est pas dans le classement, l'indice où insérer son score sinon.
 */
int ScoreListFindRank(ScoreList scores, unsigned int userScore);

#endif /* rankingboard_h */
