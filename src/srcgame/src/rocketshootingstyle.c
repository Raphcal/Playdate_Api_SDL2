//
//  rocketshootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/04/2023.
//

#include "rocketshootingstyle.h"

#include "rocket.h"

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle);

static const ShootingStyleClass RocketShootingStyleClass = (ShootingStyleClass) {
    .name = ShootingStyleClassNameRocket,
    .createBullets = createBullets,
};

const ShootingStyleClass * _Nonnull RocketShootingStyleGetClass(void) {
    return &RocketShootingStyleClass;
}

void RocketShootingStyleInit(ShootingStyle * _Nonnull self, const ShootingStyleDefinition * _Nonnull definition) {
    *self = (ShootingStyle) {
        .class = &RocketShootingStyleClass,
        .definition = definition,
        .shootInterval = MELRandomFloat(definition->shootInterval),
        .bulletAmount = definition->bulletAmount,
        .bulletAmountVariation = definition->bulletAmountVariation,
        .inversionInterval = definition->inversionInterval,
    };
}

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle) {
    RocketConstructor(self->definition, origin, angle);
}
