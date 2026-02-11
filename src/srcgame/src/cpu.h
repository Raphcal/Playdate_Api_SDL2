//
//  cpu.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 29/12/2023.
//

#ifndef cpu_h
#define cpu_h

#include "common.h"
#include "../lib/melice.h"

#include "actor.h"
#include "handidentifier.h"

/**
 * Structure de base pour gérer les adversaires.
 */
typedef struct {
    Actor super;
    float timeBetweenButtonPress;
    float time;
    MELIntPoint target;
    MELDirection targetDirection;
    MELBoolean isPressingDown;
    MELBoolean willPressDownWhenOverTarget;
} Cpu;

Actor * _Nonnull CpuAlloc(Actor * _Nonnull actor);
void CpuUpdate(Actor * _Nonnull actor);

#endif /* cpu_h */
