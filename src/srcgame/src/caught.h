//
//  caught.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 30/01/2023.
//

#ifndef caught_h
#define caught_h

#include "common.h"
#include "../lib/melice.h"

#include "player.h"

void catchSprite(LCDSprite * _Nonnull sprite, Player * _Nonnull parent);
void releaseSprite(MELSprite * _Nonnull self);
MELBoolean isCaught(MELSprite * _Nonnull self);
const MELSpriteClass * _Nonnull CaughtGetClass(void);

#endif /* caught_h */
