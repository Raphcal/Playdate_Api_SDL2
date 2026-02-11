//
//  particuleshootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/04/2023.
//

#include "particuleshootingstyle.h"

#include "explosion.h"

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle);

static const ShootingStyleClass ParticuleShootingStyleClass = (ShootingStyleClass) {
    .name = ShootingStyleClassNameParticule,
    .createBullets = createBullets,
};

const ShootingStyleClass * _Nonnull ParticuleShootingStyleGetClass(void) {
    return &ParticuleShootingStyleClass;
}

void ParticuleShootingStyleInit(ShootingStyle * _Nonnull self, const ShootingStyleDefinition * _Nonnull definition) {
    *self = (ShootingStyle) {
        .class = &ParticuleShootingStyleClass,
        .definition = definition,
        .shootInterval = MELRandomFloat(definition->shootInterval),
        .bulletAmount = definition->bulletAmount,
        .bulletAmountVariation = definition->bulletAmountVariation,
        .inversionInterval = definition->inversionInterval,
        .canShootWhenHitPointsAreZero = true,
    };
}

static void createBullets(ShootingStyle * _Nonnull self, MELPoint origin, float angle) {
    const ShootingStyleDefinition *definition = self->definition;
    const float space = definition->space;
    ExplosionConstructorWithDefinition((MELPoint) {
        .x = origin.x + MELRandomFloat(space) - space / 2,
        .y = origin.y + MELRandomFloat(space) - space / 2,
    }, definition->bulletDefinition);
}
