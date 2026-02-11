//
//  textbubble.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#ifndef textbubble_h
#define textbubble_h

#include "common.h"
#include "../lib/melice.h"

#include "story.h"

#define kTextBubbleLineCount 3

typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t charStart;
    uint32_t charCount;
    MELIntRectangle frame;
    MELBoolean isFullyDrawn;
} TextBubbleLine;

typedef struct {
    MELSprite super;
    LCDSprite * _Nullable aButton;
    PhraseList dialog;
    /// Numéro de la phrase dans `encounter`.
    uint32_t phraseIndex;
    TextBubbleLine lines[kTextBubbleLineCount];
    float textIndex;
    MELBoolean isPaused;
} TextBubble;

LCDSprite * _Nonnull TextBubbleConstructor(PhraseList dialog, MELRectangle frame);

#endif /* textbubble_h */
