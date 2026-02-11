//
//  gui_time.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 17/03/2023.
//

#include "gui_time.h"

#include "gamescene.h"

typedef struct {
    MELSprite super;
    const MELTimeInterval * _Nonnull elapsed;
    int oldElapsed;
    LCDBitmap * _Nullable remainingLabel;
} GUITime;

static void destroy(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void draw(GUITime *self, LCDSprite * _Nonnull sprite);

static const MELSpriteClass GUITimeClass = (MELSpriteClass) {
    .destroy = destroy,
};

const MELSpriteClass * _Nonnull GUITimeGetClass(void) {
    return &GUITimeClass;
}

static void destroy(LCDSprite * _Nonnull sprite) {
    GUITime *self = playdate->sprite->getUserdata(sprite);
    if (self->remainingLabel) {
        playdate->graphics->freeBitmap(self->remainingLabel);
        self->remainingLabel = NULL;
    }
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    if (image) {
        playdate->graphics->freeBitmap(image);
    }
    MELSpriteDeallocFromGameSceneOthers(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    GUITime *self = playdate->sprite->getUserdata(sprite);
    const int elapsed = MELFloatMin(*self->elapsed, 3.0f * 60.0f);
    if (elapsed != self->oldElapsed) {
        self->oldElapsed = elapsed;
        LCDBitmap *image = playdate->sprite->getImage(sprite);
        playdate->graphics->clearBitmap(image, kColorClear);
        draw(self, sprite);
    }
}

static void draw(GUITime *self, LCDSprite * _Nonnull sprite) {
    const int remaining = GAME_TOTAL_TIME_IN_SECONDS - self->oldElapsed;
    const int minutes = remaining / SECONDS_IN_A_MINUTE;
    const int seconds = remaining - minutes * SECONDS_IN_A_MINUTE;

    const char timeText[] = {
        '0' + minutes, ':', '0' + (seconds / 10), '0' + (seconds % 10) , '\0'
    };

    LCDBitmap *image = playdate->sprite->getImage(sprite);

    playdate->graphics->pushContext(image);
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->setFont(numbersFont);
    playdate->graphics->drawBitmap(self->remainingLabel, 0, 0, kBitmapUnflipped);
    playdate->graphics->drawText(timeText, 4, kASCIIEncoding, REMAINING_LABEL_WIDTH + GUI_PADDING, DIGIT_PADDING_TOP);
    playdate->graphics->popContext();

    playdate->sprite->markDirty(sprite);
}

LCDSprite * _Nonnull GUITimeConstructor(const MELTimeInterval * _Nonnull elapsed) {
    GUITime *self = playdate->system->realloc(NULL, sizeof(GUITime));

    const float left = LCD_COLUMNS - REMAINING_LABEL_WIDTH - GUI_PADDING - TIME_WIDTH - GUI_PADDING;
    const float top = GUI_PADDING;
    const float width = REMAINING_LABEL_WIDTH + GUI_PADDING + TIME_WIDTH;
    const float height = REMAINING_LABEL_HEIGHT;
    const float x = left + width / 2.0f;
    const float y = top + height / 2.0f;

    *self = (GUITime) {
        .super = {
            .class = &GUITimeClass,
            .frame = {
                .origin = {
                    .x = x,
                    .y = y
                },
                .size = {
                    .width = width,
                    .height = height
                }
            }
        },
        .elapsed = elapsed,
        .oldElapsed = *elapsed,
        .remainingLabel = LCDBitmapLoadOrError("gui-remaining"),
    };

    LCDBitmap *image = playdate->graphics->newBitmap(width, height, kColorClear);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    draw(self, sprite);
    playdate->sprite->moveTo(sprite, x, y);
    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_GUI);
    playdate->sprite->addSprite(sprite);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push GUITime(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    GameScene *gameScene = GameSceneGetCurrent();
    LCDSpriteRefListPush(&gameScene->others, sprite);

    return sprite;
}
