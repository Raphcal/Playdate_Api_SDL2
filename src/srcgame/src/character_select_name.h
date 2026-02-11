//
//  character_select_name.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/08/2023.
//

#ifndef character_select_name_h
#define character_select_name_h

#include "common.h"
#include "player.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull CharacterSelectNameConstructor(PlayerCharacter character, PlayerCharacter * _Nonnull selection, LCDBitmap * _Nonnull image);

#endif /* character_select_name_h */
