//
//  burstshootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/02/2023.
//

#include "burstshootingstyle.h"

#include "bullet.h"

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle);

static const ShootingStyleClass BurstShootingStyleClass = (ShootingStyleClass) {
    .name = ShootingStyleClassNameBurst,
    .createBullets = createBullets,
};

const ShootingStyleClass * _Nonnull BurstShootingStyleGetClass(void) {
    return &BurstShootingStyleClass;
}

void BurstShootingStyleInit(ShootingStyle * _Nonnull self, const ShootingStyleDefinition * _Nonnull definition) {
    *self = (ShootingStyle) {
        .class = &BurstShootingStyleClass,
        .definition = definition,
        .shootInterval = MELRandomFloat(definition->shootInterval),
        .bulletAmount = definition->bulletAmount,
        .bulletAmountVariation = definition->bulletAmountVariation,
        .inversionInterval = definition->inversionInterval,
    };
}

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle) {
    const ShootingStyleDefinition *definition = self->definition;

    const float bulletSpeed = definition->bulletSpeed;
    const unsigned int bulletAmount = self->bulletAmount;
    for (unsigned int index = 0; index < bulletAmount; index++) {
        const float bulletAngle = angle + MELRandomFloat(0.1f) - 0.05f;
        const MELPoint speed = (MELPoint) {
            .x = cosf(bulletAngle) * bulletSpeed,
            .y = sinf(bulletAngle) * bulletSpeed
        };
        BulletConstructor(definition, origin, speed);
    }
}
