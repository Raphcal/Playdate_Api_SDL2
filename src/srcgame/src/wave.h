//
//  wave.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 03/02/2023.
//

#ifndef wave_h
#define wave_h

#include "common.h"
#include "../lib/melice.h"
#include "../gen/maps.h"

// TODO: Faire un système de flag (appeler tags ?) dans MapMaker et lire ces flags au lieu du type.
typedef enum {
    /// Vague d'apprentissage, le temps ne s'écoule pas.
    WaveTypeTutorial = 1,
    /// Vague normale.
    WaveTypeNormal = 2,
    /// Boss.
    /// NOTE: Mettre le nom du boss ici ?
    WaveTypeBoss = 4,
    /// Transition vers un nouveau décors.
    WaveTypeBackground = 8,
    /// Autre type de carte, pas une vague.
    WaveTypeOther = 16,
} WaveType;

/**
 * Vague d'ennemis.
 *
 * La date de début permet de rythmer le jeu global et d'introduire des checkpoints dans le niveau (exemple dans Ikaruga : transition animée vers un point ou arrivée d'un boss/sous-boss.
 * La durée permet de limiter la présence d'une vague : exemple durée max d'un sous-boss. -> Pas utile, il suffit de mettre un startTime pour une vague d'après (comme dans Mushihime Sama).
 *
 * Permet de faire :
 * - vague simple : startTime = 0
 * - vague simple
 * - vague bonus
 * - vague mini boss : startTime = 60,
 * - vague bonus
 * - vague simple : starTime = 90,
 * - vague simple
 * - vague simple
 * - vague bonus
 * - vague boss : startTime = 120
 */
typedef struct {
    MapName mapName;
    /// Si > 0, temps en seconde à partir de quand cette vague commence.
    int startTime;
    WaveType type;
    MELSpriteInstanceList instances;
} Wave;

typedef Wave * _Nullable WaveRef;

MELListDefine(Wave);
MELListDefine(WaveRef);

void WaveInit(Wave * _Nonnull self, const MapName mapName);
void WaveDeinit(Wave * _Nonnull self);
void WaveInstantiate(Wave * _Nonnull self);

#endif /* wave_h */
