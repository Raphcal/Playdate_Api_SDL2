//
//  shot.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 09/02/2023.
//

#ifndef bullet_h
#define bullet_h

#include "common.h"
#include "../lib/melice.h"

#include "shootingstyledefinition.h"

LCDSprite * _Nonnull BulletConstructor(const ShootingStyleDefinition * _Nonnull definition, MELPoint origin, MELPoint speed);

const MELSpriteClass * _Nonnull BulletGetClass(void);

#endif /* bullet_h */
