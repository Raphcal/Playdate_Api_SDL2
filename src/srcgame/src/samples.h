//
//  samples.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 19/08/2023.
//

#ifndef samples_h
#define samples_h

#include "common.h"
#include "../lib/melice.h"

typedef enum {
    SampleBulletCollision,
    SampleCatch,
    SampleCollision,
    SampleConfirm,
    SampleExplosion,
    SampleOneUp,
    SamplePickUpBonus,
    SamplePlayerExplosion,
    SampleSelect,
    SampleThrow,
    SampleWarningBoss,
    SampleRank,
    SampleCounter,
    SampleCount
} Sample;

MELBoolean SampleLoadNextSample(void);
void SamplePlay(Sample sample);

#endif /* samples_h */
