//
//  boss_satellite_aux.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/03/2023.
//

#ifndef boss_satellite_aux_h
#define boss_satellite_aux_h

#include "common.h"
#include "../lib/melice.h"

typedef struct boss_satellite BossSatellite;

LCDSprite * _Nonnull BossSatelliteAuxConstructor(BossSatellite * _Nullable parent, float angle, float distance, int score);

MELSprite * _Nullable BossSatelliteAuxLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

const MELSpriteClass * _Nonnull BossSatelliteAuxGetClass(void);

#endif /* boss_satellite_aux_h */
