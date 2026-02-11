//
//  rankingsectionheader.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/11/2023.
//

#include "rankingsectionheader.h"

#include "image.h"
#include "rankingboard.h"
#include "camera.h"
#include "../gen/spritelocalonline.h"
#include "../gen/spriteactivityindicator.h"

static void createLoader(MELPoint origin);
static void updateLoader(LCDSprite * _Nonnull sprite);

static const MELSpriteClass RankingSectionHeaderClass = (MELSpriteClass) {
    .destroy = MELSpriteDealloc,
};

LCDSprite * _Nonnull RankingSectionHeaderConstructor(MELPoint origin, RankingSectionHeaderLabel label) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));

    loadSpriteLocalOnlinePalette();
    LCDBitmap *image = playdate->graphics->getTableBitmap(spriteLocalOnline.palette, label);

    LCDSprite *sprite = ImageConstructorWithSelf(self, origin, image);
    self->class = &RankingSectionHeaderClass;

    if (label == RankingSectionHeaderLabelOnline) {
        createLoader(origin);
    }

    return sprite;
}

const MELSpriteClass * _Nonnull RankingSectionHeaderGetClass(void) {
    return &RankingSectionHeaderClass;
}

static void createLoader(MELPoint origin) {
    loadSpriteActivityIndicatorPalette();
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    LCDSprite *sprite = MELSpriteInitWithCenter(self, &spriteActivityIndicator, (MELPoint) {
        .x = 278,
        .y = origin.y
    });
    playdate->sprite->setUpdateFunction(sprite, updateLoader);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push RankingSectionHeaderConstructor(%x, %x): %d", sprite, self, self->definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
}

static void updateLoader(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const MELBoolean isLoading = OnlineScoresAreLoading();
    if ((int)isLoading != (int)self->animationName) {
        MELSpriteSetAnimation(self, (AnimationName)isLoading);
    }

    MELAnimation *animation = self->animation;
    MELAnimationUpdate(animation, DELTA);
    
    const MELPoint origin = self->frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
}
