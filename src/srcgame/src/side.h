//
//  side.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#ifndef side_h
#define side_h

#include "common.h"
#include "../lib/eventbus.h"

#define SideCount 3

typedef enum {
    SideLeft,
    SideRight,
    SideCenter,
} Side;

extern const float SideOriginLeft[SideCount];
extern const Side SideOpposite[SideCount];
extern const Event SideSentChipsEvent[SideCount];
extern const Event SideLostEvent[SideCount];
extern const Event SideMayLoseEvent[SideCount];

#endif /* side_h */
