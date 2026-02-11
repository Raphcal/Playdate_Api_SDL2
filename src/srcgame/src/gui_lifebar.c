//
//  gui_lifebar.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/03/2023.
//

#include "gui_lifebar.h"

#include "gamescene.h"
#include "../gen/spritebossicon.h"

static const int kWidth = 8;
static const int kHeight = 160;
static const int x = LCD_COLUMNS - kWidth * 2;

static const int kInitialHeight = 2;
static const float kAppearingDuration = 1.0f;

static const float kEnemyIconWidth = 32.0f;
static const float kEnemyIconHeight = 40.0f;

const LCDColor kDiagonalsPattern = (uintptr_t) (LCDPattern) {
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
    0b00010001,
    0b00100010,
    0b01000100,
    0b10001000,
    0b00010001,
    0b00100010,
    0b01000100,
    0b10001000,
};

typedef struct {
    MELSprite super;
    float time;
    int maxHitPoints;
    const int * _Nonnull hitPoints;
    int oldHitPoints;
    MELSubSprite * _Nullable enemyIcon;
    MELBoolean isAppearing;
} GUILifebar;

static void destroy(LCDSprite * _Nonnull sprite);
static void updateAppearing(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void draw(LCDSprite * _Nonnull sprite, PDRect bounds, PDRect drawRect);

static const MELSpriteClass GUILifebarClass = (MELSpriteClass) {
    .destroy = destroy,
};

static void setHeight(LCDSprite * _Nonnull sprite, int height) {
    playdate->sprite->setBounds(sprite, PDRectMake(x, (LCD_ROWS - height) / 2, kWidth, height));
}

static void destroy(LCDSprite * _Nonnull sprite) {
    GUILifebar *self = playdate->sprite->getUserdata(sprite);
    if (self->enemyIcon) {
        playdate->sprite->setUpdateFunction(self->enemyIcon->sprite, MELSubSpriteDealloc);
        self->enemyIcon = NULL;
    }
    MELSpriteDeallocFromGameSceneOthers(sprite);
}

static void updateAppearing(LCDSprite * _Nonnull sprite) {
    GUILifebar *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float time = self->time;
    if (time < kAppearingDuration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, kAppearingDuration, newTime);
        self->time = newTime;

        int height = kHeight * progress;
        setHeight(sprite, height);
        self->enemyIcon->super.frame.origin.y = LCD_ROWS + kEnemyIconHeight - (kEnemyIconHeight + 25.0f) * progress;
        return;
    }
    self->enemyIcon->super.frame.origin.y = LCD_ROWS - 25.0f;
    self->isAppearing = false;
    playdate->sprite->setUpdateFunction(sprite, &update);
}

static void update(LCDSprite * _Nonnull sprite) {
    GUILifebar *self = playdate->sprite->getUserdata(sprite);
    const int hitPoints = MELIntMax(*self->hitPoints, 0);
    if (hitPoints != self->oldHitPoints) {
        self->oldHitPoints = hitPoints;
        playdate->sprite->markDirty(sprite);
    }
}

static void draw(LCDSprite * _Nonnull sprite, PDRect bounds, PDRect drawRect) {
    GUILifebar *self = playdate->sprite->getUserdata(sprite);

    playdate->graphics->setDrawMode(kDrawModeCopy);
    float percent = (float) self->oldHitPoints / self->maxHitPoints;
    if (percent < 1.0f) {
        // TODO: Faire des -1 partout pour éviter de dessiner sous les traits ?
        const int height = bounds.height * MELFloatMax(percent, 0.0f);
        playdate->graphics->fillRect(x, bounds.y, kWidth, bounds.height - height, kDiagonalsPattern);
        playdate->graphics->fillRect(x, bounds.y + bounds.height - height, kWidth, height, kColorBlack);
    } else {
        playdate->graphics->fillRect(x, bounds.y, kWidth, bounds.height, kColorBlack);
    }
    playdate->graphics->drawRect(x, bounds.y, kWidth, bounds.height, kColorWhite);
}

LCDSprite * _Nonnull GUILifebarConstructor(const int maxHitPoints, const int * _Nonnull hitPoints) {
    GUILifebar *self = playdate->system->realloc(NULL, sizeof(GUILifebar));

    loadSpriteBossIconPalette();

    *self = (GUILifebar) {
        .super = {
            .class = &GUILifebarClass,
        },
        .maxHitPoints = maxHitPoints,
        .hitPoints = hitPoints,
        .oldHitPoints = *hitPoints,
        .isAppearing = true,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    setHeight(sprite, kInitialHeight);
    playdate->sprite->setUpdateFunction(sprite, &updateAppearing);
    playdate->sprite->setDrawFunction(sprite, &draw);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_GUI);
    playdate->sprite->addSprite(sprite);

    self->enemyIcon = MELSubSpriteAlloc(sprite, &spriteBossIcon, AnimationNameStand);
    self->enemyIcon->super.frame.origin = (MELPoint) {
        .x = LCD_COLUMNS - kEnemyIconWidth / 2.0f - 2.0f,
        .y = LCD_ROWS + kEnemyIconHeight
    };

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push GUILifeBar(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    GameScene *gameScene = GameSceneGetCurrent();
    LCDSpriteRefListPush(&gameScene->others, sprite);
    return sprite;
}

void GUILifebarSave(LCDSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    GUILifebar *self = playdate->sprite->getUserdata(sprite);
    MELOutputStreamWriteFloat(outputStream, self->time);
    MELOutputStreamWriteBoolean(outputStream, self->isAppearing);
}

LCDSprite * _Nonnull GUILifebarLoad(MELInputStream * _Nonnull inputStream, const int maxHitPoints, const int * _Nonnull hitPoints) {
    LCDSprite *sprite = GUILifebarConstructor(maxHitPoints, hitPoints);
    GUILifebar *self = playdate->sprite->getUserdata(sprite);
    self->time = MELInputStreamReadFloat(inputStream);
    self->isAppearing = MELInputStreamReadBoolean(inputStream);
    if (self->isAppearing) {
        const float progress = MELEaseInOut(0, kAppearingDuration, self->time);
        setHeight(sprite, kHeight * progress);
        playdate->sprite->setUpdateFunction(sprite, updateAppearing);
    } else {
        setHeight(sprite, kHeight);
        self->enemyIcon->super.frame.origin.y = LCD_ROWS - 25.0f;
        playdate->sprite->setUpdateFunction(sprite, update);
    }
    return sprite;
}
