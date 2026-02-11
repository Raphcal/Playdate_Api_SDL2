//
//  counter.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/10/2023.
//

#ifndef counter_h
#define counter_h

#include "common.h"
#include "../lib/melice.h"

/**
 * Créé un compteur. La méthode `update` par défaut ne prend pas en compte la caméra.
 *
 * @param origin Point où aligner le texte.
 * @param alignment Alignement par rapport à l'origine.
 * @param font Police à utiliser pour afficher les chiffres.
 * @param value Valeur initiale à afficher.
 */
LCDSprite * _Nonnull CounterConstructor(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, const uint32_t value);

LCDSprite * _Nonnull CounterConstructorWithUInt8Pointer(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, uint8_t * _Nonnull value);
LCDSprite * _Nonnull CounterConstructorWithUInt32Pointer(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, uint32_t * _Nonnull value);
LCDSprite * _Nonnull CounterConstructorWithFloatPointer(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, float * _Nonnull value);

/**
 * Modifie la valeur affichée.
 *
 * @param sprite Instance de `Counter`.
 * @param value Nombre à afficher.
 */
void CounterSetValue(LCDSprite * _Nullable sprite, const uint32_t value);

void CountUpdateAnchorWithCurrentOrigin(LCDSprite * _Nonnull sprite);

#endif /* counter_h */
