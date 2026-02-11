//
//  character_select_crank.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/08/2023.
//

#ifndef character_select_crank_h
#define character_select_crank_h

#include "common.h"
#include "player.h"
#include "../lib/melice.h"

/**
 * @discussion Cette méthode ajoute le sprite à la liste des sprites de la scène.
 */
LCDSprite * _Nonnull CharacterSelectCrankConstructorWithSelection(PlayerCharacter * _Nonnull selection);

LCDSprite * _Nonnull CharacterSelectCrankConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);
MELSprite * _Nullable CharacterSelectCrankLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* character_select_crank_h */
