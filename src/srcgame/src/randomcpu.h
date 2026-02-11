//
//  randomcpu.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 23/05/2024.
//

#ifndef randomcpu_h
#define randomcpu_h

#include "cpu.h"

#define UseRandomCpu(buttonPressDelay) .super = { .alloc = CpuAlloc, .dealloc = ActorDealloc, .update = CpuUpdate, .handChanged = RandomCpuHandChanged }, .timeBetweenButtonPress = buttonPressDelay

/**
 * Adversaire qui place ses tuiles n'importe où, aléatoirement.
 */
typedef Cpu RandomCpu;

/**
 * Créé un adversaire qui place ses tuiles n'importe où, sans réfléchir.
 */
Actor * _Nonnull RandomCpuMake(void);
void RandomCpuHandChanged(Actor * _Nonnull actor, Card * _Nonnull hand, Card * _Nonnull nextHand);

#endif /* randomcpu_h */
