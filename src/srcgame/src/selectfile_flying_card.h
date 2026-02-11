//
//  selectfile_flying_card.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 07/03/2024.
//

#ifndef selectfile_flying_card_h
#define selectfile_flying_card_h

#include "common.h"
#include "../lib/melice.h"

#include "savegame.h"

LCDSprite * _Nonnull SelectFileFlyingCardConstructor(SaveGameList * _Nonnull saveGames, int8_t * _Nonnull selectedSaveGame);
void SelectFileFlyingCardSetSaveGamesAndSelectedGame(LCDSprite * _Nonnull sprite, SaveGameList * _Nonnull saveGames, int8_t * _Nonnull selectedSaveGame);

#endif /* selectfile_flying_card_h */
