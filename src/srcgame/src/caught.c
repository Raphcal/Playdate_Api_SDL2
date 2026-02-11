//
//  caught.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 30/01/2023.
//

#include "caught.h"

#include "camera.h"
#include "thrown.h"
#include "explosion.h"
#include "samples.h"
#include "gamescene.h"
#include "shootingstyle.h"
#include "particuleshootingstyle.h"
#include "../gen/spriterocketsmoke.h"

static const float kMinimumSpeedToThrow = 10.0f;
static const float kMinimumMovement = 5.0f;
static const float kSpeedMultiplier = 18.0f;
static const float kMaximumThrowSpeed = 50.0f;

typedef struct {
    MELSprite super;
    MELPoint * _Nullable parentOrigin;
    float distance;
    Player * _Nonnull player;
    MELBoolean isCranking;
    MELBoolean shouldThrow;
    float maxCrankChange;
    /**
     * Chaque ennemi lancé possède sa propre définition car le paramètre `space`
     * change en fonction de la taille du sprite.
     */
    ShootingStyleDefinition smokeDefinition;
    ShootingStyle smoke;
} Caught;

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull output);
static MELSprite * _Nonnull load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);
static void update(LCDSprite * _Nonnull sprite);
static void checkCollisionsWithOtherSprites(Caught * _Nonnull self, LCDSprite * _Nonnull sprite);

static const MELSpriteClass CaughtClass = (MELSpriteClass) {
    .name = SpriteClassNameCaught,
    .destroy = MELSpriteDealloc,
    .save = save,
    .load = load,
};

void catchSprite(LCDSprite * _Nonnull sprite, Player * _Nonnull player) {
    MELSprite *oldSelf = playdate->sprite->getUserdata(sprite);
    if (oldSelf->class->caughtOrThrown) {
        oldSelf->class->caughtOrThrown(oldSelf);
    }
    MELSprite *parent = &player->hands->super;

    Caught *self = playdate->system->realloc(oldSelf, sizeof(Caught));
    self->super.class = &CaughtClass;
    self->super.definition.type = MELSpriteTypeDestructible;
    self->parentOrigin = &parent->frame.origin;
    self->distance = MELPointDistanceToPoint(parent->frame.origin, self->super.frame.origin);
    self->player = player;
    self->isCranking = false;
    self->shouldThrow = false;
    self->smokeDefinition = (ShootingStyleDefinition) {
        .origin = ShotOriginCenter,
        .space = self->super.frame.size.height,
        .shootInterval = 1.0f,
        .bulletDefinition = &spriteRocketSmoke,
    };
    loadSpriteRocketSmokePalette();
    ParticuleShootingStyleInit(&self->smoke, &self->smokeDefinition);
    MELHitboxReaffect(self->super.hitbox, &self->super);

    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_CAUGHT);
    playdate->sprite->setUpdateFunction(sprite, update);
}

MELBoolean isCaught(MELSprite * _Nonnull self) {
    return self->class == &CaughtClass;
}

const MELSpriteClass * _Nonnull CaughtGetClass(void) {
    return &CaughtClass;
}

void releaseSprite(MELSprite * _Nonnull self) {
    if (isCaught(self)) {
        Caught *caught = (Caught *)self;
        caught->player->isCatching = false;
    }
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull output) {
    Caught *self = (Caught *)sprite;
    MELOutputStreamWriteFloat(output, self->distance);
}

static MELSprite * _Nonnull load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    playdate->sprite->setUpdateFunction(sprite, update);

    const float distance = MELInputStreamReadFloat(inputStream);
    GameScene *gameScene = GameSceneGetCurrent();
    Player *player = gameScene->player;
    MELSprite *parent = &player->hands->super;

    loadSpriteRocketSmokePalette();

    Caught *self = playdate->system->realloc(NULL, sizeof(Caught));
    *self = (Caught) {
        .super = {
            .class = &CaughtClass,
        },
        .parentOrigin = &parent->frame.origin,
        .distance = distance,
        .player = player,
        .isCranking = false,
        .shouldThrow = false,
        .smokeDefinition = (ShootingStyleDefinition) {
            .origin = ShotOriginCenter,
            .space = self->super.frame.size.height,
            .shootInterval = 1.0f,
            .bulletDefinition = &spriteRocketSmoke,
        },
    };
    ParticuleShootingStyleInit(&self->smoke, &self->smokeDefinition);
    return &self->super;
}

