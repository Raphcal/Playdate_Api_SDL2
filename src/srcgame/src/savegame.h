//
//  savegame.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/11/2023.
//

#ifndef savegame_h
#define savegame_h

#include "common.h"
#include "../lib/melice.h"

#include "story.h"
#include "achievements.h"
#include "rally.h"

typedef struct melscene MELScene;

#define kFileNameLength 11
#define kFileNameNumberIndex 4
#define kSaveGameNameLength 10
#define kNoSaveSelected 0xFF
#define kMaximumSaveCount 5
#define kScoreAttackHistoryCount 20

typedef struct {
    /**
     * Identifiant de la sauvegarde.
     */
    MELUUID uuid;
    /**
     * Numéro du fichier de sauvegarde.
     */
    uint8_t index;
    /**
     * Carte représentant la sauvegarde.
     */
    Card card;
    /**
     * Nom du joueur.
     * @deprecated
     */
    char name[kSaveGameNameLength + 1];
    /**
     * Durée de jeu en secondes.
     */
    float playtime;
    /**
     * Langue.
     */
    MELLanguage language;
    /**
     * Dernier personnage joué.
     */
    Character lastCharacter;
    /**
     * Indique si le scénario est terminé.
     */
    uint32_t completion[kStoryCount];
    /**
     * Numéro du dernier adversaire atteint pour chaque histoire.
     */
    int8_t progression[kStoryCount];
    /**
     * Champ de bits pour les cours.
     */
    uint32_t schoolProgression;
    /**
     * Nombre de victoires.
     */
    uint16_t wins;
    /**
     * Nombre de défaites.
     */
    uint16_t loses;
    /**
     * Nombre de parties en mode score attack.
     */
    uint16_t scoreAttackRun;
    /**
     * Meilleur score personnel.
     */
    uint32_t scoreAttackPersonalBest;
    /**
     * Scores des dernières parties.
     */
    uint32_t scoreAttackLastScores[kScoreAttackHistoryCount];
    /**
     * Nombre de parties en mode score attack.
     */
    uint16_t rallyRun;
    /**
     * Meilleur dernier stage atteint.
     */
    uint8_t rallyBestStage;
    /**
     * Meilleur score personnel.
     */
    uint32_t rallyPersonalBest;
    /**
     * Scores des dernières parties.
     */
    uint32_t rallyLastScores[kScoreAttackHistoryCount];
    /**
     * Partie arcade en cours.
     */
    ArcadeSaveState arcadeSaveState;
    /**
     * Nombre total de cartes escamotées.
     */
    uint32_t removedCards;
    /**
     * Nombre total de jetons escamotées.
     */
    uint32_t removedChips;
    /**
     * Nombre total de jetons envoyés.
     */
    uint32_t sentChips;
    /**
     * Nombre de fois que le joueur à fait chaque main.
     */
    uint32_t handCount[3];
    /**
     * Nombre maximum de combo.
     */
    uint8_t bestCombo;
    /**
     * Date de déblocage des succès.
     */
    uint32_t achievementUnlockDate[AchievementCount];
} SaveGame;

MELListDefine(SaveGame);

extern SaveGame currentSaveGame;

void SaveGameCreate(SaveGameList * _Nonnull saveGames);
void SaveGameToDisk(SaveGame * _Nonnull self);
void SavedGameLoad(SaveGameList * _Nonnull self);
SaveGame SavedGameLoadFirst(void);
void SaveGameDelete(SaveGameList * _Nonnull self, unsigned int indexToDelete);

MELBoolean isLessonDone(uint8_t index);
void markLessonDone(uint8_t index);

void unlockAchievement(Achievement achievement);
void unlockAchievementAt(Achievement achievement, uint32_t date);

#endif /* savegame_h */
