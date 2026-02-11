//
//  patterncpu.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 22/05/2024.
//

#ifndef patterncpu_h
#define patterncpu_h

#include "cpu.h"
#include "instantcpu.h"

#define UsePatternCpu(buttonPressDelay) .super = { .super = { .alloc = PatternCpuAlloc, .dealloc = PatternCpuDealloc, .update = PatternCpuUpdate, .handChanged = PatternCpuHandChanged }, .timeBetweenButtonPress = buttonPressDelay }

typedef struct {
    const char * _Nullable name;
    MELIntSize size;
    const char * _Nullable pattern;
    uint8_t variableCount;
    /// Numéro de la variable du trigger.
    uint8_t triggerVariable;
} Pattern;

typedef struct {
    int count;
    const Pattern * _Nullable memory;
} PatternConstList;

typedef struct {
    const Pattern * _Nonnull pattern;
    MELIntPoint topLeft;
    /// Flags des rangs mappés.
    uint8_t mapped;
    /// Correspondance entre une variable (indice du tableau) et le rang des cartes.
    Rank mapping[RankCount];
    /// Emplacements à occuper pour chaque variable.
    MELIntPointList locations[RankCount];
    /// Emplacements possibles pour démarrer le combo.
    MELIntPointList triggers;
    /// Nombre de combos qui seront (théoriquement) déclanchés.
    uint8_t comboCount;
    uint8_t usableTrigger;
    MELBoolean isReadyForTrigger;
} PatternMatch;

MELListDefine(PatternMatch);

typedef struct {
    InstantCpu super;
    PatternConstList patterns;
    PatternMatchList matches;
    Card hand[2];
    int16_t switchToInstantCpuLimit;
    int16_t backToPatternCpuLimit;
    MELBoolean isInstantCpuMode;
    /// Nombre de correspondances maximum à autoriser au même moment.
    uint8_t matchLimit;
    /// Numéro de la correspondance en cours d'examen.
    int8_t index;
} PatternCpu;

Actor * _Nonnull PatternCpuAlloc(Actor * _Nonnull actor);
void PatternCpuDealloc(Actor * _Nonnull actor);
void PatternCpuHandChanged(Actor * _Nonnull actor, Card * _Nonnull hand, Card * _Nonnull nextHand);
void PatternCpuUpdate(Actor * _Nonnull actor);

#endif /* patterncpu_h */
