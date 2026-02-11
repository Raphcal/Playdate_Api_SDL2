//
//  boardlocation.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 25/12/2023.
//

#include "boardlocation.h"

#include "board.h"

int MELIntPointBoardIndex(MELIntPoint point) {
    return point.y * BOARD_COLUMNS + point.x;
}
