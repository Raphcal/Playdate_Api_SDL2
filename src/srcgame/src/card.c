//
//  card.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 19/12/2023.
//

#include "card.h"

#include "carddeck.h"
#include "../gen/spritecards18.h"

MELListImplement(Card);
MELListImplementIndexOfWithEqualsFunction(Card, CardEquals);

const char SuitCharacter[SuitCount] = {'C', 'H', 'D', 'S'};
const char RankCharacter[RankCount] = {'7', '8', '9', 'X', 'J', 'Q', 'K', 'A'};

LCDBitmap * _Nonnull CardGetBitmap(Card card) {
    loadSpriteCards18Palette();
    return playdate->graphics->getTableBitmap(spriteCards18.palette, card.suit * RankCount + card.rank);
}

MELBoolean CardEquals(Card lhs, Card rhs) {
    return lhs.rank == rhs.rank && lhs.suit == rhs.suit;
}

Card CardForCharacter(const char cardChar) {
    switch (cardChar) {
        case 'a'...'h':
            return (Card) {
                .suit = SuitClub,
                .rank = (Rank)(cardChar - 'a'),
            };
        case 'i'...'p':
            return (Card) {
                .suit = SuitSpade,
                .rank = (Rank)(cardChar - 'i'),
            };
        case 'A'...'H':
            return (Card) {
                .suit = SuitHeart,
                .rank = (Rank)(cardChar - 'A'),
            };
        case 'I'...'P':
            return (Card) {
                .suit = SuitDiamond,
                .rank = (Rank)(cardChar - 'I'),
            };
        default:
            playdate->system->error("Unable to parse card for char: %c", cardChar);
            return (Card) {};
    }
}

char CardToCharacter(Card card) {
    switch (card.suit) {
        case SuitClub:
            return 'a' + card.rank;
        case SuitSpade:
            return 'i' + card.rank;
        case SuitHeart:
            return 'A' + card.rank;
        case SuitDiamond:
            return 'I' + card.rank;
        default:
            playdate->system->error("Unable to convert card to char: suit: %d, rank: %d", card.suit, card.rank);
            return '_';
    }
}

CardList CardListMakeWithRandomCards(const int count) {
    CardList cards = CardListMakeWithInitialCapacity(count);
    CardDeck deck = (CardDeck) {};
    for (unsigned int index = 0; index < cards.capacity; index++) {
        cards.memory[index] = CardDeckPickRandomCard(&deck);
    }
    cards.count = count;
    CardDeckDeinit(&deck);
    return cards;
}
