//
//  charactersprite.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 07/01/2024.
//

#include "charactersprite.h"

#include "../gen/spritelargeportaits.h"
#include "../gen/spritelessonimages.h"

static const float kActiveX[SideCount] = {97.0f, 303.0f};
static const float kInactiveX[SideCount] = {65.0f, 335.0f};

static const float kImageActiveX = 106.0f;

const LCDColor kInactivePattern = (uintptr_t) (LCDPattern) {
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
    0b10001000,
    0b00100010,
    0b10001000,
    0b00100010,
    0b10001000,
    0b00100010,
    0b10001000,
    0b00100010,
};

static void onDialogChanged(void * _Nonnull userdata, int index);
static void onDialogChangedForImages(void * _Nonnull userdata, int index);
static void setAnimation(MELSprite * _Nonnull self, AnimationName animationName, MELAnimationDirection direction);
static void setNoAnimation(MELSprite * _Nonnull self);
static void update(LCDSprite * _Nonnull sprite);
static void updateMoving(LCDSprite * _Nonnull sprite);
static void destroy(LCDSprite * _Nonnull sprite);

static const MELSpriteClass CharacterSpriteClass = (MELSpriteClass) {
    .destroy = destroy,
};

LCDSprite * _Nonnull CharacterSpriteMake(MELSpriteInstance * _Nonnull instance, Side side, PhraseList dialog) {
    loadSpriteLargePortaitsPalette();

    CharacterSprite *self = playdate->system->realloc(NULL, sizeof(CharacterSprite));
    LCDSprite *sprite = MELSpriteInit(&self->super, &spriteLargePortaits, instance);
    self->super.class = &CharacterSpriteClass;
    self->inactiveImage = playdate->graphics->newBitmap(spriteLargePortaits.size.width, spriteLargePortaits.size.height, kColorClear);
    self->side = side;
    self->character = CharacterNone;
    self->dialog = dialog;
    self->isActive = false;
    MELPoint origin = self->super.frame.origin;
    origin.x = kInactiveX[side];
    self->super.frame.origin = origin;

    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setZIndex(sprite, ZINDEX_PORTAITS);
    playdate->sprite->setVisible(sprite, false);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    MELEventBusAddListener(EventDialogChanged, onDialogChanged, sprite);
    return sprite;
}

LCDSprite * _Nonnull CharacterSpriteMakeImage(MELSpriteInstance * _Nonnull instance, PhraseList dialog) {
    loadSpriteLessonImagesPalette();

    CharacterSprite *self = playdate->system->realloc(NULL, sizeof(CharacterSprite));
    LCDSprite *sprite = MELSpriteInit(&self->super, &spriteLessonImages, instance);
    self->super.class = &CharacterSpriteClass;
    self->inactiveImage = NULL;
    self->side = SideLeft;
    self->character = CharacterNone;
    self->animationName = 0;
    self->direction = 0;
    self->dialog = dialog;
    self->isActive = false;
    MELPoint origin = self->super.frame.origin;
    origin.x = -spriteLessonImages.size.width / 2.0f;
    self->super.frame.origin = origin;

    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setZIndex(sprite, ZINDEX_PORTAITS);
    playdate->sprite->setVisible(sprite, false);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    MELEventBusAddListener(EventDialogChanged, onDialogChangedForImages, sprite);
    return sprite;
}

static void destroy(LCDSprite * _Nonnull sprite) {
    CharacterSprite *self = playdate->sprite->getUserdata(sprite);
    if (self->inactiveImage) {
        playdate->graphics->freeBitmap(self->inactiveImage);
        self->inactiveImage = NULL;
    }
    MELEventBusRemoveListeners(sprite);
    MELSpriteDealloc(sprite);
}

static void greyOutAnimation(CharacterSprite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimation *animation = self->super.animation;
    if (animation == NULL) {
        // L'animation est déjà grisée ou elle est vide.
        return;
    }

    const struct playdate_graphics *graphics = playdate->graphics;
    LCDBitmap *inactiveImage = self->inactiveImage;
    int width, height;
    graphics->getBitmapData(inactiveImage, &width, &height, NULL, NULL, NULL);

    LCDBitmap *frame = graphics->getTableBitmap(self->super.definition.palette, animation->definition->frames[0].atlasIndex);

    LCDBitmap *mask = graphics->newBitmap(width, height, kColorClear);
    graphics->clearBitmap(mask, kColorClear);
    graphics->pushContext(mask);
    graphics->setDrawMode(kDrawModeFillWhite);
    graphics->drawBitmap(frame, 0, 0, kBitmapUnflipped);
    graphics->popContext();

    graphics->clearBitmap(inactiveImage, kColorClear);
    graphics->pushContext(inactiveImage);
    graphics->setDrawMode(kDrawModeCopy);
    graphics->drawBitmap(frame, 0, 0, kBitmapUnflipped);
    graphics->setDrawMode(kDrawModeBlackTransparent);
    graphics->fillRect(0, 0, width, height, kInactivePattern);
    graphics->popContext();
    graphics->setBitmapMask(inactiveImage, mask);

    graphics->freeBitmap(mask);

    setNoAnimation(&self->super);
}

