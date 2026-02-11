//
//  simpleshootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 21/03/2023.
//

#include "simpleshootingstyle.h"

#include "bullet.h"

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle);

static const ShootingStyleClass SimpleShootingStyleClass = (ShootingStyleClass) {
    .name = ShootingStyleClassNameSimple,
    .createBullets = createBullets,
};

const ShootingStyleClass * _Nonnull SimpleShootingStyleGetClass(void) {
    return &SimpleShootingStyleClass;
}

void SimpleShootingStyleInit(ShootingStyle * _Nonnull self, const ShootingStyleDefinition * _Nonnull definition) {
    *self = (ShootingStyle) {
        .class = &SimpleShootingStyleClass,
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
    BulletConstructor(definition, origin, (MELPoint) {
        .x = bulletSpeed * cosf(angle),
        .y = bulletSpeed * sinf(angle)
    });
}
