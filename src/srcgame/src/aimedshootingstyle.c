//
//  aimedshootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/02/2023.
//

#include "aimedshootingstyle.h"

#include "gamescene.h"
#include "bullet.h"

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle);

static const ShootingStyleClass AimedShootingStyleClass = (ShootingStyleClass) {
    .name = ShootingStyleClassNameAimed,
    .createBullets = createBullets,
};

const ShootingStyleClass * _Nonnull AimedShootingStyleGetClass(void) {
    return &AimedShootingStyleClass;
}

void AimedShootingStyleInit(ShootingStyle * _Nonnull self, const ShootingStyleDefinition * _Nonnull definition) {
    *self = (ShootingStyle) {
        .class = &AimedShootingStyleClass,
        .definition = definition,
        .shootInterval = MELRandomFloat(definition->shootInterval),
        .bulletAmount = definition->bulletAmount,
        .bulletAmountVariation = definition->bulletAmountVariation,
        .inversionInterval = definition->inversionInterval,
    };
}

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle) {
    const ShootingStyleDefinition *definition = self->definition;

    if (currentScene->type != SceneTypeGame) {
        return;
    }
    GameScene *gameScene = GameSceneGetCurrent();
    Player *player = gameScene->player;

    const float bulletSpeed = definition->bulletSpeed;
    const unsigned int bulletAmount = self->bulletAmount;
    for (unsigned int index = 0; index < bulletAmount; index++) {
        const float angleToTarget = MELPointAngleToPoint(player->super.frame.origin, origin);

        const MELPoint speed = (MELPoint) {
            .x = cosf(angleToTarget) * bulletSpeed,
            .y = sinf(angleToTarget) * bulletSpeed
        };
        BulletConstructor(definition, origin, speed);
    }
}
