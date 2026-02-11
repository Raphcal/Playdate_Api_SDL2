//
//  image.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/07/2023.
//

#include "image.h"

#include "../lib/camera.h"

static void update(LCDSprite * _Nonnull sprite);

static const MELSpriteClass ImageClass = (MELSpriteClass) {
    .destroy = ImageDealloc,
    .update = update,
};

void ImageDealloc(LCDSprite * _Nonnull sprite) {
    LCDBitmap *bitmap = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(bitmap);
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    if (self->userdata) {
        playdate->graphics->freeBitmapTable(self->userdata);
        self->userdata = NULL;
    }
    MELSpriteDealloc(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELPoint origin = self->frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
}

LCDSprite * _Nonnull ImageConstructor(MELPoint origin, LCDBitmap * _Nonnull image) {
    return ImageConstructorWithSelf(playdate->system->realloc(NULL, sizeof(MELSprite)), origin, image);
}
LCDSprite * _Nonnull ImageConstructorWithSpritePalette(MELPoint origin, SpriteName spriteName, int imageIndex) {
    LCDBitmapTable *table = SpriteNameLoadBitmapTable(spriteName);
    LCDBitmap *image = playdate->graphics->getTableBitmap(table, imageIndex);
    LCDSprite *sprite = ImageConstructorWithSelf(playdate->system->realloc(NULL, sizeof(MELSprite)), origin, image);
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->userdata = table;
    return sprite;
}
LCDSprite * _Nonnull ImageConstructorWithSpritePaletteAndAnimation(MELPoint origin, SpriteName spriteName, AnimationName animationName, MELAnimationDirection direction) {
    MELSpriteDefinition *definition = SpriteNameGetDefinition(spriteName);
    if (!definition->palette) {
        definition->palette = SpriteNameLoadBitmapTable(spriteName);
    }
    MELAnimationDefinition *animationDefinition = MELSpriteDefinitionGetAnimationDefinition(*definition, animationName, direction);
    LCDBitmap *image = playdate->graphics->getTableBitmap(definition->palette, animationDefinition->frames->atlasIndex);
    return ImageConstructorWithSelf(playdate->system->realloc(NULL, sizeof(MELSprite)), origin, image);
}

LCDSprite * _Nonnull ImageConstructorWithPath(MELPoint origin, const char * _Nonnull path) {
    return ImageConstructor(origin, LCDBitmapLoadOrError(path));
}

LCDSprite * _Nonnull ImageConstructorWithSelf(MELSprite * _Nonnull self, MELPoint origin, LCDBitmap * _Nonnull image) {
    LCDSprite *sprite = ImageConstructorWithSelfDontPush(self, origin, image);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push Image(%x, %x): %d", sprite, self, self->definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

LCDSprite * _Nonnull ImageConstructorWithSelfDontPush(MELSprite * _Nonnull self, MELPoint origin, LCDBitmap * _Nonnull image) {
    int width, height;
    playdate->graphics->getBitmapData(image, &width, &height, NULL, NULL, NULL);

    *self = (MELSprite) {
        .class = &ImageClass,
        .frame = {
            .size = {
                .width = width,
                .height = height
            },
            .origin = origin,
        },
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
    playdate->sprite->addSprite(sprite);

    return sprite;
}

void ImageSetOrigin(LCDSprite * _Nonnull sprite, MELPoint origin, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    origin = MELOriginForSizeAndAlignment(origin, self->frame.size, horizontalAlignment, verticalAlignment);
    self->frame.origin = origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
}

const MELSpriteClass * _Nonnull ImageGetClass(void) {
    return &ImageClass;
}
