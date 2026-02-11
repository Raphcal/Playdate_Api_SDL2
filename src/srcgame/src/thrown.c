//
//  thrown.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/01/2023.
//

#include "thrown.h"

#include "../lib/melice.h"
#include "caught.h"
#include "explosion.h"
#include "gamescene.h"
#include "bonus.h"
#include "chain_multiplier.h"
#include "samples.h"

static const float kSpeedBonusDivisor = 750.0f;

typedef struct thrown {
    MELSprite super;
    Player * _Nullable player;
    float angle;
    float speed;
    float time;
    uint16_t combo;
} Thrown;

static void update(LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);
static void checkCollisionsWithOtherSprites(Thrown * _Nonnull self, LCDSprite * _Nonnull sprite);

static const MELSpriteClass ThrownClass = (MELSpriteClass) {
    .name = SpriteClassNameThrown,
    .destroy = MELSpriteDealloc,
    .save = save,
    .load = load,
};

Thrown * _Nonnull throwSprite(LCDSprite * _Nonnull sprite, Player * _Nullable player, float angle, float speed) {
    MELSprite *oldSelf = playdate->sprite->getUserdata(sprite);
    releaseSprite(oldSelf);
    if (oldSelf->class->caughtOrThrown) {
        oldSelf->class->caughtOrThrown(oldSelf);
    }

    Thrown *self = playdate->system->realloc(oldSelf, sizeof(Thrown));
    self->super.class = &ThrownClass;
    self->super.definition.type = MELSpriteTypeDestructible;
    self->player = player;
    self->angle = angle;
    self->speed = speed;
    self->time = 0.0f;
    self->combo = 0;
    MELHitboxReaffect(self->super.hitbox, &self->super);
    MELSpriteSetAnimation(&self->super, AnimationNameBounce);
    self->super.animation->speed = 0.25f + 0.75f * speed / 250.0f;

    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setUpdateFunction(sprite, update);
    return self;
}

const MELSpriteClass * _Nonnull ThrownGetClass(void) {
    return &ThrownClass;
}

static void update(LCDSprite * _Nonnull sprite) {
    Thrown *self = playdate->sprite->getUserdata(sprite);
    float angle = self->angle;

    float time = self->time + DELTA;
    if (time >= THROWN_ENEMY_INTERVAL_BETWEEN_HITPOINT_LOSS) {
        time -= THROWN_ENEMY_INTERVAL_BETWEEN_HITPOINT_LOSS;
        // Enlève plus de points quand la vitesse est lente.
        self->super.hitPoints -= MELFloatMin(10.0f, 100.0f / self->speed);
    }
    self->time = time;

    MELPoint origin = self->super.frame.origin;
    origin.x += self->speed * DELTA * cosf(angle);
    origin.y += self->speed * DELTA * sinf(angle);

    if (origin.y < 0) {
        origin.y = -origin.y;
        angle = -angle;
        self->super.hitPoints -= THROWN_ENEMY_DAMAGE_ON_BOUNCE;
    } else if (origin.y >= LCD_ROWS) {
        origin.y = LCD_ROWS + LCD_ROWS - origin.y;
        angle = -angle;
        self->super.hitPoints -= THROWN_ENEMY_DAMAGE_ON_BOUNCE;
    }
    if (origin.x < 0) {
        origin.x = -origin.x;
        angle = MEL_PI - angle;
        self->super.hitPoints -= THROWN_ENEMY_DAMAGE_ON_BOUNCE;
    } else if (origin.x >= LCD_COLUMNS) {
        origin.x = LCD_COLUMNS + LCD_COLUMNS - origin.x;
        angle = MEL_PI - angle;
        self->super.hitPoints -= THROWN_ENEMY_DAMAGE_ON_BOUNCE;
    }

    self->angle = angle;
    self->super.frame.origin = origin;

    checkCollisionsWithOtherSprites(self, sprite);
    if (self->super.hitPoints <= 0) {
        GameSceneScoreAdd(self->super.score, self->combo);
        MELSpriteDealloc(sprite);
        ExplosionConstructor(origin);
        return;
    }

   MELSpriteDraw(&self->super, sprite);
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    Thrown *self = (Thrown *) sprite;
    MELOutputStreamWriteFloat(outputStream, self->angle);
    MELOutputStreamWriteFloat(outputStream, self->speed);
    MELOutputStreamWriteFloat(outputStream, self->time);
    MELOutputStreamWriteUInt16(outputStream, self->combo);
}

