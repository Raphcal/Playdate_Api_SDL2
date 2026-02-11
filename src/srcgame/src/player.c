//
//  player.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 24/01/2023.
//

#include "player.h"

#include "caught.h"
#include "thrown.h"
#include "punch.h"
#include "explosion.h"
#include "gamescene.h"
#include "camera.h"
#include "samples.h"
#include "gunner_bot.h"
#include "../gen/spritehero.h"
#include "../gen/spriteplayerexplosion.h"

static const float kMaximumSpeed = 200.0f;
static const float kSakiMaximumSpeed = 250.0f;
static const float kAcceleration = 600.0f;

static void destroy(LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull output);
static void updateClear(LCDSprite * _Nonnull sprite);
static void updateKatsuo(LCDSprite * _Nonnull sprite);
static void updateHands(MELSubSprite * _Nonnull self, MELBoolean isCatching, MELPoint playerOrigin);
static void updateDestroyed(LCDSprite * _Nonnull sprite);
static void checkCollisionsWithOtherSprites(Player * _Nonnull self, LCDSprite * _Nonnull player);

static void updateSaki(LCDSprite * _Nonnull sprite);
static void checkSakiCollisionsWithOtherSprites(Player * _Nonnull self, LCDSprite * _Nonnull sprite);

static const MELSpriteClass PlayerClass = (MELSpriteClass) {
    .name = SpriteClassNamePlayer,
    .destroy = destroy,
    .save = save,
    .load = PlayerLoader,
};

LCDSprite * _Nonnull PlayerConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    Player *self = playdate->system->realloc(NULL, sizeof(Player));

    *self = (Player) {
        .super = {
            .class = &PlayerClass,
            .definition = *definition,
            .frame = {
                .origin = instance->center,
                .size = definition->size,
            },
            .direction = MELDirectionRight,
            .instance = instance,
        },
        .state = PlayerStateNormal,
    };
    self->super.hitbox = MELSpriteHitboxAlloc(&self->super);
    MELSpriteSetAnimation(&self->super, AnimationNameStand);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->addSprite(sprite);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_PLAYER);

    return sprite;
}

MELSprite * _Nullable PlayerLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    Player *self = playdate->system->realloc(NULL, sizeof(Player));
    *self = (Player) {
        .super = {
            .class = &PlayerClass,
            .definition = *definition,
        }
    };
    playdate->sprite->setUserdata(sprite, self);

    self->state = (PlayerState) MELInputStreamReadByte(inputStream);
    self->isDead = MELInputStreamReadBoolean(inputStream);
    self->speed = MELInputStreamReadFloat(inputStream);
    self->maximumSpeed = MELInputStreamReadFloat(inputStream);
    self->acceleration = MELInputStreamReadFloat(inputStream);
    const MELBoolean isExploding = MELInputStreamReadBoolean(inputStream);
    if (isExploding) {
        loadSpritePlayerExplosionPalette();
        MELPoint origin = MELInputStreamReadPoint(inputStream);
        LCDSprite *explosionSprite = ExplosionConstructor(origin);
        self->explosion = explosionSprite;
        MELSprite *explosion = playdate->sprite->getUserdata(explosionSprite);

        const uint32_t frameIndex = MELInputStreamReadUInt32(inputStream);
        MELAnimationSetFrameIndex(explosion->animation, frameIndex);
    }
    // NOTE: les mains sont restaurées par PlayerSetCharacter.
    self->isCatching = MELInputStreamReadBoolean(inputStream);
    const PlayerCharacter character = (PlayerCharacter) MELInputStreamReadByte(inputStream);
    self->character = character;

    GameScene *gameScene = GameSceneGetCurrent();
    gameScene->player = self;
    gameScene->playerSprite = sprite;

    PlayerSetCharacter(self, sprite, character);
    switch (self->state) {
        case PlayerStateNoop:
            playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
            break;
        case PlayerStateClear:
            playdate->sprite->setUpdateFunction(sprite, gameScene->lives > 0 ? updateClear : MELSpriteNoopUpdate);
            break;
        case PlayerStateDestroyed:
            playdate->sprite->setUpdateFunction(sprite, updateDestroyed);
            break;
        default:
            // Méthode update définie par PlayerSetCharacter.
            break;
    }

    return &self->super;
}

