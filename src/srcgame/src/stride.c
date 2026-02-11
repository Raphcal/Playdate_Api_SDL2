#include "stride.h"

static const float kMultiplier = 1000.0f;

static void update(LCDSprite * _Nonnull sprite);
static float getDuration(MELSprite * _Nonnull self);
static void setDuration(MELSprite * _Nonnull self, float duration);
static float getDelay(MELSprite * _Nonnull self);
static void setDelay(MELSprite * _Nonnull self, float delay);

void StrideSpriteTo(LCDSprite * _Nonnull sprite, MELPoint to, float delay, float duration) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELPoint from = self->frame.origin;
    self->frame.size = (MELSize) {
        .width = to.x - from.x,
        .height = to.y - from.y
    };
    self->hitTimer = 0.0f;
    setDelay(self, delay);
    setDuration(self, delay + duration);
    playdate->sprite->setUpdateFunction(sprite, update);
}

LCDSprite * _Nonnull StrideConstructor(MELSpriteDefinition * _Nonnull definition, MELPoint from, MELPoint to, float delay, float duration) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    LCDSprite *sprite = MELSpriteInitWithCenter(self, definition, from);
    StrideSpriteTo(sprite, to, delay, duration);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push StrideConstructor(%x, %x): %d", sprite, self, self->definition.name);
#endif
    return sprite;
}

void StrideTogetherAlignedRight(LCDSprite * _Nullable sprite, LCDSprite * _Nullable spriteToFollow) {
    if (!sprite || !spriteToFollow) {
        return;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELSprite *other = playdate->sprite->getUserdata(spriteToFollow);

    PDRect selfBounds = playdate->sprite->getBounds(sprite);
    PDRect otherBounds = playdate->sprite->getBounds(spriteToFollow);
    MELRectangle otherFrame = other->frame;
    self->frame = (MELRectangle) {
        .origin = {
            .x = otherFrame.origin.x + otherBounds.width / 2.0f + selfBounds.width / 2.0f,
            .y = otherFrame.origin.y
        },
        .size = otherFrame.size
    };
    self->hitTimer = other->hitTimer;
    self->score = other->score;
    self->hitPoints = other->hitPoints;
    playdate->sprite->setUpdateFunction(sprite, update);
}

void StrideSkip(LCDSprite * _Nullable sprite) {
    if (!sprite || StrideIsDone(sprite)) {
        return;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    PDRect bounds = playdate->sprite->getBounds(sprite);
    MELRectangle frame = self->frame;
    frame = (MELRectangle) {
        .origin = {
            .x = frame.origin.x + frame.size.width,
            .y = frame.origin.y + frame.size.height,
        },
        .size = {
            .width = bounds.width,
            .height = bounds.height
        }
    };
    self->frame = frame;
    self->hitTimer = 0.0f;
    setDelay(self, 0.0f);
    setDuration(self, 0.0f);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
}

void StrideResume(LCDSprite * _Nonnull sprite) {
    playdate->sprite->setUpdateFunction(sprite, update);
}

MELBoolean StrideIsDone(LCDSprite * _Nullable sprite) {
    if (!sprite) {
        return true;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    return getDuration(self) == 0.0f;
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    MELAnimation *animation = self->animation;
    if (animation) {
        MELAnimationUpdate(animation, DELTA);
        playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), MELDirectionFlip[self->direction]);
    }

    MELRectangle frame = self->frame;
    const float duration = getDuration(self);
    float time = self->hitTimer;
    if (time < duration) {
        self->hitTimer = time = MELFloatMin(time + DELTA, duration);
        const float progress = MELEaseInOut(getDelay(self), duration, time);

        const float x = frame.origin.x + frame.size.width * progress;
        const float y = frame.origin.y + frame.size.height * progress;
        playdate->sprite->moveTo(sprite, x, y);
        return;
    }
    StrideSkip(sprite);
}

static float getDuration(MELSprite * _Nonnull self) {
    return self->hitPoints / kMultiplier;
}
static void setDuration(MELSprite * _Nonnull self, float duration) {
    self->hitPoints = duration * kMultiplier;
}

static float getDelay(MELSprite * _Nonnull self) {
    return self->score / kMultiplier;
}

static void setDelay(MELSprite * _Nonnull self, float delay) {
    self->score = delay * kMultiplier;
}
