//
//  satellite.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 06/03/2023.
//

#ifndef satellite_h
#define satellite_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull SatelliteConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable SatelliteLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* satellite_h */