const MELSpriteClass * _Nonnull PlayerGetClass(void) {
    return &PlayerClass;
}

void PlayerReset(Player * _Nonnull self, LCDSprite * _Nonnull sprite) {
    self->super.frame.origin = self->super.instance->center;
    self->isDead = false;
    self->super.hitTimer = PLAYER_INVICIBILITY_DURATION;
    self->explosion = NULL;
    self->state = PlayerStateNormal;
    playdate->sprite->setUpdateFunction(sprite, self->character == PlayerCharacterKatsuo ? updateKatsuo : updateSaki);
    playdate->sprite->setVisible(sprite, true);
    playdate->sprite->setVisible(self->hands->sprite, true);
    if (self->leftHand) {
        playdate->sprite->setVisible(self->leftHand->sprite, true);
    }
}

void PlayerSetCharacter(Player * _Nonnull self, LCDSprite * _Nonnull sprite, PlayerCharacter character) {
    self->character = character;
    MELSpriteSetAnimation(&self->super, (AnimationName) character);

    if (character == PlayerCharacterKatsuo) {
        playdate->sprite->setUpdateFunction(sprite, updateKatsuo);

        MELSubSprite *hands = MELSubSpriteAlloc(sprite, &spriteHero, AnimationNameRaise);
        MELSingleFrameAnimationReuse(hands->super.animation);
        self->hands = hands;
        playdate->sprite->setZIndex(hands->sprite, ZINDEX_HANDS);
    } else {
        playdate->sprite->setUpdateFunction(sprite, updateSaki);

        Punch *leftHand = PunchAlloc(sprite, false);
        Punch *rightHand = PunchAlloc(sprite, true);
        leftHand->otherPunchState = &rightHand->state;
        rightHand->otherPunchState = &leftHand->state;
        self->leftHand = &leftHand->super;
        self->hands = &rightHand->super;
    } 
}

void PlayerSetClearState(Player * _Nonnull self, LCDSprite * _Nonnull sprite, MELBoolean isAlive) {
    self->speed = 0.0f;
    self->state = PlayerStateClear;
    playdate->sprite->setUpdateFunction(sprite, isAlive ? updateClear : MELSpriteNoopUpdate);
}

#pragma mark - Méthodes privées

static void destroy(LCDSprite * _Nonnull sprite) {
    Player *self = playdate->sprite->getUserdata(sprite);
    playdate->sprite->setUpdateFunction(self->hands->sprite, MELSubSpriteDealloc);
    if (self->leftHand) {
        playdate->sprite->setUpdateFunction(self->leftHand->sprite, MELSubSpriteDealloc);
    }
    // NOTE: Pas de désallocation de l'explosion car sera désallouée par GameScene via la liste "others".
    MELSpriteDealloc(sprite);
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull output) {
    Player *self = (Player *)sprite;
    MELOutputStreamWriteByte(output, self->state);
    MELOutputStreamWriteBoolean(output, self->isDead);
    MELOutputStreamWriteFloat(output, self->speed);
    MELOutputStreamWriteFloat(output, self->maximumSpeed);
    MELOutputStreamWriteFloat(output, self->acceleration);
    MELOutputStreamWriteBoolean(output, self->explosion != NULL);
    if (self->explosion) {
        MELSprite *explosion = playdate->sprite->getUserdata(self->explosion);
        MELOutputStreamWritePoint(output, explosion->frame.origin);
        MELOutputStreamWriteUInt32(output, explosion->animation->frameIndex);
    }
    MELOutputStreamWriteBoolean(output, self->isCatching);
    MELOutputStreamWriteByte(output, self->character);
    // TODO: Sauvegarder l'état ?
}

static void draw(Player * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const float delta = DELTA;
    MELAnimation *animation = self->super.animation;
    MELAnimationUpdate(animation, delta);

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);

    if (self->explosion == NULL) {
        playdate->sprite->setVisible(sprite, (int)(self->super.hitTimer * 5.0f) % 2 == 0);
    }
}

