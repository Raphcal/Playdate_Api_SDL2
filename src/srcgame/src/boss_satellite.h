//
//  boss_satellite.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 14/03/2023.
//

#ifndef boss_satellite_h
#define boss_satellite_h

#include "common.h"
#include "../lib/melice.h"

#include "shootingstyle.h"

enum boss_satellite_state {
    init,
    entering,
    reentering,
    main,
    goFar,
    goNear,
    thrust,
    reverse,
    moving,
    destroyed,
    exploding,
};

typedef struct boss_satellite {
    MELSprite super;
    LCDSprite * _Nullable lifeBar;

    enum boss_satellite_state state;
    uint8_t statePart;
    uint8_t auxiliaries;
    float auxDistanceMultiplier;
    float auxSpeed;
    float baseSpeed;
    MELPoint speed;
    float strideDecimalFrom0;
    float strideDecimalTo0;
    MELPoint stridePointFrom0;
    MELPoint stridePointTo0;
    float time;
    uint8_t visibleAuxiliaries;
    ShootingStyle smoke;

    LCDSprite * _Nullable explosion;
} BossSatellite;

LCDSprite * _Nonnull BossSatelliteConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable BossSatelliteLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);


#endif /* boss_satellite_h */
