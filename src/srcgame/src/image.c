//
//  image.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/07/2023.
//

#include "image.h"

#include "camera.h"

static const MELSpriteClass ImageClass = (MELSpriteClass) {
    .destroy = ImageDealloc,
};

void ImageDealloc(LCDSprite * _Nonnull sprite) {
    LCDBitmap *bitmap = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(bitmap);
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

const MELSpriteClass * _Nonnull ImageGetClass(void) {
    return &ImageClass;
}
