//
//  recentscores.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/11/2024.
//

#ifndef recentscores_h
#define recentscores_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nullable RecentScoresConstructor(MELRectangle frame, uint32_t * _Nonnull scores, uint32_t playCount);

#endif /* recentscores_h */
