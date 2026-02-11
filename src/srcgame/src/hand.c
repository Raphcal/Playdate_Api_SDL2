//
//  hand.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/12/2023.
//

#include "hand.h"

#include "board.h"

MELListImplement(Hand);

unsigned int HandGetChipCount(const Hand hand) {
    int multiplier, base;
    switch (hand.type) {
        case HandTypeFlush:
            return hand.count - 4;
        case HandTypeSameKind:
            multiplier = hand.isFlush ? 2 : 1;
            switch (hand.rank) {
                case RankAce:
                    base = BOARD_COLUMNS;
                    break;
                case RankJack:
                case RankQueen:
                case RankKing:
                    base = 4;
                    break;
                default:
                    base = 2;
                    break;
            }
            return (base + hand.count - 3) * multiplier;
        case HandTypeStraight:
            multiplier = hand.isFlush ? 2 : 1;
            return BOARD_COLUMNS * multiplier * (1.0f + (float)hand.rank/(RankCount * 2.0f)) + (hand.count - 5);
    }
    playdate->system->logToConsole("Unsupported hand type: %d", hand.type);
    return 0;
}
