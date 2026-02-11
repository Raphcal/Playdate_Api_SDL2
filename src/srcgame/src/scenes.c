//
//  scenes.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 07/11/2024.
//

#include "scenes.h"

#include "gamescene.h"
#include "scoreattackscene.h"
#include "rallyscene.h"

#define kScoreAttackLevel1Speed 1.0f
#define kScoreAttackLevel100Speed 0.1f
#define kScoreAttackFactor 0.1f

float GetGameSpeed(void) {
    int level;
    switch (currentScene->type) {
        case SceneTypeGame:
            return ((GameScene *)currentScene)->speed;
        case SceneTypeRallyGame:
            return ((RallyScene *)currentScene)->speed;
        case SceneTypeScoreAttack:
            level = ((ScoreAttackScene *)currentScene)->level;
            return MELFloatBound(kScoreAttackLevel100Speed,
                 kScoreAttackLevel100Speed + (kScoreAttackLevel1Speed - kScoreAttackLevel100Speed) * (1.0f - log10f(kScoreAttackFactor * level + 1) / log10f(kScoreAttackFactor * SCORE_ATTACK_LEVEL_MAX + 1)), kScoreAttackLevel1Speed);
        default:
            return 1.0f;
    }
}
