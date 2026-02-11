//
//  samples.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 19/08/2023.
//

#include "samples.h"

static SamplePlayer * _Nullable samplePlayers[SampleCount];
static Sample nextSample = SampleBulletCollision;

MELBoolean SampleLoadNextSample(void) {
    AudioSample *sample;
    switch (nextSample) {
        case SampleBulletCollision:
            sample = playdate->sound->sample->load("audio/sfx-bullet-collision");
            break;
        case SampleCatch:
            sample = playdate->sound->sample->load("audio/sfx-catch");
            break;
        case SampleThrow:
            sample = playdate->sound->sample->load("audio/sfx-throw");
            break;
        case SampleCollision:
            sample = playdate->sound->sample->load("audio/sfx-collision");
            break;
        case SampleExplosion:
            sample = playdate->sound->sample->load("audio/sfx-explosion");
            break;
        case SamplePlayerExplosion:
            sample = playdate->sound->sample->load("audio/sfx-player-explosion");
            break;
        case SampleOneUp:
            sample = playdate->sound->sample->load("audio/sfx-oneup");
            break;
        case SamplePickUpBonus:
            sample = playdate->sound->sample->load("audio/sfx-pick-up-bonus");
            break;
        case SampleConfirm:
            sample = playdate->sound->sample->load("audio/sfx-confirm");
            break;
        case SampleSelect:
            sample = playdate->sound->sample->load("audio/sfx-select");
            break;
        case SampleWarningBoss:
            sample = playdate->sound->sample->load("audio/sfx-warning-boss");
            break;
        case SampleRank:
            sample = playdate->sound->sample->load("audio/sfx-rank");
            break;
        case SampleCounter:
            sample = playdate->sound->sample->load("audio/sfx-counter");
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