static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    const float angle = MELInputStreamReadFloat(inputStream);
    const float speed = MELInputStreamReadFloat(inputStream);
    const float time = MELInputStreamReadFloat(inputStream);
    const uint16_t combo = MELInputStreamReadUInt16(inputStream);

    playdate->sprite->setUpdateFunction(sprite, update);

    Thrown *self = playdate->system->realloc(NULL, sizeof(Thrown));
    *self = (Thrown) {
        .super = {
            .class = &ThrownClass,
        },
        .angle = angle,
        .speed = speed,
        .time = time,
        .combo = combo,
    };
    return &self->super;
}

static void checkCollisionsWithOtherSprites(Thrown * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELRectangle selfHitbox = MELHitboxGetFrame(self->super.hitbox);
    // Pas de suppression de sprites dans l'itération donc pas besoin de copier la liste.
    LCDSpriteRefList *otherSprites = &currentScene->sprites;
    for (unsigned int index = 0; index < otherSprites->count && self->super.animationName != AnimationNameHurt; index++) {
        LCDSprite *otherSprite = otherSprites->memory[index];
        MELSprite *other = playdate->sprite->getUserdata(otherSprite);
        const MELSpriteType otherType = other->definition.type;
        switch (otherType) {
        case MELSpriteTypeEnemy:
        case MELSpriteTypeDestructible: // Ennemis lancés
        case MELSpriteTypeCollidable: // Boss
            if (&self->super != other && MELHitboxCollidesWithRectangle(other->hitbox, selfHitbox)) {
                int hitPoints = self->super.hitPoints;
                const int otherHitPoints = other->hitPoints;
                self->super.hitPoints = hitPoints - otherHitPoints;
                self->super.hitTimer = ENEMY_HIT_DURATION;
                if (otherHitPoints == 0 && otherType == MELSpriteTypeCollidable) {
                    self->super.hitPoints = 0;
                    return;
                } else if (otherHitPoints == 1) {
                    other->hitPoints = 0;
                } else {
                    const float damage = hitPoints * (1.0f + self->speed / kSpeedBonusDivisor);
                    other->hitPoints = MELIntMax(otherHitPoints - damage, otherType != MELSpriteTypeCollidable);
                }
                other->hitTimer = ENEMY_HIT_DURATION;
#if LOG_CATCHES_AND_THROWS
                playdate->system->logToConsole("Will throw sprite #%d, type: %d (self type: %d)", index, otherType, self->super.definition.type);
#endif
                if (otherType != MELSpriteTypeCollidable && !isCaught(other) && other->definition.name != SpriteNameGunnerBot) {
                    const float collisionAngle = MELPointAngleToPoint(other->frame.origin, self->super.frame.origin);
                    Thrown *thrown = throwSprite(otherSprite, self->player, collisionAngle, self->speed * 0.75f);
                    const uint16_t combo = self->combo;
                    thrown->combo = combo < UINT16_MAX ? combo + 1 : UINT16_MAX;

                    const MELPoint thrownOrigin = thrown->super.frame.origin;
                    ChainMultiplierConstructor((MELPoint) {
                        .x = (thrownOrigin.x + selfHitbox.origin.x) / 2,
                        .y = (thrownOrigin.y + selfHitbox.origin.y) / 2,
                    }, thrown->combo);
                    GameSceneComboIncrement();

                    // Bonus
                    BonusCreate(&thrown->super, collisionAngle + MEL_PI, self->speed);
                }

                // Bruit d'explosion.
                SamplePlay(SampleCollision);
            }
            break;
        case MELSpriteTypeBullet:
            if (MELHitboxCollidesWithRectangle(other->hitbox, selfHitbox)) {
                GameSceneScoreAdd(other->hitPoints * BULLET_SCORE_MULTIPLIER, self->combo);
                self->super.hitPoints -= other->hitPoints;
                self->super.hitTimer = ENEMY_HIT_DURATION;
                // TODO: Faire une animation de destruction du tir.
                playdate->sprite->setUpdateFunction(otherSprite, other->class->destroy);

                // Bruit d'arrêt de tir.
                SamplePlay(SampleBulletCollision);
            }
            break;
        default:
            // Pas de traitement.
            break;
        }
    }
}
