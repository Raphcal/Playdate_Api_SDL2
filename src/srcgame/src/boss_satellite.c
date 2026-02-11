//
//  boss_satellite.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 14/03/2023.
//

#include "boss_satellite.h"

#include "../lib/subsprite.h"
#include "../gen/spritenames.h"
#include "../gen/spritesatelliteboss.h"
#include "../gen/spritesatelliteaux.h"
#include "../gen/spritebullet.h"
#include "../gen/spriteplayerexplosion.h"
#include "../gen/spritebigexplosion.h"
#include "../gen/spriterocketsmoke.h"
#include "bullet.h"
#include "boss_satellite_aux.h"
#include "gunner_bot.h"
#include "gamescene.h"
#include "gui_lifebar.h"
#include "explosion.h"
#include "particuleshootingstyle.h"
#include "bonus.h"
#include "samples.h"
#include "camera.h"

static const float kInnerRay = 100.0f;
static const unsigned int kAuxiliaryCount = 8;

static const float kFarDistance = 1.7f;

static const MELPoint kStartingPoint = (MELPoint) { .x = 700.0f, .y = 300.0f };

static void initStatePart0(LCDSprite * _Nonnull sprite);

static void enteringStatePart0(LCDSprite * _Nonnull sprite);
static void enteringStatePart1(LCDSprite * _Nonnull sprite);

static void reenteringStatePart0(LCDSprite * _Nonnull sprite);

static void mainStatePart0(LCDSprite * _Nonnull sprite);
static void mainStatePart1(LCDSprite * _Nonnull sprite);

static void goFarStatePart0(LCDSprite * _Nonnull sprite);
static void goFarStatePart1(LCDSprite * _Nonnull sprite);

static void goNearStatePart0(LCDSprite * _Nonnull sprite);
static void goNearStatePart1(LCDSprite * _Nonnull sprite);

static void thrustStatePart0(LCDSprite * _Nonnull sprite);
static void thrustStatePart1(LCDSprite * _Nonnull sprite);
static void thrustStatePart2(LCDSprite * _Nonnull sprite);
static void thrustStatePart3(LCDSprite * _Nonnull sprite);
static void thrustStatePart4(LCDSprite * _Nonnull sprite);
static void thrustStatePart5(LCDSprite * _Nonnull sprite);
static void thrustStatePart6(LCDSprite * _Nonnull sprite);

static void reverseStatePart0(LCDSprite * _Nonnull sprite);
static void reverseStatePart1(LCDSprite * _Nonnull sprite);

static void movingStatePart0(LCDSprite * _Nonnull sprite);

static void destroyedStatePart0(LCDSprite * _Nonnull sprite);
static void destroyedStatePart1(LCDSprite * _Nonnull sprite);

static void explodingStatePart0(LCDSprite * _Nonnull sprite);

static void goToCurrentState(struct boss_satellite * _Nonnull self, LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);

static const MELSpriteClass BossSatelliteClass = (MELSpriteClass) {
    .destroy = MELSpriteDealloc,
    .save = save,
    .load = BossSatelliteLoader,
};

static const ShootingStyleDefinition kSmokeDefinition = (ShootingStyleDefinition) {
    .origin = ShotOriginCenter,
    .bulletDefinition = &spriteRocketSmoke,
    .bulletAmount = 1,
    .shootInterval = 0.1f,
    .space = 128.0f,
};

