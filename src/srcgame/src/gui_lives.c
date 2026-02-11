//
//  gui_lives.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 17/03/2023.
//

#include "gui_lives.h"

#include "gamescene.h"

static void destroy(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void redrawAndMove(GUILives * _Nonnull self, LCDSprite * _Nonnull sprite, int8_t lives);

static const MELSpriteClass GUILivesClass = (MELSpriteClass) {
    .name = SpriteClassNameGUILives,
    .destroy = destroy,
};

LCDSprite * _Nonnull GUILivesConstructor(const int8_t * _Nonnull lives) {
    return GUILivesConstructorWithImagePathAndTopLeft(lives, "gui-life", (MELPoint) {
        .x = GUI_PADDING,
        .y = GUI_PADDING + SCORE_LABEL_HEIGHT + GUI_PADDING
    });
}

LCDSprite * _Nonnull GUILivesConstructorWithImagePathAndTopLeft(const int8_t * _Nonnull lives, const char * _Nonnull imagePath, MELPoint topLeft) {
    GUILives *self = playdate->system->realloc(NULL, sizeof(GUILives));
    const int8_t currentLives = *lives;
    *self = (GUILives) {
        .super = {
            .class = &GUILivesClass,
        },
        .lives = lives,
        .currentLives = currentLives,
        .lifeIcon = LCDBitmapLoadOrError(imagePath),
        .maxIcon = LCDBitmapLoadOrError("gui-max"),
        .topLeft = topLeft,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    if (currentLives > 0) {
        redrawAndMove(self, sprite, currentLives);
    } else {
        LCDBitmap *image = playdate->graphics->newBitmap(1, 1, kColorClear);
        playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    }
    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_GUI);
    playdate->sprite->addSprite(sprite);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push GUILives(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    GameScene *gameScene = GameSceneGetCurrent();
    LCDSpriteRefListPush(&gameScene->others, sprite);
    return sprite;
}

void GUILivesSetValue(LCDSprite * _Nullable sprite, int8_t lives) {
    if (!sprite) {
        return;
    }
    if (lives > 0) {
        GUILives *self = playdate->sprite->getUserdata(sprite);
        LCDBitmap *oldImage = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(oldImage);
        playdate->sprite->setVisible(sprite, true);
        redrawAndMove(self, sprite, lives);
    } else {
        playdate->sprite->setVisible(sprite, false);
    }
}

void GUILivesUpdateTopLeftWithCurrentOrigin(LCDSprite * _Nullable sprite) {
    if (!sprite) {
        return;
    }
    GUILives *self = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = self->super.frame;
    self->topLeft = (MELPoint) {
        .x = frame.origin.x - frame.size.width / 2.0f,
        .y = frame.origin.y - frame.size.height / 2.0f,
    };
}

const MELSpriteClass * _Nonnull GUILivesGetClass(void) {
    return &GUILivesClass;
}

static void destroy(LCDSprite * _Nonnull sprite) {
    GUILives *self = playdate->sprite->getUserdata(sprite);
    if (self->lifeIcon) {
        playdate->graphics->freeBitmap(self->lifeIcon);
        self->lifeIcon = NULL;
    }
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    if (image) {
        playdate->graphics->freeBitmap(image);
    }
    MELSpriteDeallocFromGameSceneOthers(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    GUILives *self = playdate->sprite->getUserdata(sprite);
    const int8_t lives = *self->lives;
    if (lives != self->currentLives) {
        self->currentLives = lives;
        if (lives <= 0) {
            destroy(sprite);
            return;
        }
        LCDBitmap *oldImage = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(oldImage);
        redrawAndMove(self, sprite, lives);
    }
}

static void redrawAndMove(GUILives * _Nonnull self, LCDSprite * _Nonnull sprite, int8_t lives) {
    const MELIntSize size = (MELIntSize) {
        .width = LIFE_ICON_WIDTH + (LIFE_ICON_WIDTH + GUI_SPACING) * (lives - 1) + (lives == MAXIMUM_LIFE_COUNT ? GUI_SPACING + LIFE_MAX_WIDTH : 0),
        .height = LIFE_ICON_HEIGHT
    };

    // Redessine l'image.
    LCDBitmap *image = playdate->graphics->newBitmap(size.width, size.height, kColorClear);
    playdate->graphics->pushContext(image);
    playdate->graphics->setDrawMode(kDrawModeCopy);

    LCDBitmap *lifeIcon = self->lifeIcon;
    for (int x = 0; x < size.width; x += LIFE_ICON_WIDTH + GUI_SPACING) {
        playdate->graphics->drawBitmap(lifeIcon, x, 0, kBitmapUnflipped);
    }

    if (lives == MAXIMUM_LIFE_COUNT) {
        playdate->graphics->drawBitmap(self->maxIcon, (LIFE_ICON_WIDTH + GUI_SPACING) * lives + GUI_SPACING, 0, kBitmapUnflipped);
    }

    playdate->graphics->popContext();

    // Met à jour le sprite.
    MELPoint topLeft = self->topLeft;
    MELRectangle frame;
    self->super.frame = frame = (MELRectangle) {
        .origin = {
            .x = topLeft.x + size.width / 2,
            .y = topLeft.y + LIFE_ICON_HEIGHT / 2
        },
        .size = {
            .width = size.width,
            .height = size.height
        }
    };
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setSize(sprite, size.width, size.height);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->markDirty(sprite);
}
