//
//  fade.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 21/07/2023.
//

#ifndef fade_h
#define fade_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELSprite super;
    uint8_t opacity;
    uint8_t oldOpacity;
    LCDBitmap * _Nonnull pattern;
} Fade;

LCDSprite * _Nonnull FadeConstructor(void);
const MELSpriteClass * _Nonnull FadeGetClass(void);

#endif /* fade_h */
