//
//  character_select_controls.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/02/2024.
//

#ifndef character_select_controls_h
#define character_select_controls_h

#include "common.h"
#include "player.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull CharacterSelectControlsConstructor(PlayerCharacter character, PlayerCharacter * _Nonnull selection, LCDBitmap * _Nonnull image);

#endif /* character_select_controls_h */
