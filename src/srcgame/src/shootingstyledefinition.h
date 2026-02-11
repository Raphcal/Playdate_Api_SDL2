//
//  shootingstyledefinition.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/01/2023.
//

#ifndef shootingstyledefinition_h
#define shootingstyledefinition_h

#include "common.h"
#include "../lib/melice.h"

// TODO: Retirer les inversions inutilisées à la fin.
typedef enum {
    ShootingStyleNone            =       0,
    ShootingStyleInversionX      =       1,
    ShootingStyleInversionY      =    0b10,
    ShootingStyleInversionAim    =   0b100,
    ShootingStyleInversionAngle  =  0b1000,
    ShootingStyleInversionAmount = 0b10000,
} ShootingStyleInversion;

typedef enum {
    ShotOriginCenter,
    ShotOriginFront,
    ShotOriginBack,
} ShotOrigin;

typedef struct shootingstyle ShootingStyle;
typedef struct shootingstyledefinition ShootingStyleDefinition;

typedef struct shootingstyledefinition {
    /// Point d'origine des tirs
    ShotOrigin origin;

    // Déplacement de l'origine du tir.
    MELPoint translation;

    /// Dommages provoqués par chaque tir
    int damage;

    /// Définition des sprites tirés.
    MELSpriteDefinition * _Nonnull bulletDefinition;

    /// Nom de l'animation à utiliser.
    AnimationName bulletAnimationName;

    /// Nombre de tirs
    int bulletAmount;
    /// Augmentation ou diminution du nombre de tirs
    int bulletAmountVariation;

    /// Vitesse d'un tir
    float bulletSpeed;
    /// Interval de temps entre chaque tir
    MELTimeInterval shootInterval;

    /// Nombre d'inversions
    ShootingStyleInversion inversions;
    /// Nombre de tirs avant l'inversion
    int inversionInterval;

    // Espace entre les tirs.
    float space;
} ShootingStyleDefinition;

#endif /* shootingstyledefinition_h */
