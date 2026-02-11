//
//  gui_score.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 17/03/2023.
//

#include "gui_score.h"

#include "gamescene.h"

static const float left = GUI_PADDING;
static const float top = GUI_PADDING;

typedef struct {
    MELSprite super;
    LCDBitmap * _Nullable scoreLabel;
    const unsigned int * _Nonnull score;
    unsigned int currentScore;
} GUIScore;

static void destroy(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void redrawAndMove(GUIScore * _Nonnull self, LCDSprite * _Nonnull sprite);

static const MELSpriteClass GUIScoreClass = (MELSpriteClass) {
    .destroy = destroy,
};

static void destroy(LCDSprite * _Nonnull sprite) {
    GUIScore *self = playdate->sprite->getUserdata(sprite);
    if (self->scoreLabel) {
        playdate->graphics->freeBitmap(self->scoreLabel);
        self->scoreLabel = NULL;
    }
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    if (image) {
        playdate->graphics->freeBitmap(image);
    }
    MELSpriteDeallocFromGameSceneOthers(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    GUIScore *self = playdate->sprite->getUserdata(sprite);
    const unsigned int score = *self->score;
    if (score != self->currentScore) {
        self->currentScore = score;
        redrawAndMove(self, sprite);
    }
}

static void redrawAndMove(GUIScore * _Nonnull self, LCDSprite * _Nonnull sprite) {
    char *scoreText = MELUInt32ToString(self->currentScore);

    // Calcul de la taille.
    const int textWidth = playdate->graphics->getTextWidth(numbersFont, scoreText, 99, kASCIIEncoding, 0);
    MELSize size = (MELSize) {
        .width = SCORE_LABEL_WIDTH + GUI_PADDING + textWidth,
        .height = SCORE_LABEL_HEIGHT
    };
    MELPoint origin = (MELPoint) {
        .x = left + size.width / 2.0f,
        .y = top + size.height / 2.0f
    };
    self->super.frame = (MELRectangle) {
        .origin = origin,
        .size = size
    };

    // Redimensionnement et effaçage de l'image.
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    if (image == NULL) {
        image = playdate->graphics->newBitmap(size.width, size.height, kColorClear);
    } else if (LCDBitmapGetWidth(image) < size.width) {
        playdate->graphics->freeBitmap(image);
        image = playdate->graphics->newBitmap(size.width, size.height, kColorClear);
    } else {
        playdate->graphics->clearBitmap(image, kColorClear);
    }

    // Dessin.
    playdate->graphics->pushContext(image);
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->setFont(numbersFont);
    playdate->graphics->drawBitmap(self->scoreLabel, 0, 0, kBitmapUnflipped);
    playdate->graphics->drawText(scoreText, 99, kASCIIEncoding, SCORE_LABEL_WIDTH + GUI_PADDING, DIGIT_PADDING_TOP);
    playdate->graphics->popContext();

    playdate->system->realloc(scoreText, 0);

    // Mise à jour du sprite.
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setSize(sprite, size.width, size.height);
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->markDirty(sprite);
}

LCDSprite * _Nonnull GUIScoreConstructor(const unsigned int * _Nonnull score) {
    GUIScore *self = playdate->system->realloc(NULL, sizeof(GUIScore));
    *self = (GUIScore) {
        .super = {
            .class = &GUIScoreClass,
        },
        .score = score,
        .currentScore = *score,
        .scoreLabel = LCDBitmapLoadOrError("gui-score"),
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    redrawAndMove(self, sprite);
    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_GUI);
    playdate->sprite->addSprite(sprite);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push GUIScore(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    GameScene *gameScene = GameSceneGetCurrent();
    LCDSpriteRefListPush(&gameScene->others, sprite);
    return sprite;
}