LCDSprite * _Nonnull BossSatelliteConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance)  {
    struct boss_satellite *self = playdate->system->realloc(NULL, sizeof(struct boss_satellite));
    LCDSprite *sprite = MELSpriteInit(&self->super, definition, instance);
    self->super.class = &BossSatelliteClass;
    self->super.hitPoints = BOSS_SATELLITE_HIT_POINTS;
    self->super.score = BOSS_SATELLITE_SCORE;
    self->super.frame.origin = kStartingPoint;
    loadSpriteSatelliteAuxPalette();
    loadSpriteBulletPalette();
    self->auxiliaries = kAuxiliaryCount;
    self->auxDistanceMultiplier = kFarDistance;
    self->auxSpeed = 0.3f;
    self->baseSpeed = 0.3f;
    self->speed = (MELPoint) { .x = 25.0f, .y = 25.0f };
    self->state = init;
    self->statePart = 0;
    self->explosion = NULL;
    playdate->sprite->setUpdateFunction(sprite, &initStatePart0);
    playdate->sprite->setZIndex(sprite, ZINDEX_ENEMIES);
    loadSpriteRocketSmokePalette();
    ParticuleShootingStyleInit(&self->smoke, &kSmokeDefinition);

    self->lifeBar = GUILifebarConstructor(BOSS_SATELLITE_HIT_POINTS, &self->super.hitPoints);

    return sprite;
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    struct boss_satellite *self = (struct boss_satellite *)sprite;
    GUILifebarSave(self->lifeBar, outputStream);
    MELOutputStreamWriteByte(outputStream, self->state);
    MELOutputStreamWriteByte(outputStream, self->statePart);
    MELOutputStreamWriteByte(outputStream, self->auxiliaries);
    MELOutputStreamWriteFloat(outputStream, self->auxDistanceMultiplier);
    MELOutputStreamWriteFloat(outputStream, self->auxSpeed);
    MELOutputStreamWriteFloat(outputStream, self->baseSpeed);
    MELOutputStreamWritePoint(outputStream, self->speed);
    MELOutputStreamWriteFloat(outputStream, self->strideDecimalFrom0);
    MELOutputStreamWriteFloat(outputStream, self->strideDecimalTo0);
    MELOutputStreamWritePoint(outputStream, self->stridePointFrom0);
    MELOutputStreamWritePoint(outputStream, self->stridePointTo0);
    MELOutputStreamWriteFloat(outputStream, self->time);
    MELOutputStreamWriteByte(outputStream, self->visibleAuxiliaries);
    ShootingStyleSave(&self->smoke, outputStream);

    MELOutputStreamWriteBoolean(outputStream, self->explosion != NULL);
    if (self->explosion) {
        MELSprite *explosion = playdate->sprite->getUserdata(self->explosion);
        MELOutputStreamWritePoint(outputStream, explosion->frame.origin);
        MELOutputStreamWriteUInt32(outputStream, explosion->animation->frameIndex);
    }
}

MELSprite * _Nullable BossSatelliteLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    struct boss_satellite *self = playdate->system->realloc(NULL, sizeof(struct boss_satellite));
    LCDSprite *lifeBar = GUILifebarLoad(inputStream, BOSS_SATELLITE_HIT_POINTS, &self->super.hitPoints);
    const enum boss_satellite_state state = MELInputStreamReadByte(inputStream);
    const uint8_t statePart = MELInputStreamReadByte(inputStream);
    const uint8_t auxiliaries = MELInputStreamReadByte(inputStream);
    const float auxDistanceMultiplier = MELInputStreamReadFloat(inputStream);
    const float auxSpeed = MELInputStreamReadFloat(inputStream);
    const float baseSpeed = MELInputStreamReadFloat(inputStream);
    const MELPoint speed = MELInputStreamReadPoint(inputStream);
    const float strideDecimalFrom0 = MELInputStreamReadFloat(inputStream);
    const float strideDecimalTo0 = MELInputStreamReadFloat(inputStream);
    const MELPoint stridePointFrom0 = MELInputStreamReadPoint(inputStream);
    const MELPoint stridePointTo0 = MELInputStreamReadPoint(inputStream);
    const float time = MELInputStreamReadFloat(inputStream);
    const uint8_t visibleAuxiliaries = MELInputStreamReadByte(inputStream);
    loadSpriteRocketSmokePalette();
    const ShootingStyle smoke = ShootingStyleLoad(inputStream, &kSmokeDefinition);
    const MELBoolean isExploding = MELInputStreamReadBoolean(inputStream);

    *self = (struct boss_satellite) {
        .super = {
            .class = &BossSatelliteClass
        },
        .lifeBar = lifeBar,
        .state = state,
        .statePart = statePart,
        .auxiliaries = auxiliaries,
        .auxDistanceMultiplier = auxDistanceMultiplier,
        .auxSpeed = auxSpeed,
        .baseSpeed = baseSpeed,
        .speed = speed,
        .strideDecimalFrom0 = strideDecimalFrom0,
        .strideDecimalTo0 = strideDecimalTo0,
        .stridePointFrom0 = stridePointFrom0,
        .stridePointTo0 = stridePointTo0,
        .time = time,
        .visibleAuxiliaries = visibleAuxiliaries,
        .smoke = smoke,
    };
    if (isExploding) {
        loadSpriteBigExplosionPalette();
        MELPoint origin = MELInputStreamReadPoint(inputStream);
        LCDSprite *explosionSprite = ExplosionConstructor(origin);
        self->explosion = explosionSprite;
        MELSprite *explosion = playdate->sprite->getUserdata(explosionSprite);

        const uint32_t frameIndex = MELInputStreamReadUInt32(inputStream);
        MELAnimationSetFrameIndex(explosion->animation, frameIndex);
    }
    self->lifeBar = GUILifebarConstructor(BOSS_SATELLITE_HIT_POINTS, &self->super.hitPoints);
    goToCurrentState(self, sprite);
    return &self->super;
}

