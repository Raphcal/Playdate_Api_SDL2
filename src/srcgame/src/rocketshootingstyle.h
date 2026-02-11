//
//  rocketshootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/04/2023.
//

#ifndef rocketshootingstyle_h
#define rocketshootingstyle_h

#include "shootingstyle.h"
#include "shootingstyledefinition.h"

const ShootingStyleClass * _Nonnull RocketShootingStyleGetClass(void);

void RocketShootingStyleInit(ShootingStyle * _Nonnull self, const ShootingStyleDefinition * _Nonnull definition);

#endif /* rocketshootingstyle_h */