static void explode(Player * _Nonnull self, LCDSprite * _Nonnull sprite) {
    loadSpritePlayerExplosionPalette();
    LCDSprite *explosion = ExplosionConstructorWithDefinition(self->super.frame.origin, &spritePlayerExplosion);
    self->explosion = explosion;
    self->state = PlayerStateDestroyed;
    playdate->sprite->setUpdateFunction(sprite, updateDestroyed);
    playdate->sprite->setVisible(sprite, false);
    playdate->sprite->setVisible(self->hands->sprite, false);
    if (self->leftHand) {
        playdate->sprite->setVisible(self->leftHand->sprite, false);
    }
    CameraShakeConstructor(0.5f, 24.0f);
}

static void updateDestroyed(LCDSprite * _Nonnull sprite) {
    Player *self = playdate->sprite->getUserdata(sprite);
    MELSprite *explosion = playdate->sprite->getUserdata(self->explosion);
    if (MELAnimationIsLastFrame(explosion->animation)) {
        self->state = PlayerStateNoop;
        self->explosion = NULL;
        playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
        GameSceneRestartAfterDeath();
    }
}

static void updateClear(LCDSprite * _Nonnull sprite) {
    // TODO: Faire la même chose mais vers la gauche pour le boss Gunner quand mort pendant le boss.
    Player *self = playdate->sprite->getUserdata(sprite);

    const float maximumSpeed = 700.0f;
    const float speed = MELFloatMin(self->speed + DELTA * kAcceleration, maximumSpeed);
    self->speed = speed;

    MELRectangle frame = self->super.frame;
    frame.origin.x += speed * DELTA;
    self->super.frame = frame;

    if (frame.origin.x > LCD_COLUMNS + frame.size.width) {
        // NOTE: Ne passe pas à l'état "noop" pour pouvoir charger correctement GameScene.
        playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    }

    // Gestion des mains de Katsuo. Les mains de Saki suivent automatiquement le centre.
    if (self->character == PlayerCharacterKatsuo) {
        updateHands(self->hands, self->isCatching, frame.origin);
    }
    draw(self, sprite);
}

#pragma mark - Katsuo

static void updateKatsuo(LCDSprite * _Nonnull sprite) {
    Player *self = playdate->sprite->getUserdata(sprite);

    const float delta = DELTA;
    const MELController controller = MELControllerMake();

    MELPoint origin = self->super.frame.origin;

    // Déplacement
    if (!MELPointEquals(controller.axe, MELPointZero)) {
        const MELSize halfSize = MELSizeDivideByValue(self->super.frame.size, 2.0f);

        origin.x = MELFloatBound(halfSize.width, origin.x + kMaximumSpeed * delta * controller.axe.x, LCD_COLUMNS - halfSize.width);
        origin.y = MELFloatBound(halfSize.height, origin.y + kMaximumSpeed * delta * controller.axe.y, LCD_ROWS - halfSize.height);
        self->super.frame.origin = origin;
    }
    camera.frame.origin.y = 32.0f - (origin.y / LCD_ROWS) * 32.0f;

    // Gestion des mains
    updateHands(self->hands, self->isCatching, origin);

    checkCollisionsWithOtherSprites(self, sprite);
    self->super.hitTimer = MELFloatMax(self->super.hitTimer - delta, 0.0f);

    draw(self, sprite);
}

static void updateHands(MELSubSprite * _Nonnull self, const MELBoolean isCatching, const MELPoint playerOrigin) {
    // 0° est en haut, donc retrait de 90° pour que l'angle 0 soit devant.
    const float crankAngle = playdate->system->getCrankAngle() - FORWARD_ANGLE + 360.0f;
    MELSingleFrameAnimationSetAngleDegree(self->super.animation, crankAngle);
    if (isCatching) {
        self->super.animation->definition = spriteHero.animations[AnimationNameDuck * MELAnimationDirectionCount + MELAnimationDirectionRight];
    } else {
        self->super.animation->definition = spriteHero.animations[AnimationNameRaise * MELAnimationDirectionCount + MELAnimationDirectionRight];
    }

    const float radian = MELDegreesToRadians(crankAngle);
    self->super.frame.origin = (MELPoint) {
        .x = playerOrigin.x + 24.0f * cosf(radian),
        .y = playerOrigin.y + 16.0f * sinf(radian),
    };
}

