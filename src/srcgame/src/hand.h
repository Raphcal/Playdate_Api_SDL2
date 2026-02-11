//
//  hand.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/12/2023.
//

#ifndef hand_h
#define hand_h

#include "common.h"

#include "card.h"

typedef enum {
    HandTypeFlush,
    HandTypeSameKind,
    HandTypeStraight,
} HandType;

typedef struct {
    HandType type;
    Suit suit;
    Rank rank;
    unsigned int count;
    MELBoolean isFlush;
} Hand;

MELListDefine(Hand);

unsigned int HandGetChipCount(const Hand hand);

#endif /* hand_h */
