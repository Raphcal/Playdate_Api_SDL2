//
//  scoreboard.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 10/11/2024.
//

#include "scoreboard.h"

#include "scores.h"
#include "savegame.h"
#include "card.h"

#include "../gen/spritefileselectcard.h"

#define kValueAsStringLength 50

static const MELLocalizedString kSaveGameFormat = (MELLocalizedString) {
    .text = {
        "Save Game %d",
        "Sauvegarde %d",
        "セーブ %d",
        "Jogo %d",
    }
};

static const MELLocalizedString kPlayerAndSaveGameFormat = (MELLocalizedString) {
    .text = {
        "%s (save %d)",
        "%s (svgde %d)",
        "%s (セーブ%d)",
        "%s (jogo %d)",
    }
};

typedef uint32_t UInt32;
typedef SaveGame * _Nullable SaveGameRef;
MELKeyValueTableDefine(UInt32, SaveGameRef);
MELKeyValueTableImplement(UInt32, SaveGameRef);

typedef struct {
    MELSprite super;
    uint32_t lastUpdate;
    SaveGameList saveGames;
    UInt32SaveGameRefTable saveGameByUuid;
    float top;
    ScoreList * _Nonnull scores;
} Scoreboard;

static void dealloc(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);

static void repaint(Scoreboard * _Nonnull self, LCDSprite * _Nonnull sprite);

static const MELSpriteClass ScoreboardClass = (MELSpriteClass) {
    .name = SpriteClassNameScoreboard,
    .destroy = dealloc,
    .update = update,
};

