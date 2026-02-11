//
//  progressbar.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/10/2023.
//

#ifndef progressbar_h
#define progressbar_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull ProgressBarConstructor(const MELRectangle bounds, const float progress);

void ProgressBarSetProgress(LCDSprite * _Nonnull sprite, float progress);

#endif /* progressbar_h */
