//
//  selectfile_card.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/03/2024.
//

#ifndef selectfile_card_h
#define selectfile_card_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull SelectFileCardConstructor(MELRectangle frame, int8_t saveIndex, int8_t saveCount, int8_t * _Nonnull selectedSaveGame);
void SelectFileCardsSlide(void);
void SelectFileCardsReset(void);
void SelectFileCardsSkipAnimation(LCDSprite * _Nonnull sprite);

const MELSpriteClass * _Nonnull SelectFileCardsGetClass(void);

#endif /* selectfile_card_h */
