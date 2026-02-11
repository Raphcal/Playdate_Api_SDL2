//
//  gunner_bot.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/02/2023.
//

#ifndef gunner_bot_h
#define gunner_bot_h

#include "common.h"
#include "../lib/melice.h"

#include "player.h"

LCDSprite * _Nonnull GunnerBotConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);
LCDSprite * _Nonnull GunnerBotConstructorWithOrigin(MELPoint origin);

MELSprite * _Nullable GunnerBotLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

void GunnerBotCatch(LCDSprite * _Nonnull sprite, Player * _Nonnull parent);
void GunnerBotThrow(LCDSprite * _Nonnull sprite, float angle, float speed, MELBoolean wasHit);

#endif /* gunner_bot_h */
