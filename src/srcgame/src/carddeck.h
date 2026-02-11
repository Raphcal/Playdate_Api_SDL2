//
//  carddeck.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 18/12/2023.
//

#ifndef carddeck_h
#define carddeck_h

#include "common.h"

#include "card.h"

#define CardDeckTotal (SuitCount * RankCount)
typedef struct {
    CardList cards;
} CardDeck;

void CardDeckDeinit(CardDeck * _Nonnull self);
Card CardDeckPickRandomCard(CardDeck * _Nonnull self);
Card CardDeckPickCard(CardDeck * _Nonnull self, Card card);

#endif /* carddeck_h */