static void checkCollisionsWithOtherSprites(Player * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELRectangle playerHitbox = MELHitboxGetFrame(self->super.hitbox);
    const MELRectangle handsHitbox = MELHitboxGetFrame(self->hands->super.hitbox);
    // Pas de suppression de sprites dans l'itération donc pas besoin de copier la liste.
    // TODO: Faire une table des sprites proches et simplement vérifier les collisions avec eux (ne prendre en compte que les sprites autour du joueur avec un rectangle donné ?). Construire cette liste pendant l'update précédent pour l'update suivant.
    LCDSpriteRefList *otherSprites = &currentScene->sprites;
    for (unsigned int index = 0; index < otherSprites->count && self->super.animationName != AnimationNameHurt; index++) {
        LCDSprite *otherSprite = otherSprites->memory[index];
        MELSprite *other = playdate->sprite->getUserdata(otherSprite);
        const MELSpriteType otherType = other->definition.type;
        switch (otherType) {
        case MELSpriteTypeEnemy:
            if (!self->isCatching && MELHitboxCollidesWithRectangle(other->hitbox, handsHitbox)) {
                self->isCatching = true;
                catchSprite(otherSprite, self);

                GameSceneComboFillTimer();

                // Bruit d'attrapage.
                SamplePlay(SampleCatch);
                break;
            }
            // fallthrough
        case MELSpriteTypeCollidable:
            if (other->definition.name == SpriteNameGunnerBot
                && other->hitTimer == 0.0f
                && !self->isCatching
                && MELHitboxCollidesWithRectangle(other->hitbox, handsHitbox)) {
                self->isCatching = true;
                GunnerBotCatch(otherSprite, self);

                // Bruit d'attrapage.
                SamplePlay(SampleCatch);
            }
            // fallthrough
        case MELSpriteTypeBullet:
            if (self->super.hitTimer == 0.0f && MELHitboxCollidesWithRectangle(other->hitbox, playerHitbox)) {
                GameSceneLifeSubstractOne();

                // Relâche le sprite porté.
                self->isCatching = false;

                // Endommagement du sprite touché.
                other->hitPoints -= PLAYER_COLLISION_DAMAGE;
                if (otherType != MELSpriteTypeCollidable && other->hitPoints <= 0) {
                    ExplosionConstructor(other->frame.origin);
                    playdate->sprite->setUpdateFunction(otherSprite, other->class->destroy);
                }

                // Explosion du joueur.
                explode(self, sprite);

                // Bruit d'explosion.
                SamplePlay(SamplePlayerExplosion);
                return;
            }
            break;
        default:
            // Pas de traitement.
            break;
        }
    }
}

#pragma mark - Saki

static void updateSaki(LCDSprite * _Nonnull sprite) {
    Player *self = playdate->sprite->getUserdata(sprite);

    const float delta = DELTA;
    const MELController controller = MELControllerMake();

    MELPoint origin = self->super.frame.origin;

    // Déplacement
    if (!MELPointEquals(controller.axe, MELPointZero)) {
        const float effectiveSpeed = controller.pressingB ? kSakiMaximumSpeed / 2.0f : kSakiMaximumSpeed;

        const MELSize halfSize = MELSizeDivideByValue(self->super.frame.size, 2.0f);

        origin.x = MELFloatBound(halfSize.width, origin.x + effectiveSpeed * delta * controller.axe.x, LCD_COLUMNS - halfSize.width);
        origin.y = MELFloatBound(halfSize.height, origin.y + effectiveSpeed * delta * controller.axe.y, LCD_ROWS - halfSize.height);
        self->super.frame.origin = origin;
    }
    camera.frame.origin.y = 32.0f - (origin.y / LCD_ROWS) * 32.0f;

    checkSakiCollisionsWithOtherSprites(self, sprite);
    self->super.hitTimer = MELFloatMax(self->super.hitTimer - delta, 0.0f);

    draw(self, sprite);
}

static MELBoolean punchHasConnected(Punch * _Nonnull self, MELRectangle punchHitbox, MELHitbox * _Nonnull otherHitbox) {
    return self->state == Punching && MELHitboxCollidesWithRectangle(otherHitbox, punchHitbox);
}

