//
//  scores.h
//  Poker Poker Magic
//
//  Created by Raphaël Calabro on 18/06/2023.
//

#ifndef scores_h
#define scores_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    PDScore super;
    MELUUID playerUUID;
} Score;

MELListDefine(Score);

typedef enum {
    ScoreBoardScoreAttack,
    ScoreBoardRally,
} ScoreBoard;

extern ScoreList scoreAttackScores;
extern ScoreList rallyScores;
extern uint32_t scoreLastUpdateDate;
extern char * _Nullable scoreAttackLastUpdateError;
extern char * _Nullable rallyLastUpdateError;

void ScoresAdd(ScoreBoard scoreBoard, uint32_t score);

void LocalScoresLoad(void);
void LocalScoresSave(void);

void OnlineScoresLoadAll(void);
void OnlineScoresLoadScoreAttack(void);
void OnlineScoresLoadRally(void);

void OnlineScoresLoad(void);
void OnlineScoresAddScoreAttackScore(uint32_t score);
void OnlineScoresAddRallyScore(uint32_t score);

MELBoolean OnlineScoresScoreAttackIsLoading(void);
MELBoolean OnlineScoresRallyIsLoading(void);
MELBoolean OnlineScoresAreLoading(void);

/**
 * Cherche l'indice où insérer le score du joueur.
 *
 * @param scores Tableau des scores triés du plus grand au plus petit.
 * @param userScore Score de l'utilisateur.
 * @returns `count` si le joueur n'est pas dans le classement, l'indice où insérer son score sinon.
 */
int ScoreListFindRank(const ScoreList scores, uint32_t userScore);

#endif /* scores_h */
