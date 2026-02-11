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
    SampleRotate,
    SampleMove,
    SampleAttach,
    SampleMagic,
    SampleChipsDrop,
    SampleChipsCollision,
    SampleLevelUp,
    SampleCheerBonus,
    SampleOvertake,
    SampleCounter1,
    SampleCounter2,
    SampleBang,
    SampleAudience,
    SampleCount
} Sample;

MELBoolean SampleLoadNextSample(void);
void SamplePlay(Sample sample);
void SamplePlayWithRate(Sample sample, float rate);

#endif /* samples_h */
