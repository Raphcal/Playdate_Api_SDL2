//
//  references.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 19/12/2023.
//

#include "references.h"

MELListImplement(MELIntPoint);

MELIntPoint MELIntPointListCenter(MELIntPointList points) {
    if (points.count == 0) {
        return MELIntPointZero;
    }
    int x = 0;
    int y = 0;

    for (int index = 0; index < points.count; index++) {
        MELIntPoint point = points.memory[index];
        x += point.x;
        y += point.y;
    }
    
    MELIntPoint result = (MELIntPoint) {
        .x = ((float)x / points.count),
        .y = ((float)y / points.count)
    };
    return result;
}
