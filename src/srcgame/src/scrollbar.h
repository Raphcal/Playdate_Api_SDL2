//
//  scrollbar.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 01/04/2025.
//

#ifndef scrollbar_h
#define scrollbar_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nullable ScrollBarConstructor(float top, float height);
void ScrollBarSetHeight(LCDSprite * _Nonnull sprite, float height);

#endif /* scrollbar_h */
