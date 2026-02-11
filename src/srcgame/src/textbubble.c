//
//  textbubble.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 01/09/2023.
//

#include "textbubble.h"

#include "camera.h"

typedef struct {
    MELSprite super;
    LCDBitmap * _Nullable bubble;
    LCDBitmap * _Nullable text;
    MELIntRectangle textClip;
    float time;
    float scrollY;
    float oldScrollY;
    float maxScrollY;
    float zoom;
    float oldZoom;
    MELDirection zoomDirection;
} TextBubble;

static void update(LCDSprite * _Nonnull sprite);
static void updateAppearing(LCDSprite * _Nonnull sprite);
static void updateDisappearing(LCDSprite * _Nonnull sprite);
static void destroy(LCDSprite * _Nonnull sprite);
static void draw(TextBubble * _Nonnull self, LCDSprite * _Nonnull sprite);

static const MELSpriteClass TextBubbleClass = (MELSpriteClass) {
    .destroy = destroy,
};

static const int kPaddingTop = 8;
static const int kPaddingLeft = 10;
static const int kPaddingRight = 10;
static const int kPaddingBottom = 2;
static const int kTextLeading = 8;
static const int kTextLength = 999;
static const float kInitialZoom = 0.75f;
static const float kMaximumZoom = 1.0f;
static const float kSlideInHeight = 32.0f;

LCDSprite * _Nonnull TextBubbleConstructor(const char * _Nonnull text, int lines, MELPoint origin) {
    TextBubble *self = playdate->system->realloc(NULL, sizeof(TextBubble));

    LCDBitmap *bubble = LCDBitmapLoadOrError("gui-text-bubble");
    int width, height;
    playdate->graphics->getBitmapData(bubble, &width, &height, NULL, NULL, NULL);

    const int textWidth = playdate->graphics->getTextWidth(rains2xFont, text, kTextLength, kASCIIEncoding, 0);
    const int textHeight = (FONT_RAINS2X_HEIGHT + kTextLeading) * lines;
    LCDBitmap *textBitmap = playdate->graphics->newBitmap(textWidth, textHeight, kColorWhite);
    playdate->graphics->pushContext(textBitmap);
    playdate->graphics->setFont(rains2xFont);
    playdate->graphics->setTextLeading(kTextLeading);
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->drawText(text, kTextLength, kASCIIEncoding, 0, 0);
    playdate->graphics->popContext();

    *self = (TextBubble) {
        .super = {
            .class = &TextBubbleClass,
            .frame = {
                .origin = origin,
                .size = {
                    .width = width,
                    .height = height
                }
            }
        },
        .bubble = bubble,
        .text = textBitmap,
        .textClip = {
            .origin = {
                .x = kPaddingLeft,
                .y = kPaddingTop,
            },
            .size = {
                .width = width - kPaddingLeft - kPaddingRight,
                .height = height - kPaddingTop - kPaddingBottom,
            },
        },
        .maxScrollY = textHeight - FONT_RAINS2X_HEIGHT,
        .zoom = 0.75f,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setUserdata(sprite, self);
    draw(self, sprite);
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
    playdate->sprite->setZIndex(sprite, ZINDEX_PLAYER + 1);
    playdate->sprite->addSprite(sprite);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push TextBubbleConstructor(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

void TextBubbleAnimateAppear(LCDSprite * _Nonnull sprite) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);
#if CHECK_CLASS_CAST
    if (self->super.class != &TextBubbleClass) {
        playdate->system->error("Given sprite is not an instance of TextBubble");
    }
#endif
    self->zoom = kInitialZoom;
    self->time = 0.0f;
    self->zoomDirection = MELDirectionDown;
    update(sprite);
    playdate->sprite->setVisible(sprite, true);
    playdate->sprite->setUpdateFunction(sprite, updateAppearing);
}

void TextBubbleAnimateDisappear(LCDSprite * _Nonnull sprite) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);
#if CHECK_CLASS_CAST
    if (self->super.class != &TextBubbleClass) {
        playdate->system->error("Given sprite is not an instance of TextBubble");
    }
#endif
    self->time = 0.0f;
    self->zoomDirection = MELDirectionUp;
    playdate->sprite->setUpdateFunction(sprite, updateDisappearing);
}

float TextBubbleScroll(LCDSprite * _Nonnull sprite, float value) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);
#if CHECK_CLASS_CAST
    if (self->super.class != &TextBubbleClass) {
        playdate->system->error("Given sprite is not an instance of TextBubble");
    }