static void update(LCDSprite * _Nonnull sprite) {
    Caught *self = playdate->sprite->getUserdata(sprite);
    MELSprite *melSpriteToDraw = &self->super;
    const MELPoint parentOrigin = *self->parentOrigin;
    Player *player = self->player;
    if (!player->isCatching) {
        throwSprite(sprite, self->player, MELPointAngleToPoint(parentOrigin, self->super.frame.origin), CAUGHT_RELEASE_SPEED_WHEN_PLAYER_IS_DESTROYED);
        return;
    }

    const float crankAngle = playdate->system->getCrankAngle() - FORWARD_ANGLE + 360.0f;
    const float radian = MELDegreesToRadians(crankAngle);
    const MELPoint origin = (MELPoint) {
        .x = parentOrigin.x + self->distance * cosf(radian),
        .y = parentOrigin.y + self->distance * sinf(radian),
    };
    self->super.frame.origin = origin;

    checkCollisionsWithOtherSprites(self, sprite);

    const int hitPoints = self->super.hitPoints;
    if (hitPoints <= 0) {
        player->isCatching = false;
        GameSceneScoreAdd(self->super.score, 0);
        ExplosionConstructor(self->super.frame.origin);
        MELSpriteDealloc(sprite);
        return;
    } else if (hitPoints <= 30) {
        self->smokeDefinition.shootInterval = hitPoints / 60.0f;
        ShootingStyleShootFromSprite(&self->smoke, &self->super, 0.0f);

        // TODO: Faire flasher le sprite
    }

    // TODO: Calculer une vélocité glissante (?) sur 0.1 secondes et relâcher en cas de grand écart absolu.
    const MELBoolean isCranking = self->isCranking;
    const float crankChange = fabsf(MELCrankGetAcceleratedChange());
    if (!isCranking && crankChange != 0.0f) {
#if LOG_CATCHES_AND_THROWS
        playdate->system->logToConsole("Started cranking, crankChange: %f", (double) crankChange);
#endif
        self->isCranking = true;
        self->maxCrankChange = 0.0f;
    } else if (isCranking && (self->maxCrankChange < ADAPT_CRANK(kMinimumSpeedToThrow) || crankChange > ADAPT_CRANK(kMinimumMovement))) {
#if LOG_CATCHES_AND_THROWS
        playdate->system->logToConsole("Cranking! max: %f, crankChange: %f", (double) self->maxCrankChange, (double) crankChange);
#endif
        if (crankChange > self->maxCrankChange) {
            self->maxCrankChange = crankChange;
        }
    } else if (isCranking) {
        self->isCranking = false;
        const float speed = MELFloatMin(self->maxCrankChange, kMaximumThrowSpeed);
#if LOG_CATCHES_AND_THROWS
        playdate->system->logToConsole("Crank speed slowed down, throwing! max: %f, crankChange: %f", (double) self->maxCrankChange, (double) crankChange);
        playdate->system->logToConsole("speed: %f, total: %f", (double) speed, (double) (speed * ADAPT_SPEED(kSpeedMultiplier)));
#endif
        player->isCatching = false;
        Thrown *thrown = throwSprite(sprite, self->player, radian, speed * ADAPT_SPEED(kSpeedMultiplier));
        melSpriteToDraw = (MELSprite *)thrown;

        // Bruit de jet.
        SamplePlay(SampleThrow);
    }

    MELSpriteDraw(melSpriteToDraw, sprite);
}

static void checkCollisionsWithOtherSprites(Caught * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELRectangle selfHitbox = MELHitboxGetFrame(self->super.hitbox);
    // Pas de suppression de sprites dans l'itération donc pas besoin de copier la liste.
    LCDSpriteRefList *otherSprites = &currentScene->sprites;
    for (unsigned int index = 0; index < otherSprites->count && self->super.animationName != AnimationNameHurt; index++) {
        LCDSprite *otherSprite = otherSprites->memory[index];
        MELSprite *other = playdate->sprite->getUserdata(otherSprite);
        const MELSpriteType otherType = other->definition.type;
        switch (otherType) {
            case MELSpriteTypeEnemy:
            case MELSpriteTypeDestructible: // Ennemis lancés ou boss détruits
            case MELSpriteTypeCollidable: // Boss
                if (&self->super != other && MELHitboxCollidesWithRectangle(other->hitbox, selfHitbox)) {
                    int hitPoints = self->super.hitPoints;
                    const int otherHitPoints = other->hitPoints;
                    self->super.hitPoints = hitPoints - otherHitPoints;
                    self->super.hitTimer = ENEMY_HIT_DURATION;
                    if (otherHitPoints == 1) {
                        other->hitPoints = 0;
                    } else {
                        const float damage = hitPoints;
                        other->hitPoints = MELIntMax(otherHitPoints - damage, otherType != MELSpriteTypeCollidable);
                    }
                    other->hitTimer = ENEMY_HIT_DURATION;
#if LOG_CATCHES_AND_THROWS
                    playdate->system->logToConsole("Will throw sprite #%d, type: %d (self type: %d)", index, otherType, self->super.definition.type);
#endif
                    if (otherType != MELSpriteTypeCollidable && other->definition.name != SpriteNameGunnerBot) {
                        // La vitesse du joueur est constante donc pas la peine de faire varier la vitesse de lancé.
                        const float speed = 50.0f;
                        throwSprite(otherSprite, self->player, MELPointAngleToPoint(other->frame.origin, self->super.frame.origin), speed);
                        GameSceneComboIncrement();
                    }
                    // Bruit de collision.
                    SamplePlay(SampleCollision);
                }
                break;
            case MELSpriteTypeBullet:
                if (MELHitboxCollidesWithRectangle(other->hitbox, selfHitbox)) {
                    GameSceneScoreAdd(other->hitPoints * BULLET_SCORE_MULTIPLIER, 0);
                    self->super.hitPoints -= other->hitPoints;
                    self->super.hitTimer = ENEMY_HIT_DURATION;
                    // TODO: Faire une animation quand les pv sont bas ?
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
