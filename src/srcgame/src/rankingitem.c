//
//  ranking_entry.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 18/06/2023.
//

#include "rankingitem.h"

#include "camera.h"
#include "../gen/spriteranksmall.h"

static const int kFontHeight = 16;
static const int kFaceWidth = 64;

static const int kSmallRankWidth[] = {0, 8, 8, 8, 9, 9, 9, 8, 19};

static void destroy(LCDSprite * _Nonnull sprite);
static LCDBitmap * _Nonnull draw(RankingItem * _Nonnull self, LCDBitmap * _Nullable oldBitmap);

static const MELSpriteClass RankingItemClass = (MELSpriteClass) {
    .destroy = destroy,
};

static void update(LCDSprite * _Nonnull sprite) {
    RankingItem *self = playdate->sprite->getUserdata(sprite);

    if (self->isEditable) {
        const uint32_t timeInMillis = playdate->system->getCurrentTimeMilliseconds();
        // time % 1500 < 750
        const MELBoolean isCursorVisible = (timeInMillis / 500) % 2;
        if (isCursorVisible != self->isCursorVisible) {
            self->isDirty = true;
            self->isCursorVisible = isCursorVisible;
        }
    }

    if (self->isDirty) {
        LCDBitmap *oldBitmap = playdate->sprite->getImage(sprite);
        playdate->sprite->setImage(sprite, draw(self, oldBitmap), kBitmapUnflipped);
        playdate->sprite->markDirty(sprite);
        self->isDirty = false;
    }

    MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
}

static void destroy(LCDSprite * _Nonnull sprite) {
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);
    MELSpriteDealloc(sprite);
}

static void drawReadOnly(RankingItem * _Nonnull self) {
    const Score score = self->score;
    const MELSize size = self->super.frame.size;

    char *rankText = MELUInt32ToString(self->lastRank);
    int rankTextCapacity = MELStringLengthToDisplayUInt(self->lastRank) + 1;
    const int lastRankTextWidth = playdate->graphics->getTextWidth(romajiFont, rankText, 99, kASCIIEncoding, 0);

    const MELBoolean drawFaces = self->faces != NULL;

    const int rankWidth = lastRankTextWidth + 8 - (lastRankTextWidth % 8);
    const int slopeWidth = 8;
    const int borderRight = 64;
    const int polygonLeft = rankWidth;
    const int polygonRight = size.width - (drawFaces ? borderRight : 0);
    int points[] = {polygonLeft + slopeWidth, 0, polygonRight, 0, polygonRight - slopeWidth, size.height, polygonLeft, size.height};
    playdate->graphics->fillPolygon(4, points, kColorBlack, kPolygonFillNonZero);

    if (drawFaces) {
        const int spaceBetweenLines = 3;
        const int linesWidth = polygonRight + 24;
        for (int x = polygonRight; x < linesWidth; x += spaceBetweenLines) {
            playdate->graphics->drawLine(x, 0, x - 8, size.height, 1, kColorBlack);
        }

        // Personnage
        playdate->graphics->setDrawMode(kDrawModeCopy);
        LCDBitmap *face = playdate->graphics->getTableBitmap(self->faces, score.character);
        playdate->graphics->drawBitmap(face, size.width - kFaceWidth, 0, kBitmapUnflipped);
    }

    // Rang
    rankText = MELUInt32ToStringWithBuffer(score.super.rank, rankText, &rankTextCapacity);
    const int rankTextWidth = playdate->graphics->getTextWidth(romajiFont, rankText, 99, kASCIIEncoding, 0);
    playdate->graphics->drawText(rankText, 99, kASCIIEncoding, rankWidth - rankTextWidth, 4);
    playdate->system->realloc(rankText, 0);

    const int textLeft = polygonLeft + 16;
    playdate->graphics->setDrawMode(kDrawModeInverted);

    // Nom du joueur
    const int lengthToDisplay = 15;
    playdate->graphics->drawText(score.super.player, lengthToDisplay, kASCIIEncoding, textLeft, 4);

    // Score
    char *scoreText = MELUInt32ToString(score.super.value);
    const int scoreTextWidth = playdate->graphics->getTextWidth(romajiFont, scoreText, 99, kASCIIEncoding, 0);
    const int scoreTextRight = polygonRight - scoreTextWidth - slopeWidth - 4;
    playdate->graphics->drawText(scoreText, 99, kASCIIEncoding, scoreTextRight, 22);
    playdate->system->realloc(scoreText, 0);

    // Rangs
    if (score.ranks.lives != RankNone) {
        loadSpriteRankSmallPalette();
        LCDBitmap *lives = playdate->graphics->getTableBitmap(spriteRankSmall.palette, score.ranks.lives - 1);
        LCDBitmap *timeLeft = playdate->graphics->getTableBitmap(spriteRankSmall.palette, score.ranks.timeLeft - 1);
        LCDBitmap *combo = playdate->graphics->getTableBitmap(spriteRankSmall.palette, score.ranks.combo - 1);

        const int livesLeft = textLeft;
        const int timeLeftLeft = textLeft + kSmallRankWidth[score.ranks.lives] + 2;
        const int comboLeft = timeLeftLeft + kSmallRankWidth[score.ranks.timeLeft] + 2;

        playdate->graphics->drawBitmap(lives, livesLeft, 26, kBitmapUnflipped);
        playdate->graphics->drawBitmap(timeLeft, timeLeftLeft, 26, kBitmapUnflipped);
        playdate->graphics->drawBitmap(combo, comboLeft, 26, kBitmapUnflipped);
    }
}

