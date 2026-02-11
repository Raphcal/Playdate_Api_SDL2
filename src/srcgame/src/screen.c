//
//  screen.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/04/2023.
//

#include "screen.h"

const MELRectangle kScreen = (MELRectangle) {
    .origin = {
        .x = LCD_COLUMNS / 2,
        .y = LCD_ROWS / 2,
    },
    .size = {
        .width = LCD_COLUMNS,
        .height = LCD_ROWS,
    }
};
