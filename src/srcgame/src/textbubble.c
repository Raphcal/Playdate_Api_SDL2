//
//  textbubble.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#include "textbubble.h"

#include "image.h"
#include "../gen/spriteabutton.h"

/// Nombre de lettres à afficher par seconde.
static const float kCharPerSecond = 20.0f;

static void reflowText(TextBubble * _Nonnull self);
static void dealloc(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void updatePause(LCDSprite * _Nonnull sprite);
static void onLanguageChanged(void * _Nullable userdata, int language);

static const MELSpriteClass TextBubbleClass = (MELSpriteClass) {
    .destroy = dealloc,
};

LCDSprite * _Nonnull TextBubbleConstructor(PhraseList dialog, MELRectangle frame) {
    loadSpriteAButtonPalette();
    LCDSprite *aButton =  ImageConstructor((MELPoint) {
        .x = LCD_COLUMNS - spriteAButton.size.width / 2 - 8,
        .y = LCD_ROWS - spriteAButton.size.height / 2 - 8,
    }, playdate->graphics->getTableBitmap(spriteAButton.palette, 0));
    playdate->sprite->setVisible(aButton, false);
    playdate->sprite->setZIndex(aButton, ZINDEX_DIALOG_PRESS_A);

    LCDBitmap *image = playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorWhite);

    TextBubble *self = playdate->system->realloc(NULL, sizeof(TextBubble));
    *self = (TextBubble) {
        .super = {
            .class = &TextBubbleClass,
            .frame = frame,
        },
        .dialog = dialog,
        .aButton = aButton,
    };
    reflowText(self);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->addSprite(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_DIALOG);
    
    MELEventBusAddListener(EventLanguageChanged, onLanguageChanged, sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    MELEventBusRemoveListeners(sprite);
    ImageDealloc(sprite);
}

static const char * _Nonnull getCurrentPhraseText(TextBubble * _Nonnull self) {
    const uint32_t index = self->phraseIndex;
    if (index < self->dialog.count) {
        const Phrase phrase = self->dialog.memory[index];
        return MELLocalizedStringGetText(phrase.line);
    } else {
        return kEmptyString;
    }
}

static void reflowText(TextBubble * _Nonnull self) {
    const char * _Nonnull text = getCurrentPhraseText(self);
    const int fontHeight = playdate->graphics->getFontHeight(currentFont);
    const int textLength = (int)strlen(text);
    const MELSize size = self->super.frame.size;
    TextBubbleLine lastLine = self->lines[kTextBubbleLineCount - 1];
    const int lastEnd = lastLine.end;
    int lineStart = lastEnd;
    uint32_t charStart = lastLine.charStart + lastLine.charCount;
    for (uint32_t line = 0; line < kTextBubbleLineCount; line++) {
        if (lineStart > textLength) {
            self->lines[line] = (TextBubbleLine) {
                .start = textLength,
                .end = textLength,
                .charStart = charStart,
                .charCount = 0,
                .isFullyDrawn = true,
            };
            continue;
        }
        const float availableWidth = line < kTextBubbleLineCount - 1 ? size.width : size.width - spriteAButton.size.width;

        char *space;
        int endIndex, bestEndIndex, lineWidth, charCount, bestCharCount;
        endIndex = bestEndIndex = lineStart;
        charCount = bestCharCount = 0;
        do {
            if (currentLanguage == MELLanguageJapanese) {
                endIndex = endIndex + MELUTF8StringCharacterSize(text, endIndex);
                charCount++;
            } else {
                space = strchr(text + endIndex, ' ');
                endIndex = space != NULL ? (int)(space - text + 1) : textLength;
                charCount = MELUTF8StringCodePointCount(text + lineStart, endIndex - lineStart);
            }
            lineWidth = playdate->graphics->getTextWidth(currentFont, text + lineStart, charCount, kUTF8Encoding, 0);
            if (lineWidth <= availableWidth) {
                bestEndIndex = endIndex;
                bestCharCount = charCount;
            }
        } while (endIndex < textLength && lineWidth <= availableWidth);

        const float nameHeight = playdate->graphics->getFontHeight(smallFont);
        const float marginBottom = currentLanguage == MELLanguageJapanese ? 10.0f : 4.0f;
        self->lines[line] = (TextBubbleLine) {
            .frame = (MELIntRectangle) {
                .origin = {
                    .x = 0,
                    .y = (nameHeight + 6) + (fontHeight + marginBottom) * line,
                },
                .size = {
                    .width = availableWidth,
                    .height = fontHeight
                }
            },
            .start = lineStart,
            .end = bestEndIndex,
            .charStart = charStart,
            .charCount = bestCharCount,
            .isFullyDrawn = bestEndIndex == lineStart,
        };
        lineStart = bestEndIndex;
        charStart += bestCharCount;
    }
}

static void drawLine(TextBubbleLine * _Nonnull self, const char * _Nonnull text, int index) {
    if (self->isFullyDrawn || index < self->charStart) {
        return;
    }
    const int end = self->charCount;
    int charIndex = index - self->charStart;
    self->isFullyDrawn = charIndex == end;

    MELIntRectangle frame = self->frame;
    playdate->graphics->fillRect(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height, kColorWhite);
    playdate->graphics->drawText(text + self->start, MELIntMin(charIndex, self->charCount), kUTF8Encoding, frame.origin.x, frame.origin.y);
}

static void draw(TextBubble * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const char *text = getCurrentPhraseText(self);
    const float textIndex = self->textIndex;

    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->pushContext(image);
    playdate->graphics->setDrawMode(kDrawModeCopy);

    const uint32_t phraseIndex = self->phraseIndex;
    const Character character = phraseIndex < self->dialog.count
        ?  self->dialog.memory[phraseIndex].character
        : CharacterNone;
    playdate->graphics->setFont(smallFont);
    playdate->graphics->drawText(MELLocalizedStringGetText(CharacterName[character]), 99, kUTF8Encoding, 0, 0);

    playdate->graphics->setFont(currentFont);
    for (uint32_t index = 0; index < kTextBubbleLineCount; index++) {
        drawLine(self->lines + index, text, textIndex);
    }
    playdate->graphics->popContext();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->markDirty(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);

    TextBubbleLine lastLine = self->lines[kTextBubbleLineCount - 1];
    const uint32_t endIndex = lastLine.charStart + lastLine.charCount;
    float textIndex = MELFloatMin(self->textIndex + DELTA * kCharPerSecond, endIndex);

    if (((int)textIndex) > ((int)self->textIndex)) {
        // TODO: Jouer un son.
    }

    if (MELControllerMake().pressedA) {
        textIndex = endIndex;
    }

    self->textIndex = textIndex;
    if (textIndex == endIndex) {
        playdate->sprite->setVisible(self->aButton, true);
        playdate->sprite->setUpdateFunction(sprite, updatePause);
    }
    draw(self, sprite);
}

static void updatePause(LCDSprite * _Nonnull sprite) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);

    if (MELControllerMake().pressedA) {
        LCDBitmap *image = playdate->sprite->getImage(sprite);
        playdate->graphics->clearBitmap(image, kColorWhite);
        playdate->sprite->markDirty(sprite);

        playdate->sprite->setVisible(self->aButton, false);

        TextBubbleLine lastLine = self->lines[kTextBubbleLineCount - 1];
        const char *text = getCurrentPhraseText(self);
        const int textLength = (int)strlen(text);
        if (lastLine.end >= textLength) {
            const int phraseIndex = ++self->phraseIndex;
            if (phraseIndex < self->dialog.count) {
                self->lines[kTextBubbleLineCount - 1].end = 0;
                MELEventBusFireEvent(EventDialogChanged, phraseIndex);
            } else {
                MELEventBusFireEvent(EventDialogHasEnded, true);
                return;
            }
        }
        reflowText(self);
        playdate->sprite->setUpdateFunction(sprite, update);
    }
}

static void onLanguageChanged(void * _Nullable userdata, int language) {
    LCDSprite *sprite = userdata;
    TextBubble *self = playdate->sprite->getUserdata(sprite);
    self->textIndex = 0;
    self->phraseIndex = 0;
    self->lines[0] = (TextBubbleLine){};
    self->lines[1] = (TextBubbleLine){};
    self->lines[2] = (TextBubbleLine){};
    reflowText(self);

    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->clearBitmap(image, kColorWhite);
    playdate->sprite->markDirty(sprite);

    playdate->sprite->setVisible(self->aButton, false);

    MELEventBusFireEvent(EventDialogChanged, 0);
    playdate->sprite->setUpdateFunction(sprite, update);
}
