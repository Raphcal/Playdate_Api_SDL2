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

/**
 * @brief Modifie l'opacité du fondu.
 * @param sprite Le sprite `Fade` à modifier.
 * @param opacity Opacité à définir, entre 0 et 1. 0 étant totalement transparent et 1 étant totalement noir.
 */
void FadeSetOpacity(LCDSprite * _Nonnull sprite, float opacity);

#endif /* fade_h */