static void drawEditable(RankingItem * _Nonnull self) {
    const Score score = self->score;
    const MELSize size = self->super.frame.size;

    char *rankText = MELUInt32ToString(self->lastRank);
    int rankTextCapacity = MELStringLengthToDisplayUInt(self->lastRank) + 1;
    const int lastRankTextWidth = playdate->graphics->getTextWidth(romajiFont, rankText, 99, kASCIIEncoding, 0);

    const int rankWidth = lastRankTextWidth + 8 - (lastRankTextWidth % 8);
    const int slopeWidth = 8;
    const int polygonLeft = rankWidth;
    const int polygonRight = size.width;
    int points[] = {polygonLeft + slopeWidth, 0, polygonRight, 0, polygonRight - slopeWidth, size.height, polygonLeft, size.height};
    playdate->graphics->fillPolygon(4, points, kColorBlack, kPolygonFillNonZero);

    const int lineSize = 2;
    const int innerPolygonLeft = polygonLeft + lineSize;
    const int innerPolygonRight = polygonRight - lineSize;
    const int innerPolygonTop = lineSize;
    const int innerPolygonBottom = size.height - lineSize;
    int innerPoints[] = {innerPolygonLeft + slopeWidth, innerPolygonTop, innerPolygonRight, innerPolygonTop, innerPolygonRight - slopeWidth, innerPolygonBottom, innerPolygonLeft, innerPolygonBottom};
    playdate->graphics->fillPolygon(4, innerPoints, kColorWhite, kPolygonFillNonZero);

    // Rang
    rankText = MELUInt32ToStringWithBuffer(score.super.rank, rankText, &rankTextCapacity);
    const int rankTextWidth = playdate->graphics->getTextWidth(romajiFont, rankText, 99, kASCIIEncoding, 0);
    playdate->graphics->drawText(rankText, 99, kASCIIEncoding, rankWidth - rankTextWidth, 4);
    playdate->system->realloc(rankText, 0);

    const int textLeft = polygonLeft + 16;

    // Nom du joueur
    const int lengthToDisplay = 15;
    const int scorePlayerWidth = playdate->graphics->getTextWidth(romajiFont, score.super.player, lengthToDisplay, kASCIIEncoding, 0);
    playdate->graphics->drawText(score.super.player, lengthToDisplay, kASCIIEncoding, textLeft, 4);

    // Curseur
    if (self->isCursorVisible) {
        const int cursorLeft = textLeft + scorePlayerWidth + 4;
        playdate->graphics->fillRect(cursorLeft, 4, 2, kFontHeight, kColorBlack);
    }

    // Score
    char *scoreText = MELUInt32ToString(score.super.value);
    const int scoreTextWidth = playdate->graphics->getTextWidth(romajiFont, scoreText, 99, kASCIIEncoding, 0);
    const int scoreTextRight = polygonRight - scoreTextWidth - slopeWidth - 4;
    playdate->graphics->drawText(scoreText, 99, kASCIIEncoding, scoreTextRight, 22);
    playdate->system->realloc(scoreText, 0);

    // Rangs
    if (score.ranks.lives != RankNone) {
        loadSpriteRankSmallPalette();
        LCDBitmap *lives = playdate->graphics->getTableBitmap(spriteRankSmall.palette, score.ranks.lives - 1);
        LCDBitmap *timeLeft = playdate->graphics->getTableBitmap(spriteRankSmall.palette, score.ranks.timeLeft - 1);
        LCDBitmap *combo = playdate->graphics->getTableBitmap(spriteRankSmall.palette, score.ranks.combo - 1);

        const int livesLeft = textLeft;
        const int timeLeftLeft = textLeft + kSmallRankWidth[score.ranks.lives] + 2;
        const int comboLeft = timeLeftLeft + kSmallRankWidth[score.ranks.timeLeft] + 2;

        playdate->graphics->drawBitmap(lives, livesLeft, 26, kBitmapUnflipped);
        playdate->graphics->drawBitmap(timeLeft, timeLeftLeft, 26, kBitmapUnflipped);
        playdate->graphics->drawBitmap(combo, comboLeft, 26, kBitmapUnflipped);
    }
}

