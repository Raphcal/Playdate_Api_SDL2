//
//  card.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 19/12/2023.
//

#ifndef card_h
#define card_h

#include "common.h"
#include "../lib/melice.h"

#define SuitCount 4
typedef enum {
    SuitClub, SuitHeart, SuitDiamond, SuitSpade
} Suit;
extern const char SuitCharacter[SuitCount];

#define RankCount 8
typedef enum {
    RankSeven, RankEight, RankNine, RankTen, RankJack, RankQueen, RankKing, RankAce
} Rank;
extern const char RankCharacter[RankCount];

typedef struct {
    Suit suit;
    Rank rank;
} Card;

MELListDefine(Card);
MELListDefineIndexOf(Card);

LCDBitmap * _Nonnull CardGetBitmap(Card card);
MELBoolean CardEquals(Card lhs, Card rhs);
Card CardForCharacter(const char c);
char CardToCharacter(Card card);

CardList CardListMakeWithRandomCards(int count);

#endif /* card_h */