static void goToCurrentState(struct boss_satellite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    switch (self->state) {
        case init:
            playdate->sprite->setUpdateFunction(sprite, initStatePart0);
            break;
        case reentering:
            playdate->sprite->setUpdateFunction(sprite, reenteringStatePart0);
            break;
        case main:
            switch (self->statePart) {
                case 1:
                    playdate->sprite->setUpdateFunction(sprite, mainStatePart1);
                    break;
                default:
                    playdate->sprite->setUpdateFunction(sprite, mainStatePart0);
                    break;
            }
            break;
        case goFar:
            switch (self->statePart) {
                case 1:
                    playdate->sprite->setUpdateFunction(sprite, goFarStatePart1);
                    break;
                default:
                    playdate->sprite->setUpdateFunction(sprite, goFarStatePart0);
                    break;
            }
            break;
        case goNear:
            switch (self->statePart) {
                case 1:
                    playdate->sprite->setUpdateFunction(sprite, goNearStatePart1);
                    break;
                default:
                    playdate->sprite->setUpdateFunction(sprite, goNearStatePart0);
                    break;
            }
            break;
        case thrust:
            switch (self->statePart) {
                case 1:
                    playdate->sprite->setUpdateFunction(sprite, thrustStatePart1);
                    break;
                case 2:
                    playdate->sprite->setUpdateFunction(sprite, thrustStatePart2);
                    break;
                case 3:
                    playdate->sprite->setUpdateFunction(sprite, thrustStatePart3);
                    break;
                case 4:
                    playdate->sprite->setUpdateFunction(sprite, thrustStatePart4);
                    break;
                case 5:
                    playdate->sprite->setUpdateFunction(sprite, thrustStatePart5);
                    break;
                case 6:
                    playdate->sprite->setUpdateFunction(sprite, thrustStatePart6);
                    break;
                default:
                    playdate->sprite->setUpdateFunction(sprite, thrustStatePart0);
                    break;
            }
            break;
        case reverse:
            switch (self->statePart) {
                case 1:
                    playdate->sprite->setUpdateFunction(sprite, reverseStatePart1);
                    break;
                default:
                    playdate->sprite->setUpdateFunction(sprite, reverseStatePart0);
                    break;
            }
            break;
        case moving:
            playdate->sprite->setUpdateFunction(sprite, movingStatePart0);
            break;
        case exploding:
            playdate->sprite->setUpdateFunction(sprite, explodingStatePart0);
            break;
        case destroyed:
            switch (self->statePart) {
                case 1:
                    playdate->sprite->setUpdateFunction(sprite, destroyedStatePart1);
                    break;
                default:
                    playdate->sprite->setUpdateFunction(sprite, destroyedStatePart0);
                    break;
            }
            break;
        case entering:
            switch (self->statePart) {
                case 1:
                    playdate->sprite->setUpdateFunction(sprite, enteringStatePart1);
                    break;
                default:
                    playdate->sprite->setUpdateFunction(sprite, enteringStatePart0);
                    break;
            }
            break;
    }
}

static void checkCollisionsWithOtherSprites(struct boss_satellite * _Nonnull self) {
    const MELRectangle hitbox = MELHitboxGetFrame(self->super.hitbox);
    uint8_t visibleAuxiliaries = 0;
    // Pas de suppression de sprites dans l'itération donc pas besoin de copier la liste.
    LCDSpriteRefList *otherSprites = &currentScene->sprites;
    for (unsigned int index = 0; index < otherSprites->count; index++) {
        LCDSprite *otherSprite = otherSprites->memory[index];
        MELSprite *other = playdate->sprite->getUserdata(otherSprite);
        switch (other->definition.type) {
        case MELSpriteTypeBullet:
            if (MELHitboxCollidesWithRectangle(other->hitbox, hitbox)) {
                playdate->sprite->setUpdateFunction(otherSprite, other->class->destroy);
            }
            break;
        case MELSpriteTypeEnemy:
                visibleAuxiliaries += MELRectangleOriginIsTopLeftContainsPoint((MELRectangle) {
                    .origin = MELPointZero,
                    .size = {
                        .width = LCD_COLUMNS,
                        .height = LCD_ROWS
                    }
                }, other->frame.origin);
            break;
        default:
            // Pas de traitement.
            break;
        }
    }
    self->visibleAuxiliaries = visibleAuxiliaries;
}

