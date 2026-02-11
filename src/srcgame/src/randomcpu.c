//
//  randomcpu.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 23/05/2024.
//

#include "randomcpu.h"

Actor * _Nonnull RandomCpuMake(void) {
    Cpu *self = playdate->system->realloc(NULL, sizeof(Cpu));
    *self = (Cpu) {
        .super = {
            .dealloc = ActorDealloc,
            .update = CpuUpdate,
            .handChanged = RandomCpuHandChanged,
        }
    };
    return &self->super;
}

void RandomCpuHandChanged(Actor * _Nonnull actor, Card * _Nonnull hand, Card * _Nonnull nextHand) {
    Cpu *self = (Cpu *)actor;
    self->target = (MELIntPoint) {
        .x = MELRandomInt(BOARD_COLUMNS),
    };
    self->targetDirection = MELDirectionCircle[MELRandomInt(DIRECTION_COUNT)];
}