LCDSprite * _Nullable ScoreboardConstructor(ScoreList * _Nonnull scores, float top) {
    Scoreboard *self = new(Scoreboard);
    *self = (Scoreboard) {
        .super = {
            .class = &ScoreboardClass,
            .frame = {
                .origin = {
                    .x = LCD_COLUMNS / 2.0f,
                    .y = top
                },
                .size = {
                    .width = LCD_COLUMNS,
                    .height = 0.0f
                }
            }
        },
        .top = top,
        .scores = scores,
    };

    SaveGameList saveGames = SaveGameListMakeWithInitialCapacity(kMaximumSaveCount);
    SavedGameLoad(&saveGames);
    self->saveGames = saveGames;

    for (unsigned index = 0; index < saveGames.count; index++) {
        SaveGame *saveGame = saveGames.memory + index;
        const UInt32 key = MELUUIDGetHashcode(saveGame->uuid);
        UInt32SaveGameRefTablePut(&self->saveGameByUuid, key, saveGame);
    }

    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->setUserdata(sprite, self);
    spriteAPI->setUpdateFunction(sprite, update);
    spriteAPI->addSprite(sprite);

    repaint(self, sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

void ScoreboardRepaint(LCDSprite * _Nonnull sprite) {
    Scoreboard *self = playdate->sprite->getUserdata(sprite);
    repaint(self, sprite);
}

float ScoreboardGetTop(LCDSprite * _Nonnull sprite) {
    Scoreboard *self = playdate->sprite->getUserdata(sprite);
    return self->top;
}

void ScoreboardSetTop(LCDSprite * _Nonnull sprite, float top) {
    Scoreboard *self = playdate->sprite->getUserdata(sprite);
    self->top = top;
    
    MELRectangle frame = self->super.frame;
    frame.origin.y = top + frame.size.height / 2;
    self->super.frame = frame;

    playdate->sprite->moveTo(sprite, frame.origin.x - camera.frame.origin.x, frame.origin.y  - camera.frame.origin.y);
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    Scoreboard *self = playdate->sprite->getUserdata(sprite);

    LCDBitmap *image = playdate->sprite->getImage(sprite);
    if (image) {
        playdate->graphics->freeBitmap(image);
    }

    UInt32SaveGameRefTableDeinit(&self->saveGameByUuid);
    SaveGameListDeinit(&self->saveGames);

    MELSpriteDealloc(sprite);
}

static void repaint(Scoreboard * _Nonnull self, LCDSprite * _Nonnull sprite) {
    loadSpriteFileSelectCardPalette();

    LCDBitmap *oldImage = playdate->sprite->getImage(sprite);
    if (oldImage) {
        playdate->system->realloc(oldImage, 0);
    }

    const ScoreList scores = *self->scores;

    static const int cellHeight = 32;

    const int width = LCD_COLUMNS;
    const int height = scores.count ? cellHeight * scores.count : cellHeight;

    struct playdate_graphics gfx = *playdate->graphics;
    LCDBitmap *bitmap = gfx.newBitmap(width, height, kColorWhite);

    gfx.pushContext(bitmap);
    gfx.setDrawMode(kDrawModeCopy);

    char valueAsString[kValueAsStringLength] = {};

    static const int fontHeight = 16;
    static const int horizontalPadding = 20;
    static const int verticalPadding = (cellHeight - fontHeight) / 2;

    int top = 0;
    for (unsigned int index = 0; index < scores.count; index++) {
        Score score = scores.memory[index];

        gfx.setDrawMode(kDrawModeCopy);
        LCDBitmapDrawMode drawMode = kDrawModeCopy;
        LCDColor backgroundColor = kColorWhite;

        if (MELUUIDIsEmpty(score.playerUUID)) {
            gfx.setFont(fontRoobert11MonoMediumNumerals);
            MELUInt32ToStringWithFixedSizeBuffer(score.super.rank, valueAsString, kValueAsStringLength);
            const int left = gfx.getTextWidth(fontRoobert11MonoMediumNumerals, valueAsString, kValueAsStringLength, kUTF8Encoding, 0) + horizontalPadding + 16;
            gfx.drawText(valueAsString, kValueAsStringLength, kUTF8Encoding, horizontalPadding, top + verticalPadding);

            gfx.setFont(currentFont);
            if (score.super.player) {
                gfx.drawText(score.super.player, 99, kUTF8Encoding, left, top + verticalPadding);
            } else {
                gfx.drawText("Error: name is NULL", 99, kUTF8Encoding, left, top + verticalPadding);
            }
        } else {
            if (MELUUIDEquals(score.playerUUID, currentSaveGame.uuid)) {
                backgroundColor = kColorBlack;
                drawMode = kDrawModeInverted;
                gfx.fillRect(0, top + 1, width, cellHeight - 2, kColorBlack);
                gfx.setDrawMode(kDrawModeInverted);
            }

            int left = horizontalPadding;
            if (score.super.rank) {
                gfx.setFont(fontRoobert11MonoMediumNumerals);
                MELUInt32ToStringWithFixedSizeBuffer(score.super.rank, valueAsString, kValueAsStringLength);
                left += gfx.getTextWidth(fontRoobert11MonoMediumNumerals, valueAsString, kValueAsStringLength, kUTF8Encoding, 0) + 16;
                gfx.drawText(valueAsString, kValueAsStringLength, kUTF8Encoding, horizontalPadding, top + verticalPadding);
            }

            const uint32_t hash = MELUUIDGetHashcode(score.playerUUID);
            SaveGame *saveGame = NULL;
            if (UInt32SaveGameRefTableGet(self->saveGameByUuid, hash, &saveGame)) {
                const Card card = saveGame->card;
                MELAnimationDefinition *suit = MELSpriteDefinitionGetAnimationDefinition(spriteFileSelectCard, card.suit + 1, MELAnimationDirectionRight);
                LCDBitmap *cardImage = gfx.getTableBitmap(spriteFileSelectCard.palette, suit->frames[card.rank].atlasIndex);

                gfx.setDrawMode(kDrawModeCopy);
                gfx.tileBitmap(cardImage, left, top + (cellHeight - 24) / 2, spriteFileSelectCard.size.width, 24, kBitmapUnflipped);
                gfx.setDrawMode(drawMode);

                char *playerName = NULL;
                if (score.super.player) {
                    playdate->system->formatString(&playerName, kPlayerAndSaveGameFormat.text[currentLanguage], score.super.player, saveGame->index + 1);
                } else {
                    playdate->system->formatString(&playerName, kSaveGameFormat.text[currentLanguage], saveGame->index + 1);

                }
                gfx.setFont(currentFont);
                gfx.drawText(playerName, 99, kUTF8Encoding, left + spriteFileSelectCard.size.width + 8, top + verticalPadding);
                playdate->system->realloc(playerName, 0);
            }
        }

        gfx.setFont(fontRoobert11MonoMediumNumerals);
        MELUInt32ToStringWithFixedSizeBuffer(score.super.value, valueAsString, kValueAsStringLength);
        const int textWidth = gfx.getTextWidth(fontRoobert11MonoMediumNumerals, valueAsString, kValueAsStringLength, kUTF8Encoding, 0);
        gfx.fillRect(width - textWidth - horizontalPadding - 8, top + 1, textWidth + horizontalPadding + 8, cellHeight - 2, backgroundColor);
        gfx.drawText(valueAsString, kValueAsStringLength, kUTF8Encoding, width - textWidth - horizontalPadding, top + verticalPadding);

        top += cellHeight;
    }
    gfx.popContext();

    playdate->sprite->setImage(sprite, bitmap, kBitmapUnflipped);

    MELRectangle frame = self->super.frame;
    frame.origin.y = self->top + height / 2;
    frame.size.height = scores.count ? height : 0;
    self->super.frame = frame;

    playdate->sprite->moveTo(sprite, frame.origin.x - camera.frame.origin.x, frame.origin.y  - camera.frame.origin.y);
}

static void update(LCDSprite * _Nonnull sprite) {
    Scoreboard *self = playdate->sprite->getUserdata(sprite);

    if (self->lastUpdate != scoreLastUpdateDate) {
        repaint(self, sprite);
        self->lastUpdate = scoreLastUpdateDate;
    }

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
}