static LCDBitmap * _Nonnull draw(RankingItem * _Nonnull self, LCDBitmap * _Nullable oldBitmap) {
    const MELSize size = self->super.frame.size;

    int oldBitmapWidth = 0;
    int oldBitmapHeight = 0;
    if (oldBitmap != NULL) {
        playdate->graphics->getBitmapData(oldBitmap, &oldBitmapWidth, &oldBitmapHeight, NULL, NULL, NULL);
    }

    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->setFont(romajiFont);

    const LCDColor backgroundColor = self->faces == NULL ? kColorClear : kColorWhite;

    // Création du bitmap
    LCDBitmap *bitmap;
    if (oldBitmapWidth == size.width && oldBitmapHeight == size.height) {
        bitmap = oldBitmap;
        playdate->graphics->pushContext(bitmap);
        playdate->graphics->fillRect(0, 0, size.width, size.height, backgroundColor);
    } else {
        bitmap = playdate->graphics->newBitmap(size.width, size.height, backgroundColor);
        playdate->graphics->pushContext(bitmap);

        if (oldBitmap != NULL) {
            playdate->graphics->freeBitmap(oldBitmap);
        }
    }

    if (self->isEditable) {
        drawEditable(self);
    } else {
        drawReadOnly(self);
    }

    playdate->graphics->popContext();
    return bitmap;
}

#pragma mark - Public methods

LCDSprite * _Nonnull RankingItemConstructor(const int y, const Score score, const int lastRank, LCDBitmapTable * _Nullable faces) {
    RankingItem *self = playdate->system->realloc(NULL, sizeof(RankingItem));

    *self = (RankingItem) {
        .super = {
            .class = &RankingItemClass,
            .frame = {
                .size = {
                    .width = kRankingItemWidth,
                    .height = kRankingItemHeight
                },
                .origin = {
                    .x = kRankingItemWidth / 2,
                    .y = y
                }
            },
        },
        .score = score,
        .lastRank = lastRank,
        .faces = faces,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setImage(sprite, draw(self, NULL), kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->addSprite(sprite);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push RankingItemConstructor(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

const MELSpriteClass * _Nonnull RankingItemGetClass(void) {
    return &RankingItemClass;
}

MELBoolean isRankingItem(MELSprite * _Nonnull self) {
    return self->class == &RankingItemClass;
}
