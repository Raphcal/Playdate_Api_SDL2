//
//  instantcpu.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 22/05/2024.
//

#ifndef instantcpu_h
#define instantcpu_h

#include "cpu.h"

#define UseInstantCpu(buttonPressDelay) .super = { .super = { .alloc = InstantCpuAlloc, .dealloc = InstantCpuDealloc, .update = CpuUpdate, .handChanged = InstantCpuHandChanged }, .timeBetweenButtonPress = buttonPressDelay }

/**
 * Adversaire qui place ses tuiles en fonction des mains que cela va créer (même numéro, même couleur, suite).
 * Un système de poid permet de régler les préférences de l'adversaire.
 */
typedef struct {
    Cpu super;
    MELBoolean isAlwaysFast;
    MELBoolean isFastWhenGoodHandIsFound;
    MELBoolean isFastWhenAboveTarget;
    int randomFast;
    int sameKindScore;
    int sameSuitScore;
    int straightScore;
    int rowScore;
    int miss;
    MELBoolean preferSides;
    MELBoolean avoidCenter;
    HandIdentifier * _Nullable identifier;
} InstantCpu;

/**
 * Créé un adversaire qui place ses tuiles pour faire des coups mais sans réfléchir aux combos.
 */
Actor * _Nonnull InstantCpuMake(MELBoolean isAlwaysFast, MELBoolean isFastWhenGoodHandIsFound, int sameKindScore, int sameSuitScore, int straightScore, int rowScore, MELBoolean preferSides);
Actor * _Nonnull InstantCpuMakeDefault(void);

Actor * _Nonnull InstantCpuAlloc(Actor * _Nonnull actor);
void InstantCpuDealloc(Actor * _Nonnull self);
void InstantCpuHandChanged(Actor * _Nonnull self, Card * _Nonnull hand, Card * _Nonnull nextHand);

#endif /* instantcpu_h */