static void checkSakiCollisionsWithOtherSprites(Player * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELRectangle playerHitbox = MELHitboxGetFrame(self->super.hitbox);
    Punch *leftPunch = (Punch *)self->leftHand;
    Punch *rightPunch = (Punch *)self->hands;
    const MELRectangle leftPunchHitbox = MELHitboxGetFrame(leftPunch->super.super.hitbox);
    const MELRectangle rightPunchHitbox = MELHitboxGetFrame(rightPunch->super.super.hitbox);
    // Pas de suppression de sprites dans l'itération donc pas besoin de copier la liste.
    LCDSpriteRefList *otherSprites = &currentScene->sprites;
    for (unsigned int index = 0; index < otherSprites->count && self->super.animationName != AnimationNameHurt; index++) {
        // NOTE: Attention ! memory peut être désalloué en cas d'AJOUT de sprites (exemple : explosion).
        LCDSprite *otherSprite = otherSprites->memory[index];
        MELSprite *other = playdate->sprite->getUserdata(otherSprite);
        const MELSpriteType otherType = other->definition.type;
        switch (otherType) {
        case MELSpriteTypeBullet:
            if (punchHasConnected(leftPunch, leftPunchHitbox, other->hitbox)
                || punchHasConnected(rightPunch, rightPunchHitbox, other->hitbox)) {
                playdate->sprite->setUpdateFunction(otherSprite, other->class->destroy);
                SamplePlay(SampleBulletCollision);
                continue;
            }
            break;
        case MELSpriteTypeEnemy:
            if (punchHasConnected(leftPunch, leftPunchHitbox, other->hitbox)) {
                const int otherHitPoints = other->hitPoints;
                other->hitPoints = otherHitPoints > 1 ? MELIntMax(otherHitPoints - (int)leftPunch->speed / 100, 1) : 0;

                throwSprite(otherSprite, self, MELPointAngleToPoint(other->frame.origin, leftPunchHitbox.origin), leftPunch->speed * 2.0f);
                PunchDidLand(leftPunch);
                GameSceneComboFillTimer();

                SamplePlay(SampleCollision);
                continue;
            }
            else if (punchHasConnected(rightPunch, rightPunchHitbox, other->hitbox)) {
                const int otherHitPoints = other->hitPoints;
                other->hitPoints = otherHitPoints > 1 ? MELIntMax(otherHitPoints - (int)rightPunch->speed / 100, 1) : 0;

                throwSprite(otherSprite, self, MELPointAngleToPoint(other->frame.origin, rightPunchHitbox.origin), rightPunch->speed * 2.0f);
                PunchDidLand(rightPunch);
                GameSceneComboFillTimer();

                SamplePlay(SampleCollision);
                continue;
            }
            break;
        case MELSpriteTypeCollidable: // Boss
            if (other->definition.name == SpriteNameGunnerBot && other->hitTimer == 0.0f) {
                if (punchHasConnected(leftPunch, leftPunchHitbox, other->hitbox)) {
                    GunnerBotThrow(otherSprite, MELPointAngleToPoint(other->frame.origin, leftPunchHitbox.origin), leftPunch->speed * 2.0f, true);
                    PunchDidLand(leftPunch);
                    SamplePlay(SampleCollision);
                    continue;
                }
                else if (punchHasConnected(rightPunch, rightPunchHitbox, other->hitbox)) {
                    GunnerBotThrow(otherSprite, MELPointAngleToPoint(other->frame.origin, rightPunchHitbox.origin), rightPunch->speed * 2.0f, true);
                    PunchDidLand(rightPunch);
                    SamplePlay(SampleCollision);
                    continue;
                }
            }
            break;
        default:
            // Saute le test de collision.
            continue;
        }
        if (self->super.hitTimer == 0.0f && MELHitboxCollidesWithRectangle(other->hitbox, playerHitbox)) {
            // Collision avec un tir ou un ennemi.
            GameSceneLifeSubstractOne();

            // Relâche le sprite porté.
            self->isCatching = false;

            // Endommagement du sprite touché.
            other->hitPoints -= PLAYER_COLLISION_DAMAGE;
            if (otherType != MELSpriteTypeCollidable && other->hitPoints <= 0) {
                ExplosionConstructor(other->frame.origin);
                playdate->sprite->setUpdateFunction(otherSprite, other->class->destroy);
            }

            // Explosion du joueur.
            explode(self, sprite);

            // Bruit d'explosion.
            SamplePlay(SamplePlayerExplosion);

            // NOTE: Quitte la boucle car détruit ET car otherSprites.memory peut avoir été désalloué !
            return;
        }
    }
}