static void explode(struct boss_satellite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    // NOTE: Pas besoin de s'occuper aux auxiliaires, ils s'auto-détruisent.
    self->state = destroyed;
    self->statePart = 0;
    playdate->sprite->setUpdateFunction(sprite, destroyedStatePart0);

    playdate->sprite->setUpdateFunction(self->lifeBar, LCDSpriteDeinit);
    self->lifeBar = NULL;
}

static void speedUpAndCreateMoreAuxiliaries(BossSatellite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    self->state = reentering;
    self->statePart = 0;
    self->baseSpeed = MELFloatMin(self->baseSpeed * 2.0f, 1.0f);
    self->auxSpeed = self->baseSpeed;
    self->auxDistanceMultiplier = 4.0f;
    self->time = 0.0f;
    self->auxiliaries = kAuxiliaryCount;
    for (unsigned int index = 0; index < kAuxiliaryCount; index++) {
        BossSatelliteAuxConstructor(self, index * MEL_2_PI / kAuxiliaryCount, kInnerRay, 0);
    }
    playdate->sprite->setUpdateFunction(sprite, &reenteringStatePart0);
}

static void draw(struct boss_satellite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const enum boss_satellite_state state = self->state;
    const MELBoolean isReadyAndNotExploding = state >= reentering && state < exploding;
    if (isReadyAndNotExploding && self->super.hitPoints <= 0) {
        explode(self, sprite);
    } else if (isReadyAndNotExploding && self->auxiliaries == 0) {
        speedUpAndCreateMoreAuxiliaries(self, sprite);
    }
    checkCollisionsWithOtherSprites(self);

    const MELTimeInterval delta = DELTA;
    self->super.animation->class->update(self->super.animation, delta);

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, self->super.animation->frame.atlasIndex), kBitmapUnflipped);
}

static void initStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    self->state = entering;

    // Création des auxiliaires APRÈS la création du parent pour permettre le chargement.
    for (unsigned int index = 0; index < kAuxiliaryCount; index++) {
        BossSatelliteAuxConstructor(self, index * MEL_2_PI / kAuxiliaryCount, kInnerRay, BOSS_SATELLITE_AUX_SCORE);
    }

    enteringStatePart0(sprite);
}

static void enteringStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);

    // during
    self->stridePointTo0 = self->super.instance->center;
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &enteringStatePart1);

    draw(self, sprite);
}

static void enteringStatePart1(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 10.0f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->super.frame.origin = MELPointAdd(kStartingPoint, MELPointMultiplyByValue((MELPointSubstract(self->stridePointTo0, kStartingPoint)), progress));
        self->auxDistanceMultiplier = 4.0f + (kFarDistance - 4.0f) * progress;
        draw(self, sprite);
        return;
    }
    self->state = main;
    self->statePart = 0;
    goToCurrentState(self, sprite);
    draw(self, sprite);
}

static void reenteringStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 3.0f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->auxDistanceMultiplier = 4.0f + (kFarDistance - 4.0f) * progress;
        draw(self, sprite);
        return;
    }
    self->state = main;
    self->statePart = 0;
    goToCurrentState(self, sprite);
    draw(self, sprite);
}

static void mainStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);

    // during
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart1);

    draw(self, sprite);
}

static void mainStatePart1(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    if (self->super.hitPoints < BOSS_SATELLITE_HIT_POINTS / 4) {
        self->state = moving;
        playdate->sprite->setUpdateFunction(sprite, movingStatePart0);
        draw(self, sprite);
        return;
    }

    // during
    const float duration = 3.0f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        self->time = newTime;

        // wait
        draw(self, sprite);
        return;
    }

    const int action = self->visibleAuxiliaries > 0 && MELRandomInt(3);
    switch (action) {
    case 0:
        self->state = self->auxDistanceMultiplier > 1.0f ? goNear : goFar;
        break;
    case 1:
        self->state = thrust;
        break;
    default:
        self->state = reverse;
        break;
    }
    self->statePart = 0;
    goToCurrentState(self, sprite);
    draw(self, sprite);
}