#endif
    const float scrollY = self->scrollY + value;
    if (scrollY < 0.0f) {
        self->scrollY = 0.0f;
        return scrollY;
    }
    else if (scrollY >= self->maxScrollY) {
        self->scrollY = self->maxScrollY;
        return scrollY - self->maxScrollY;
    } else {
        self->scrollY = scrollY;
        return 0.0f;
    }
}

float TextBubbleZoom(LCDSprite * _Nonnull sprite, float value, MELDirection zoomDirection) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);
#if CHECK_CLASS_CAST
    if (self->super.class != &TextBubbleClass) {
        playdate->system->error("Given sprite is not an instance of TextBubble");
    }
#endif
    self->zoomDirection = zoomDirection;
    const float zoom = self->zoom + value;
    if (zoom < kInitialZoom) {
        self->zoom = kInitialZoom;
        return zoom - kInitialZoom;
    }
    else if (zoom >= kMaximumZoom) {
        self->zoom = kMaximumZoom;
        return zoom - kMaximumZoom;
    } else {
        self->zoom = zoom;
        return 0.0f;
    }
}

static void destroy(LCDSprite * _Nonnull sprite) {
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    if (image) {
        playdate->graphics->freeBitmap(image);
    }

    TextBubble *self = playdate->sprite->getUserdata(sprite);
    playdate->graphics->freeBitmap(self->bubble);
    playdate->graphics->freeBitmap(self->text);
    MELSpriteDealloc(sprite);
}

static void draw(TextBubble * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELIntRectangle textClip = self->textClip;
    const float zoom = self->zoom;
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->setDrawMode(kDrawModeCopy);
    if (image == NULL) {
        int width, height;
        playdate->graphics->getBitmapData(self->bubble, &width, &height, NULL, NULL, NULL);
        image = playdate->graphics->newBitmap(ceilf(width * zoom), ceilf(height * zoom), kColorClear);
        playdate->graphics->pushContext(image);
        playdate->graphics->drawBitmap(self->bubble, 0, 0, kBitmapUnflipped);
    } else {
        playdate->graphics->pushContext(image);
        playdate->graphics->drawRect(textClip.origin.x * zoom, textClip.origin.y * zoom, textClip.size.width * zoom, textClip.size.height * zoom, kColorWhite);
    }
    playdate->graphics->setClipRect(textClip.origin.x * zoom, textClip.origin.y * zoom, textClip.size.width * zoom, textClip.size.height * zoom);
    playdate->graphics->drawBitmap(self->text, textClip.origin.x * zoom, (textClip.origin.y + kTextLeading - self->scrollY) * zoom, kBitmapUnflipped);
    // TODO: Faire un rectangle blanc du bas de l'image vers le bas de la bulle
    playdate->graphics->clearClipRect();
    playdate->graphics->popContext();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
}

static float slideInLocation(float zoom, MELDirection zoomDirection) {
    return (1.0f - (zoom - kInitialZoom) / (1.0f - kInitialZoom)) * kSlideInHeight * MELDirectionValues[zoomDirection];
}

static void update(LCDSprite * _Nonnull sprite) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);
    const float zoom = self->zoom;
    const float scrollY = self->scrollY;
    if (zoom != self->oldZoom) {
        self->oldZoom = zoom;
        LCDBitmap *image = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(image);
        playdate->sprite->setImage(sprite, NULL, kBitmapUnflipped);
        draw(self, sprite);
    }
    else if (scrollY != self->oldScrollY) {
        self->oldScrollY = scrollY;
        draw(self, sprite);
    }
    MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y + slideInLocation(zoom, self->zoomDirection));
}

static void updateAppearing(LCDSprite * _Nonnull sprite) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);
    const float duration = 0.4f;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseOutBack(0, duration, newTime);

        self->zoom = kInitialZoom + (kMaximumZoom - kInitialZoom) * progress;
        update(sprite);
        return;
    }
    self->zoom = kMaximumZoom;
    update(sprite);
    playdate->sprite->setUpdateFunction(sprite, update);
}

static void updateDisappearing(LCDSprite * _Nonnull sprite) {
    TextBubble *self = playdate->sprite->getUserdata(sprite);
    const float duration = 0.4f;
    if (self->time < duration) {
        const float newTime = self->time = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        self->zoom = kMaximumZoom - (kMaximumZoom - kInitialZoom) * progress;
        update(sprite);
        return;
    }
    self->zoom = kInitialZoom;
    playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    playdate->sprite->setVisible(sprite, false);
}
