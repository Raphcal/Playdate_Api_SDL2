//
//  carddeck.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 18/12/2023.
//

#include "carddeck.h"

void CardDeckDeinit(CardDeck * _Nonnull self) {
    CardListDeinit(&self->cards);
}

static void fillDeck(CardDeck * _Nonnull self) {
    CardListEnsureCapacity(&self->cards, CardDeckTotal);
    Card *cards = self->cards.memory;
    for (Suit suit = 0; suit < SuitCount; suit++) {
        for (Rank rank = 0; rank < RankCount; rank++) {
            cards[suit * RankCount + rank] = (Card) {
                .suit = suit,
                .rank = rank,
            };
        }
    }
    self->cards.count = CardDeckTotal;
}

Card CardDeckPickRandomCard(CardDeck * _Nonnull self) {
    if (self->cards.count == 0) {
        fillDeck(self);
    }
    return CardListRemoveSwap(&self->cards, MELRandomInt(self->cards.count));
}

Card CardDeckPickCard(CardDeck * _Nonnull self, Card card) {
    if (self->cards.count == 0) {
        fillDeck(self);
    }
    return CardListRemoveSwap(&self->cards, CardListIndexOf(self->cards, card));
}