static void goFarStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);

    // during
    self->strideDecimalFrom0 = self->auxDistanceMultiplier;
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &goFarStatePart1);

    draw(self, sprite);
}

static void goFarStatePart1(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 1.0f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->auxDistanceMultiplier = self->strideDecimalFrom0 + (kFarDistance - self->strideDecimalFrom0) * progress;
        draw(self, sprite);
        return;
    }
    self->state = main;
    self->statePart = 0;
    goToCurrentState(self, sprite);
    draw(self, sprite);
}

static void goNearStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);

    // during
    self->strideDecimalFrom0 = self->auxDistanceMultiplier;
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &goNearStatePart1);

    draw(self, sprite);
}

static void goNearStatePart1(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 1.0f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->auxDistanceMultiplier = self->strideDecimalFrom0 + (1.0f - self->strideDecimalFrom0) * progress;
        draw(self, sprite);
        return;
    }
    self->state = main;
    self->statePart = 0;
    goToCurrentState(self, sprite);
    draw(self, sprite);
}

static void thrustStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);

    // during
    self->strideDecimalFrom0 = self->auxSpeed;
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &thrustStatePart1);

    draw(self, sprite);
}

static void thrustStatePart1(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 0.2f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->auxSpeed = self->strideDecimalFrom0 + (0.0f - self->strideDecimalFrom0) * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    self->statePart = 2;
    playdate->sprite->setUpdateFunction(sprite, &thrustStatePart2);

    draw(self, sprite);
}

static void thrustStatePart2(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 0.8f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        self->time = newTime;

        // wait
        draw(self, sprite);
        return;
    }
    // during
    self->strideDecimalFrom0 = self->auxDistanceMultiplier;
    self->time = 0;
    self->statePart = 3;
    playdate->sprite->setUpdateFunction(sprite, &thrustStatePart3);

    draw(self, sprite);
}

static void thrustStatePart3(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 1.0f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->auxDistanceMultiplier = self->strideDecimalFrom0 + (2.5f - self->strideDecimalFrom0) * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->strideDecimalFrom0 = self->auxDistanceMultiplier;
    self->time = 0;
    self->statePart = 4;
    playdate->sprite->setUpdateFunction(sprite, &thrustStatePart4);

    draw(self, sprite);
}

static void thrustStatePart4(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 1.0f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->auxDistanceMultiplier = self->strideDecimalFrom0 + (kFarDistance - self->strideDecimalFrom0) * progress;
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    self->statePart = 5;
    playdate->sprite->setUpdateFunction(sprite, &thrustStatePart5);

    draw(self, sprite);
}

static void thrustStatePart5(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 0.8f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        self->time = newTime;

        // wait
        draw(self, sprite);
        return;
    }
    // during
    self->strideDecimalFrom0 = self->auxSpeed;
    self->time = 0;
    self->statePart = 6;
    playdate->sprite->setUpdateFunction(sprite, &thrustStatePart6);

    draw(self, sprite);
}

static void thrustStatePart6(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 0.2f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->auxSpeed = self->strideDecimalFrom0 + (self->baseSpeed - self->strideDecimalFrom0) * progress;
        draw(self, sprite);
        return;
    }
    self->state = main;
    self->statePart = 0;
    goToCurrentState(self, sprite);
    draw(self, sprite);
}

static void reverseStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);

    // during
    self->strideDecimalFrom0 = self->auxSpeed;
    self->strideDecimalTo0 = -self->auxSpeed;
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &reverseStatePart1);

    draw(self, sprite);
}

static void reverseStatePart1(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    // during
    const float duration = 1.0f;
    const float time = self->time;
    if (time < duration) {
        const float newTime = time + delta;
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        self->auxSpeed = self->strideDecimalFrom0 + (self->strideDecimalTo0 - self->strideDecimalFrom0) * progress;
        draw(self, sprite);
        return;
    }
    self->state = main;
    self->statePart = 0;
    goToCurrentState(self, sprite);
    draw(self, sprite);
}

