//
//  achievements.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 25/03/2025.
//

#include "common.h"

#ifndef achievements_h
#define achievements_h

#include "../lib/achievements.h"

typedef enum achievement {
    AchievementStoryCeresEnd,
    AchievementStoryCarolineEnd,
    AchievementStoryClovisEnd,
    AchievementStoryBelleplumeEnd,
    AchievementStoryCrocofordEnd,
    AchievementStoryMelusineEnd,
    AchievementScoreAttackFirst,
    AchievementScoreAttack100000,
    AchievementScoreAttack500000,
    AchievementArcadeFirst,
    AchievementArcadeSpeedUp,
    AchievementArcadeEnd,
    AchievementGameVictory1,
    AchievementGameVictory20,
    AchievementGameDefeat1,
    AchievementChainX2,
    AchievementChainX4,
    AchievementChainStar,
    AchievementJackpot,
    AchievementFiveOfAKind,
    AchievementRoyalStraight,
    AchievementReversal,
    AchievementAllClear,
    AchievementCount
} Achievement;

extern const MELAchievementData kAchievementData;

#endif /* achievements_h */
