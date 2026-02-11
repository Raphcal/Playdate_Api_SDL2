//
//  circularshootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/02/2023.
//

#ifndef circularshootingstyle_h
#define circularshootingstyle_h

#include "shootingstyle.h"
#include "shootingstyledefinition.h"

typedef struct {
    ShootingStyleDefinition super;

    /// Angle du premier tir.
    float baseAngle;

    /// Variation de l'angle de départ.
    float baseAngleVariation;

    /// Différence d'angle entre 2 tirs.
    float angleIncrement;
} CircularShootingStyleDefinition;

const ShootingStyleClass * _Nonnull CircularShootingStyleGetClass(void);

void CircularShootingStyleInit(ShootingStyle * _Nonnull self, const CircularShootingStyleDefinition * _Nonnull definition);

#endif /* circularshootingstyle_h */