static void movingStatePart0(LCDSprite * _Nonnull sprite) {
    struct boss_satellite *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;

    MELRectangle frame = self->super.frame;
    frame.origin = MELPointAdd(frame.origin, MELPointMultiplyByValue(self->speed, delta));
    if (frame.origin.x >= LCD_COLUMNS - frame.size.width / 2) {
        frame.origin.x = LCD_COLUMNS + LCD_COLUMNS - frame.size.width - frame.origin.x;
        self->speed.x = -self->speed.x;
    } else if (frame.origin.x <= frame.size.width / 2) {
        frame.origin.x = frame.size.width - frame.origin.x;
        self->speed.x = -self->speed.x;
    }
    if (frame.origin.y >= LCD_ROWS - frame.size.height / 2) {
        frame.origin.y = LCD_ROWS + LCD_ROWS - frame.size.height - frame.origin.y;
        self->speed.y = -self->speed.y;
    } else if (frame.origin.y <= frame.size.height / 2) {
        frame.origin.y = frame.size.height - frame.origin.y;
        self->speed.y = -self->speed.y;
    }
    self->super.frame = frame;
    self->statePart = 0;
    goToCurrentState(self, sprite);
    draw(self, sprite);
}

static void destroyedStatePart0(LCDSprite * _Nonnull sprite) {
    BossSatellite *self = playdate->sprite->getUserdata(sprite);
    self->super.definition.type = MELSpriteTypeDecor;
    self->super.hitTimer = 5.0f;

    GameSceneDestroyBullets();
    loadSpriteRocketSmokePalette();

    GameSceneScoreAdd(self->super.score, 0);
    BonusCreate(&self->super, -MEL_PI_2, 100.0f);

    GameScene *gameScene = GameSceneGetCurrent();
    // Arrête le temps pendant l'animation.
    gameScene->wave->type = WaveTypeOther;

    const MELPoint origin = self->super.frame.origin;

    // during
    self->time = 0;
    self->statePart = 1;
    self->stridePointFrom0 = origin;
    self->stridePointTo0 = gameScene->time >= GAME_TOTAL_TIME_IN_SECONDS
        ? (MELPoint) { .x = origin.x + 64, .y = origin.y + 80 }
        :  (MELPoint) { .x = origin.x - 200, .y = origin.y + 80 };
    self->strideDecimalFrom0 = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, &destroyedStatePart1);

    MELSpriteDraw(&self->super, sprite);
}

static void destroyedStatePart1(LCDSprite * _Nonnull sprite) {
    BossSatellite *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = 5.0f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        self->time = newTime;

        if (newTime - self->strideDecimalFrom0 >= 0.5f) {
            SamplePlay(SampleBulletCollision);
            self->strideDecimalFrom0 = newTime;
        }

        self->super.frame.origin.x = self->stridePointTo0.x + (self->stridePointFrom0.x - self->stridePointTo0.x) * ((1 - progress) * (1 - progress));
        self->super.frame.origin.y = self->stridePointFrom0.y + (self->stridePointTo0.y - self->stridePointFrom0.y) * progress;

        self->super.frame.origin.x += ((2 * progress) * cosf(25.13274f * self->time)) * 2;
        self->super.frame.origin.y += ((2 * progress) * sinf(25.13274f * self->time)) * 2;

        camera.frame.origin.x += MELEaseIn(0, duration, newTime) * CAMERA_SPEED_UP_DURING_BOSS_GUNNER * DELTA;

        // shoot
        ShootingStyleShootFromSprite(&self->smoke, &self->super, 0.0f);
        MELSpriteDraw(&self->super, sprite);
        return;
    }
    loadSpriteBigExplosionPalette();
    LCDSprite *explosion = ExplosionConstructorWithDefinition(self->super.frame.origin, &spriteBigExplosion);
    CameraShakeConstructor(2.0f, 30.0f);

    self->explosion = explosion;
    self->state = exploding;
    self->statePart = 0;
    playdate->sprite->setVisible(sprite, false);
    playdate->sprite->setUpdateFunction(sprite, explodingStatePart0);

    GameScene *gameScene = GameSceneGetCurrent();
    if (gameScene->time < GAME_TOTAL_TIME_IN_SECONDS) {
        GunnerBotConstructorWithOrigin(self->super.frame.origin);
    }

    SamplePlay(SampleExplosion);
}

static void explodingStatePart0(LCDSprite * _Nonnull sprite) {
    BossSatellite *self = playdate->sprite->getUserdata(sprite);
    MELSprite *explosion = playdate->sprite->getUserdata(self->explosion);
    if (MELAnimationIsLastFrame(explosion->animation)) {
        GameScene *gameScene = GameSceneGetCurrent();
        if (gameScene->time >= GAME_TOTAL_TIME_IN_SECONDS) {
            GameSceneFadeToClearScreen();
        }
        MELSpriteDealloc(sprite);
    }
}
