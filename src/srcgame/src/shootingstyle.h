//
//  shootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/01/2023.
//

#ifndef shootingstyle_h
#define shootingstyle_h

#include "shootingstyledefinition.h"

typedef enum {
    ShootingStyleClassNameAimed,
    ShootingStyleClassNameBurst,
    ShootingStyleClassNameCircular,
    ShootingStyleClassNameSimple,
    ShootingStyleClassNameParticule,
    ShootingStyleClassNameRocket,
} ShootingStyleClassName;

typedef struct {
    ShootingStyleClassName name;
    void (* _Nonnull createBullets)(ShootingStyle * _Nonnull self, MELPoint origin, float angle);
} ShootingStyleClass;

const ShootingStyleClass * _Nullable ShootingStyleClassForName(ShootingStyleClassName className);


typedef struct shootingstyle {
    const ShootingStyleClass * _Nonnull class;
    const ShootingStyleDefinition * _Nonnull definition;

    MELTimeInterval shootInterval;
    unsigned int bulletAmount;
    unsigned int bulletAmountVariation;

    unsigned int inversionInterval;

    float baseAngle;

    MELBoolean canShootWhenHitPointsAreZero;
} ShootingStyle;

void ShootingStyleShootFromSprite(ShootingStyle * _Nonnull self, MELSprite * _Nonnull sprite, float angle);

void ShootingStyleSave(ShootingStyle * _Nonnull self, MELOutputStream * _Nonnull outputStream);
ShootingStyle ShootingStyleLoad(MELInputStream * _Nonnull inputStream, const ShootingStyleDefinition * _Nonnull definition);

#endif /* shootingstyle_h */
