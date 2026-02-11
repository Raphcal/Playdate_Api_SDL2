//
//  map_player.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 04/04/2024.
//

#ifndef map_player_h
#define map_player_h

#include "common.h"
#include "../lib/melice.h"

#include "character.h"

LCDSprite * _Nonnull MapPlayerConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);
LCDSprite * _Nonnull MapPlayerConstructorWithCharacterAndFromAndTo(Character character, MELPoint from, MELPoint to);

#endif /* map_player_h */
