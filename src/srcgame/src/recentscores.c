//
//  recentscores.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/11/2024.
//

#include "recentscores.h"

#include "savegame.h"
#include "image.h"

#define kValueAsStringLength 100
#define kScoreScaleLength 30

static const LCDColor kGrey50Pattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = noir, 1 = blanc
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    // Mask, 0 = transparent, 1 = visible
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
};

typedef struct {
    MELSprite super;
    uint32_t * _Nonnull scores;
    uint32_t playCount;
} RecentScores;

static void update(LCDSprite * _Nonnull sprite);
static void repaint(RecentScores * _Nonnull self, LCDSprite * _Nonnull sprite);

LCDSprite * _Nullable RecentScoresConstructor(MELRectangle frame, uint32_t * _Nonnull scores, uint32_t playCount) {
    RecentScores *self = new(RecentScores);
    *self = (RecentScores) {
        .super = {
            .class = ImageGetClass(),
            .frame = frame,
        },
        .scores = scores,
        .playCount = playCount,
    };

    if (!fontNontendoBold) {
        fontNontendoBold = LCDFontLoadOrError("fonts/Nontendo-Bold");
    }

    LCDBitmap *image = playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorWhite);

    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->setUserdata(sprite, self);
    spriteAPI->setImage(sprite, image, kBitmapUnflipped);
    spriteAPI->setUpdateFunction(sprite, update);
    spriteAPI->addSprite(sprite);

    repaint(self, sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void repaint(RecentScores * _Nonnull self, LCDSprite * _Nonnull sprite) {
    if (self->playCount == 0) {
        // Rien à faire.
        return;
    }

    const MELRectangle frame = self->super.frame;
    LCDBitmap *image = playdate->sprite->getImage(sprite);

    struct playdate_graphics gfx = *playdate->graphics;
    gfx.pushContext(image);
    gfx.setDrawMode(kDrawModeCopy);

    const unsigned int max = MELIntMin(self->playCount, kScoreAttackHistoryCount);
    uint32_t minScore = self->scores[0],
             maxScore = self->scores[0];
    for (unsigned int index = 1; index < max; index++) {
        const uint32_t score = self->scores[index];
        minScore = MELIntMin(minScore, score);
        maxScore = MELIntMax(maxScore, score);
    }
    const uint32_t centerScore = minScore + (maxScore - minScore) / 2;

    const int fontHeight = gfx.getFontHeight(fontNontendoBold);
    char topValue[kScoreScaleLength], centerValue[kScoreScaleLength], bottomValue[kScoreScaleLength];
    MELUInt32ToStringWithFixedSizeBuffer(maxScore, topValue, kScoreScaleLength);
    MELUInt32ToStringWithFixedSizeBuffer(centerScore, centerValue, kScoreScaleLength);
    MELUInt32ToStringWithFixedSizeBuffer(minScore, bottomValue, kScoreScaleLength);

    const int maxScoreWidth = gfx.getTextWidth(fontNontendoBold, topValue, kScoreScaleLength, kASCIIEncoding, 0);
    const int centerScoreWidth = gfx.getTextWidth(fontNontendoBold, centerValue, kScoreScaleLength, kASCIIEncoding, 0);
    const int minScoreWidth = gfx.getTextWidth(fontNontendoBold, bottomValue, kScoreScaleLength, kASCIIEncoding, 0);
    const int scaleMaxWidth = MELInt32Max3(maxScoreWidth, centerScoreWidth, minScoreWidth);

    gfx.setFont(fontNontendoBold);
    gfx.drawText(topValue, kScoreScaleLength, kASCIIEncoding, 10 + scaleMaxWidth - maxScoreWidth, 0);
    gfx.drawText(centerValue, kScoreScaleLength, kASCIIEncoding, 10 + scaleMaxWidth - centerScoreWidth, (frame.size.height - fontHeight) / 2.0f);
    gfx.drawText(bottomValue, kScoreScaleLength, kASCIIEncoding, 10 + scaleMaxWidth - minScoreWidth, frame.size.height - fontHeight);

    const int lineLeft = 10 + scaleMaxWidth + 4;

    const float rowHeight = frame.size.height / 4;
    gfx.drawLine(lineLeft, 0, LCD_COLUMNS - 10, 0, 1, kColorBlack);
    gfx.drawLine(lineLeft, rowHeight, LCD_COLUMNS - 10, rowHeight, 1, kGrey50Pattern);
    gfx.drawLine(lineLeft, rowHeight * 2, LCD_COLUMNS - 10, rowHeight * 2, 1, kGrey50Pattern);
    gfx.drawLine(lineLeft, rowHeight * 3, LCD_COLUMNS - 10, rowHeight * 3, 1, kGrey50Pattern);
    gfx.drawLine(lineLeft, frame.size.height - 1, LCD_COLUMNS - 10, frame.size.height - 1, 1, kColorBlack);

    /// Nombre de pixels par point.
    const float scale = frame.size.height / (maxScore - minScore);

    const int leftMargin = lineLeft + 10;
    const int rightMargin = 20;
    const int spaceBetweenDots = (frame.size.width - leftMargin - rightMargin) / (max - 1);
    static const int radius = 3;
    static const int diameter = radius * 2;

    float oldY = 0.0f;
    for (unsigned int index = 0, left = leftMargin; index < max; index++, left += spaceBetweenDots) {
        // TODO: Dessiner un point et faire une ligne vers le point suivant/précédent
        const uint32_t score = self->scores[index];
        const float y = frame.size.height - (score - minScore) * scale;
        gfx.fillEllipse(left - radius, y - radius, diameter, diameter, 0, 0, kColorBlack);

        if (index) {
            gfx.drawLine(left - spaceBetweenDots, oldY, left, y, 1, kColorBlack);
        }
        oldY = y;
    }
    gfx.popContext();

    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const MELPoint origin = self->frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
}
