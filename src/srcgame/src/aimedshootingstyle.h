//
//  aimedshootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/02/2023.
//

#ifndef aimedshootingstyle_h
#define aimedshootingstyle_h

#include "shootingstyle.h"
#include "shootingstyledefinition.h"

const ShootingStyleClass * _Nonnull AimedShootingStyleGetClass(void);

void AimedShootingStyleInit(ShootingStyle * _Nonnull self, const ShootingStyleDefinition * _Nonnull definition);

#endif /* aimedshootingstyle_h */
