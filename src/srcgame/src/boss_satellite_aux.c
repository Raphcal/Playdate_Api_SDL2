//
//  boss_satellite_aux.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/03/2023.
//

#include "boss_satellite_aux.h"

#include "boss_satellite.h"
#include "simpleshootingstyle.h"
#include "explosion.h"
#include "../gen/spritesatelliteaux.h"
#include "../gen/spritebullet.h"

typedef struct {
    MELSprite super;
    BossSatellite * _Nullable parent;
    float angle;
    float distance;
    ShootingStyle shootingStyle;
} BossSatelliteAux;

static const ShootingStyleDefinition kShootingStyleDefinition = (ShootingStyleDefinition) {
    .origin = ShotOriginCenter,
    .bulletDefinition = &spriteBullet,
    .bulletAnimationName = AnimationNameStand,
    .bulletSpeed = 100,
    .bulletAmount = 1,
    .damage = BOSS_SATELLITE_AUX_BULLET_DAMAGE,
    .shootInterval = 0.5f,
};

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static void update(LCDSprite * _Nonnull sprite);
static void caughtOrThrown(MELSprite * _Nonnull self);
static BossSatellite * _Nullable findParent(void);

static const MELSpriteClass BossSatelliteAuxClass = (MELSpriteClass) {
    .name = SpriteClassNameBossSatelliteAux,
    .destroy = MELSpriteDealloc,
    .caughtOrThrown = caughtOrThrown,
    .save = save,
    .load = BossSatelliteAuxLoader,
};

LCDSprite * _Nonnull BossSatelliteAuxConstructor(BossSatellite * _Nullable parent, float angle, float distance, int score) {
    BossSatelliteAux *self = playdate->system->realloc(NULL, sizeof(BossSatelliteAux));
    loadSpriteSatelliteAuxPalette();
    LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, &spriteSatelliteAux, parent->super.frame.origin);
    self->super.class = &BossSatelliteAuxClass;
    self->super.hitPoints = BOSS_SATELLITE_AUX_HIT_POINTS;
    self->super.score = score;
    self->parent = parent;
    self->angle = angle;
    self->distance = distance;
    loadSpriteBulletPalette();
    SimpleShootingStyleInit(&self->shootingStyle, &kShootingStyleDefinition);
    MELSingleFrameAnimationReuse(self->super.animation);
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setZIndex(sprite, ZINDEX_ENEMIES);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push BossSatelliteAux(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

MELSprite * _Nullable BossSatelliteAuxLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    playdate->sprite->setUpdateFunction(sprite, update);

    const float angle = MELInputStreamReadFloat(inputStream);
    const float distance = MELInputStreamReadFloat(inputStream);
    loadSpriteBulletPalette();
    const ShootingStyle shootingStyle = ShootingStyleLoad(inputStream, &kShootingStyleDefinition);

    BossSatelliteAux *self = playdate->system->realloc(NULL, sizeof(BossSatelliteAux));
    *self = (BossSatelliteAux) {
        .super = {
            .class = &BossSatelliteAuxClass,
        },
        .parent = findParent(),
        .angle = angle,
        .distance = distance,
        .shootingStyle = shootingStyle,
    };
    return &self->super;
}

const MELSpriteClass * _Nonnull BossSatelliteAuxGetClass(void) {
    return &BossSatelliteAuxClass;
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    BossSatelliteAux *self = (BossSatelliteAux *)sprite;
    MELOutputStreamWriteFloat(outputStream, self->angle);
    MELOutputStreamWriteFloat(outputStream, self->distance);
    ShootingStyleSave(&self->shootingStyle, outputStream);
}

static void caughtOrThrown(MELSprite * _Nonnull sprite) {
    BossSatelliteAux *self = (BossSatelliteAux *)sprite;
    self->parent->auxiliaries--;
}

static BossSatellite * _Nullable findParent(void) {
    LCDSpriteRefList sprites = currentScene->sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        MELSprite *melSprite = playdate->sprite->getUserdata(sprites.memory[index]);
        if (melSprite->definition.name == SpriteNameSatelliteBoss) {
            return (BossSatellite *)melSprite;
        }
    }
    playdate->system->logToConsole("Parent not found!");
    return NULL;
}

static void update(LCDSprite * _Nonnull sprite) {
    BossSatelliteAux *self = playdate->sprite->getUserdata(sprite);
    BossSatellite *parent = self->parent;

    if (parent->super.hitPoints <= 0) {
        // Auto-destruction quand le boss est KO.
        ExplosionConstructor(self->super.frame.origin);
        MELSpriteDealloc(sprite);
        return;
    }

    const float delta = DELTA;

    const float auxSpeed = parent->auxSpeed;
    const float auxDistanceMultiplier = parent->auxDistanceMultiplier;

    const MELPoint origin = parent->super.frame.origin;
    const float angle = self->angle + delta * auxSpeed;
    self->angle = angle;
    const float distance = self->distance;
    const MELPoint selfOrigin = (MELPoint) {
        .x = origin.x + cosf(angle) * distance * auxDistanceMultiplier,
        .y = origin.y + sinf(angle) * distance * auxDistanceMultiplier,
    };
    self->super.frame.origin = selfOrigin;
    MELSingleFrameAnimationSetAngleRadian(self->super.animation, angle);

    if (parent->state != reentering) {
        ShootingStyleShootFromSprite(&self->shootingStyle, &self->super, angle + MEL_PI);
    }

    self->super.animation->class->update(self->super.animation, delta);

    playdate->sprite->moveTo(sprite, selfOrigin.x, selfOrigin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, self->super.animation->frame.atlasIndex), kBitmapUnflipped);
}
