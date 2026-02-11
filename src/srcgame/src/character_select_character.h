//
//  character_select_character.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/08/2023.
//

#ifndef character_select_character_h
#define character_select_character_h

#include "common.h"
#include "player.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull CharacterSelectCharacterConstructor(PlayerCharacter character, PlayerCharacter * _Nonnull selection, LCDBitmapTable * _Nonnull table);

#endif /* character_select_character_h */
