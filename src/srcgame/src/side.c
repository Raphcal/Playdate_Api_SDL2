//
//  side.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#include "side.h"

#include "board.h"

const float SideOriginLeft[SideCount] = {
    TILE_SIZE,
    LCD_COLUMNS - TILE_SIZE - TILE_SIZE * BOARD_COLUMNS,
    7 * TILE_SIZE,
};
const Side SideOpposite[SideCount] = {
    SideRight,
    SideLeft,
    SideLeft,
};
const Event SideSentChipsEvent[SideCount] = {
    EventLeftSideSentChips,
    EventRightSideSentChips,
    EventCenterSideSentChips,
};
const Event SideLostEvent[SideCount] = {
    EventLeftSideLost,
    EventRightSideLost,
    EventCenterSideLost,
};
const Event SideMayLoseEvent[SideCount] = {
    EventLeftSideMayLose,
    EventRightSideMayLose,
    EventCenterSideMayLose,
};
