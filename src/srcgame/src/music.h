//
//  music.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 21/03/2024.
//

#ifndef music_h
#define music_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    FilePlayer * _Nullable filePlayers[2];
    uint8_t current;
} MusicManager;

void MusicManagerReset(void);
void MusicManagerPlay(const char * _Nonnull music, int repeat, float crossFadeDuration);
void MusicManagerSetRate(float rate);
void MusicManagerFadeOut(float duration);

#endif /* music_h */
