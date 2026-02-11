//
//  progressbar.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/11/2024.
//

#ifndef progressbar_h
#define progressbar_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nullable ProgressBarConstructor(MELRectangle frame, int minimum, int maximum, int * _Nonnull current);

#endif /* progressbar_h */
