//
//  rocket.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/04/2023.
//

#include "rocket.h"

#include "shootingstyle.h"
#include "particuleshootingstyle.h"
#include "gamescene.h"
#include "screen.h"

#include "../gen/spriterocketsmoke.h"

typedef struct {
    MELSprite super;
    float angle;
    ShootingStyle smoke;
    MELPoint * _Nonnull playerOrigin;
} Rocket;

static const float kSpeed = 100.0f;

static const ShootingStyleDefinition kSmokeDefinition = (ShootingStyleDefinition) {
    .origin = ShotOriginBack,
    .bulletDefinition = &spriteRocketSmoke,
    .bulletAmount = 1,
    .shootInterval = 0.1f,
    .space = 12.0f,
};

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);
static void update(LCDSprite * _Nonnull sprite);

static const MELSpriteClass RocketClass = (MELSpriteClass) {
    .name = SpriteClassNameRocket,
    .destroy = MELSpriteDealloc,
    .save = save,
    .load = load,
};

LCDSprite * _Nullable RocketConstructor(const ShootingStyleDefinition * _Nonnull definition, MELPoint origin, float angle) {
    if (currentScene->type != SceneTypeGame) {
        return NULL;
    }

    Rocket *self = playdate->system->realloc(NULL, sizeof(Rocket));
    MELSpriteDefinition *bulletDefinition = definition->bulletDefinition;

    GameScene *gameScene = GameSceneGetCurrent();
    Player *player = gameScene->player;

    *self = (Rocket) {
        .super = {
            .class = &RocketClass,
            .definition = *bulletDefinition,
            .frame = {
                .origin = origin,
                .size = bulletDefinition->size
            },
            .hitPoints = definition->damage,
        },
        .angle = angle,
        .playerOrigin = &player->super.frame.origin,
    };
    self->super.hitbox = MELSpriteHitboxAlloc(&self->super);
    MELSpriteSetAnimation(&self->super, definition->bulletAnimationName);
    loadSpriteRocketSmokePalette();
    ParticuleShootingStyleInit(&self->smoke, &kSmokeDefinition);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->addSprite(sprite);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_BULLETS);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push Rocket(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);

    return sprite;
}

const MELSpriteClass * _Nonnull RocketGetClass(void) {
    return &RocketClass;
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    Rocket *self = (Rocket *)sprite;
    MELOutputStreamWriteFloat(outputStream, self->angle);
    ShootingStyleSave(&self->smoke, outputStream);
}

static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    playdate->sprite->setUpdateFunction(sprite, update);

    loadSpriteRocketSmokePalette();
    const float angle = MELInputStreamReadFloat(inputStream);
    const ShootingStyle smoke = ShootingStyleLoad(inputStream, &kSmokeDefinition);

    GameScene *gameScene = GameSceneGetCurrent();
    Player *player = gameScene->player;

    Rocket *self = playdate->system->realloc(NULL, sizeof(Rocket));
    *self = (Rocket) {
        .super = {
            .class = &RocketClass,
        },
        .angle = angle,
        .smoke = smoke,
        .playerOrigin = &player->super.frame.origin,
    };
    return &self->super;
}

static void update(LCDSprite * _Nonnull sprite) {
    Rocket *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    MELRectangle frame = self->super.frame;
    MELPoint playerOrigin = *self->playerOrigin;
    const float targetAngle = MELPointAngleToPoint(frame.origin, playerOrigin) + MEL_PI;
    float angle = self->angle;
    if (fabsf(targetAngle - angle) <= delta) {
        angle = targetAngle;
    } else {
        angle = angle < targetAngle ? angle + delta : angle - delta;
    }
    self->angle = angle;
    ShootingStyleShootFromSprite(&self->smoke, &self->super, angle);

    frame.origin = (MELPoint) {
        .x = frame.origin.x + kSpeed * cosf(angle) * delta,
        .y = frame.origin.y + kSpeed * sinf(angle) * delta,
    };
    self->super.frame = frame;

    if (!MELRectangleIntersectsWithRectangle(frame, kScreen)) {
        MELSpriteDealloc(sprite);
        return;
    }

    MELAnimation *animation = self->super.animation;
    MELAnimationUpdate(animation, delta);
    // TODO: Faire tourner la roquette en fonction de son angle ?

    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
}
