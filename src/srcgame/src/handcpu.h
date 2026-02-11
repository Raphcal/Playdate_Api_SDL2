//
//  handcpu.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 23/05/2024.
//

#ifndef handcpu_h
#define handcpu_h

#include "cpu.h"

/**
 * Adversaire qui place ses tuiles en fonction du nombre de jetons que cela va envoyer à l'adversaire.
 */
typedef struct {
    Cpu super;
    MELBoolean isAlwaysFast;
    MELBoolean isFastWhenGoodHandIsFound;
    HandIdentifier * _Nullable identifier;
} HandCpu;

Actor * _Nonnull HandCpuAlloc(Actor * _Nonnull actor);
void HandCpuDealloc(Actor * _Nonnull self);
void HandCpuHandChanged(Actor * _Nonnull self, Card * _Nonnull hand, Card * _Nonnull nextHand);

#endif /* handcpu_h */
