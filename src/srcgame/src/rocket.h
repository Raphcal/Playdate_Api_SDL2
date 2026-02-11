//
//  rocket.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/04/2023.
//

#ifndef rocket_h
#define rocket_h

#include "common.h"
#include "../lib/melice.h"

#include "shootingstyledefinition.h"

LCDSprite * _Nullable RocketConstructor(const ShootingStyleDefinition * _Nonnull definition, MELPoint origin, float angle);
const MELSpriteClass * _Nonnull RocketGetClass(void);

#endif /* rocket_h */
