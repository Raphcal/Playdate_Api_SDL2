//
//  thrown.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/01/2023.
//

#ifndef thrown_h
#define thrown_h

#include "common.h"

#include "player.h"

typedef struct thrown Thrown;

Thrown * _Nonnull throwSprite(LCDSprite * _Nonnull sprite, Player * _Nullable player, float angle, float speed);
const MELSpriteClass * _Nonnull ThrownGetClass(void);

#endif /* thrown_h */
