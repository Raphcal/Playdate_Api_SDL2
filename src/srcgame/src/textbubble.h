//
//  textbubble.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 01/09/2023.
//

#ifndef textbubble_h
#define textbubble_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull TextBubbleConstructor(const char * _Nonnull text, int lines, MELPoint origin);
void TextBubbleAnimateAppear(LCDSprite * _Nonnull sprite);
void TextBubbleAnimateDisappear(LCDSprite * _Nonnull sprite);
/**
 * Déplace le texte de la valeur donnée.
 *
 * @param value Distance à déplacer.
 * @returns La distance inutilisée (supérieure à 0 si trop bas, inférieure à 0 si trop haut).
 */
float TextBubbleScroll(LCDSprite * _Nonnull sprite, float value);
float TextBubbleZoom(LCDSprite * _Nonnull sprite, float value, MELDirection zoomDirection);

#endif /* textbubble_h */