static void onDialogChanged(void * _Nonnull userdata, int index) {
    LCDSprite *sprite = userdata;
    CharacterSprite *self = playdate->sprite->getUserdata(sprite);

    if (index >= self->dialog.count) {
        update(sprite);
        return;
    }

    const Phrase phrase = self->dialog.memory[index];
    const MELBoolean isThisSide = phrase.side == self->side;
    const MELBoolean characterChanged = phrase.character != self->character;
    if (!isThisSide && self->isActive) {
        self->start = self->super.frame.origin.x;
        self->target = kInactiveX[self->side];
        self->time = 0.0f;
        self->isActive = false;
        greyOutAnimation(self, sprite);
        playdate->sprite->setUpdateFunction(sprite, updateMoving);
    } else if (isThisSide && !characterChanged && !self->isActive) {
        self->start = self->super.frame.origin.x;
        self->target = kActiveX[self->side];
        self->time = 0.0f;
        self->isActive = true;
        setAnimation(&self->super, CharacterAnimationName[self->character], (MELAnimationDirection)phrase.expression);
        playdate->sprite->setUpdateFunction(sprite, updateMoving);
    } else if (isThisSide && characterChanged && phrase.character == CharacterNone) {
        self->character = CharacterNone;
        self->start = self->super.frame.origin.x;
        self->target = kInactiveX[self->side];
        self->time = 0.0f;
        self->isActive = false;
        greyOutAnimation(self, sprite);
        playdate->sprite->setUpdateFunction(sprite, updateMoving);
    } else if (isThisSide && characterChanged) {
        self->character = phrase.character;
        setAnimation(&self->super, CharacterAnimationName[phrase.character], (MELAnimationDirection)phrase.expression);
        self->start = self->super.frame.origin.x;
        self->target = kActiveX[self->side];
        self->time = 0.0f;
        self->isActive = true;
        playdate->sprite->setUpdateFunction(sprite, updateMoving);
        playdate->sprite->setVisible(sprite, true);
    } else if (isThisSide) {
        setAnimation(&self->super, CharacterAnimationName[phrase.character], (MELAnimationDirection)phrase.expression);
    }
    update(sprite);
}

static void onDialogChangedForImages(void * _Nonnull userdata, int index) {
    LCDSprite *sprite = userdata;
    CharacterSprite *self = playdate->sprite->getUserdata(sprite);

    if (index >= self->dialog.count) {
        update(sprite);
        return;
    }

    const MELBoolean wasDisplayingImage = self->isActive;

    const Phrase phrase = self->dialog.memory[index];
    const MELBoolean shouldDisplayImage = phrase.direction != 0;

    if (shouldDisplayImage) {
        const AnimationName animationName = phrase.animation;
        const MELAnimationDirection direction = phrase.direction - 1;
        setAnimation(&self->super, animationName, direction);
        if (!wasDisplayingImage) {
            self->start = self->super.frame.origin.x;
            self->target = kImageActiveX;
            self->time = 0.0f;
            playdate->sprite->setVisible(sprite, true);
            playdate->sprite->setUpdateFunction(sprite, updateMoving);
        }
    } else if (wasDisplayingImage) {
        self->start = self->super.frame.origin.x;
        self->target = -spriteLessonImages.size.width / 2.0f;
        self->time = 0.0f;
        playdate->sprite->setUpdateFunction(sprite, updateMoving);
    }
    self->isActive = shouldDisplayImage;

    update(sprite);
}

static void setAnimation(MELSprite * _Nonnull self, AnimationName animationName, MELAnimationDirection direction) {
    MELAnimation *currentAnimation = self->animation;
    if (currentAnimation) {
        MELAnimationDealloc(currentAnimation);
    }
    self->animationName = animationName;
    MELAnimation *anAnimation;
    anAnimation = MELSpriteDefinitionGetAnimation(self->definition, animationName, direction);
    anAnimation->class->start(anAnimation);
    self->animation = anAnimation;
}

static void setNoAnimation(MELSprite * _Nonnull self) {
    if (self->animation) {
        MELAnimationDealloc(self->animation);
        self->animation = NULL;
    }
}

static void draw(CharacterSprite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    LCDBitmap *frame;
    MELAnimation *animation = self->super.animation;
    if (animation) {
        MELAnimationUpdate(animation, DELTA);
        frame = playdate->graphics->getTableBitmap(self->super.definition.palette, animation->frame.atlasIndex);
    } else {
        frame = self->inactiveImage;
    }
    
    MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
    playdate->sprite->setImage(sprite, frame, MELDirectionFlip[self->super.direction]);
}

static void update(LCDSprite * _Nonnull sprite) {
    CharacterSprite *self = playdate->sprite->getUserdata(sprite);
    draw(self, sprite);
}

static void updateMoving(LCDSprite * _Nonnull sprite) {
    CharacterSprite *self = playdate->sprite->getUserdata(sprite);
    const float duration = 0.25f;
    if (self->time < duration) {
        const float time = self->time += DELTA;
        const float progress = MELEaseInOut(0, duration, time);
        MELPoint origin = self->super.frame.origin;
        origin.x = self->start + (self->target - self->start) * progress;
        self->super.frame.origin = origin;

        draw(self, sprite);
        return;
    }
    self->time = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, update);
    if (self->super.definition.name == SpriteNameLargePortaits && self->character == CharacterNone) {
        playdate->sprite->setVisible(sprite, false);
    }
}
