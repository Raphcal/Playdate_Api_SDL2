//
//  punch.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 12/09/2023.
//

#ifndef punch_h
#define punch_h

#include "common.h"
#include "../lib/melice.h"

typedef enum {
    Idle,
    CanCharge,
    Punching,
    ComingBack
} PunchState;

typedef struct {
    MELSubSprite super;
    MELPoint * _Nullable playerOrigin;
    MELPoint offset;
    float prepareOffset;
    float time;
    float speed;
    float distance;
    PunchState state;
    PunchState * _Nullable otherPunchState;
} Punch;

Punch * _Nonnull PunchAlloc(LCDSprite * _Nonnull parent, MELBoolean isRightHand);
void PunchDidLand(Punch * _Nonnull self);

#endif /* punch_h */
