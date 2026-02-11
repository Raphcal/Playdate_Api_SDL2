//
//  burstshootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/02/2023.
//

#ifndef burstshootingstyle_h
#define burstshootingstyle_h

#include "shootingstyle.h"
#include "shootingstyledefinition.h"

const ShootingStyleClass * _Nonnull BurstShootingStyleGetClass(void);

void BurstShootingStyleInit(ShootingStyle * _Nonnull self, const ShootingStyleDefinition * _Nonnull definition);

#endif /* burstshootingstyle_h */
