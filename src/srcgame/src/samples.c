//
//  samples.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 19/08/2023.
//

#include "samples.h"

static SamplePlayer * _Nullable samplePlayers[SampleCount];
static Sample nextSample = 0;

MELBoolean SampleLoadNextSample(void) {
    AudioSample *sample;
    switch (nextSample) {
        case SampleRotate:
            sample = playdate->sound->sample->load("sfx/rotate");
            break;
        case SampleMove:
            sample = playdate->sound->sample->load("sfx/move");
            break;
        case SampleAttach:
            sample = playdate->sound->sample->load("sfx/attach");
            break;
        case SampleMagic:
            sample = playdate->sound->sample->load("sfx/magic");
            break;
        case SampleChipsDrop:
            sample = playdate->sound->sample->load("sfx/chips_drop");
            break;
        case SampleChipsCollision:
            sample = playdate->sound->sample->load("sfx/chips_collision");
            break;
        case SampleLevelUp:
            sample = playdate->sound->sample->load("sfx/levelup");
            break;
        case SampleCheerBonus:
            sample = playdate->sound->sample->load("sfx/cheer");
            break;
        case SampleOvertake:
            sample = playdate->sound->sample->load("sfx/overtake");
            break;
        case SampleCounter1:
            sample = playdate->sound->sample->load("sfx/counter1");
            break;
        case SampleCounter2:
            sample = playdate->sound->sample->load("sfx/counter2");
            break;
        case SampleBang:
            sample = playdate->sound->sample->load("sfx/bang");
            break;
        case SampleAudience:
            sample = playdate->sound->sample->load("sfx/audience");
            break;
        default:
            return true;
    }
    SamplePlayer *samplePlayer = playdate->sound->sampleplayer->newPlayer();
    playdate->sound->sampleplayer->setSample(samplePlayer, sample);
    samplePlayers[nextSample++] = samplePlayer;
    return false;
}

void SamplePlay(Sample sample) {
    playdate->sound->sampleplayer->play(samplePlayers[sample], 1, 1.0f);
}

void SamplePlayWithRate(Sample sample, float rate) {
    playdate->sound->sampleplayer->play(samplePlayers[sample], 1, rate);
}
