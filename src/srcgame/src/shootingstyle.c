//
//  shootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/01/2023.
//

#include "shootingstyle.h"

#include "aimedshootingstyle.h"
#include "burstshootingstyle.h"
#include "circularshootingstyle.h"
#include "simpleshootingstyle.h"
#include "particuleshootingstyle.h"
#include "rocketshootingstyle.h"

static MELPoint shotOrigin(ShotOrigin origin, MELRectangle frame, float angle) {
    switch (origin) {
        case ShotOriginFront:
            return MELRectangleOriginIsCenterGetPointAtAngle(frame, angle);
        case ShotOriginCenter:
            return frame.origin;
        case ShotOriginBack:
            return MELRectangleOriginIsCenterGetPointAtAngle(frame, angle + MEL_PI);
        default:
            playdate->system->error("Bad shot origin: %d", origin);
            return frame.origin;
    }
}

#if ENABLE_SHOOTING_STYLE_INVERSIONS
static void invert(ShootingStyle * _Nonnull self, const ShootingStyleInversion inversions) {
    if (inversions & ShootingStyleInversionAmount) {
        self->bulletAmountVariation = -self->bulletAmountVariation;
    }
}
#endif

void ShootingStyleShootFromSprite(ShootingStyle * _Nonnull self, MELSprite * _Nonnull sprite, float angle) {
    if (!sprite->hitPoints && !self->canShootWhenHitPointsAreZero) {
        // Le sprite n'a plus de points de vie. Pas de tir.
        return;
    }
    const MELTimeInterval delta = DELTA;
    MELTimeInterval shootInterval = self->shootInterval;
    if (shootInterval > 0) {
        shootInterval -= delta;
    } else {
        const ShootingStyleDefinition *definition = self->definition;
        shootInterval += definition->shootInterval;

        MELPoint origin = shotOrigin(definition->origin, sprite->frame, angle);
        const MELPoint translation = definition->translation;
        origin = (MELPoint) {
            .x = origin.x + translation.x,
            .y = origin.y + translation.y
        };

        // Salve de tir
        // TODO: Calculer le décalage initial des tirs en fonction de la valeur de shootInterval.
        self->class->createBullets(self, origin, angle);

        self->bulletAmount += definition->bulletAmountVariation;

#if ENABLE_SHOOTING_STYLE_INVERSIONS
        const ShootingStyleInversion inversions = definition->inversions;
        if (inversions && self->inversionInterval > 0) {
            self->inversionInterval--;
        } else if (inversions) {
            self->inversionInterval = definition->inversionInterval;
            invert(self, inversions);
        }
#endif
    }
    self->shootInterval = shootInterval;
}

void ShootingStyleSave(ShootingStyle * _Nonnull self, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteByte(outputStream, self->class->name);

    MELOutputStreamWriteFloat(outputStream, self->shootInterval);
    MELOutputStreamWriteUInt32(outputStream, self->bulletAmount);
    MELOutputStreamWriteUInt32(outputStream, self->bulletAmountVariation);
    MELOutputStreamWriteUInt32(outputStream, self->inversionInterval);
    MELOutputStreamWriteFloat(outputStream, self->baseAngle);
    MELOutputStreamWriteBoolean(outputStream, self->canShootWhenHitPointsAreZero);
}

ShootingStyle ShootingStyleLoad(MELInputStream * _Nonnull inputStream, const ShootingStyleDefinition * _Nonnull definition) {
    ShootingStyleClassName className = MELInputStreamReadByte(inputStream);

    const float shootInterval = MELInputStreamReadFloat(inputStream);
    const uint32_t bulletAmount = MELInputStreamReadUInt32(inputStream);
    const uint32_t bulletAmountVariation = MELInputStreamReadUInt32(inputStream);
    const uint32_t inversionInterval = MELInputStreamReadUInt32(inputStream);
    const float baseAngle = MELInputStreamReadFloat(inputStream);
    const MELBoolean canShootWhenHitPointsAreZero = MELInputStreamReadBoolean(inputStream);

    return (ShootingStyle) {
        .class = ShootingStyleClassForName(className),
        .definition = definition,
        .shootInterval = shootInterval,
        .bulletAmount = bulletAmount,
        .bulletAmountVariation = bulletAmountVariation,
        .inversionInterval = inversionInterval,
        .baseAngle = baseAngle,
        .canShootWhenHitPointsAreZero = canShootWhenHitPointsAreZero,
    };
}

const ShootingStyleClass * _Nullable ShootingStyleClassForName(ShootingStyleClassName className) {
    switch (className) {
        case ShootingStyleClassNameAimed:
            return AimedShootingStyleGetClass();
        case ShootingStyleClassNameBurst:
            return BurstShootingStyleGetClass();
        case ShootingStyleClassNameCircular:
            return CircularShootingStyleGetClass();
        case ShootingStyleClassNameSimple:
            return SimpleShootingStyleGetClass();
        case ShootingStyleClassNameParticule:
            return ParticuleShootingStyleGetClass();
        case ShootingStyleClassNameRocket:
            return RocketShootingStyleGetClass();
        default:
            playdate->system->error("Unsupported shooting style class: %d", className);
            return NULL;
    }
}
