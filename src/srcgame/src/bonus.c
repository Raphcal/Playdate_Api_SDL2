//
//  bonus.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 30/07/2023.
//

#include "bonus.h"

#include "gamescene.h"
#include "screen.h"
#include "samples.h"

#include "../gen/spritenames.h"
#include "../gen/spritecrystalsmall.h"
#include "../gen/spritecrystalmedium.h"
#include "../gen/spritecrystallarge.h"

typedef struct {
    MELSprite super;
    Player * _Nullable player;
    MELPoint speed;
    uint32_t createdAt;
    BonusType type;
    MELBoolean isMagnetized;
} Bonus;

const int kBonusTypeScore[3] = {5, 25, 50};
MELSpriteDefinition * _Nonnull kBonusTypeDefinition[3] = {
    &spriteCrystalSmall, &spriteCrystalMedium, &spriteCrystalLarge
};

static const uint32_t kMinimumDurationBeforeMagnetization = 150;
static const uint32_t kMinimumDurationBeforeDestruction = 750;
static const float kAngleRange = MEL_PI_2;
static const float kMagnetRange = 200.0f;
static const float kCollectRange = 16.0f;
static const float kWeight = 400.0f;
static const float kMaximumFallSpeed = 500.0f;
// Une vitesse de 900px/s permet d'être sûr de pouvoir attraper le bonus à 30fps car le bonus se déplacera à 30px/frame soit moins que les 32px de collect range.
static const float kMagnetAcceleration = 900.0f;
static const float kMaximumMagnetSpeed = 600.0f;

static void destroy(LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);

static void updateFalling(LCDSprite * _Nonnull sprite);
static void updateMagnetized(LCDSprite * _Nonnull sprite);

static const MELSpriteClass BonusClass = (MELSpriteClass) {
    .name = SpriteClassNameBonus,
    .destroy = destroy,
    .save = save,
    .load = BonusLoader,
};

LCDSprite * _Nonnull BonusConstructorWithCenter(BonusType type, MELPoint center, float angle, float speed) {
    Bonus *self = playdate->system->realloc(NULL, sizeof(Bonus));
    MELSpriteDefinition *definition = kBonusTypeDefinition[type];
    if (definition->palette == NULL) {
        definition->palette = SpriteNameLoadBitmapTable(definition->name);
    }
    LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, definition, center);
    self->super.class = &BonusClass;
    self->type = type;
    angle += MELRandomFloat(kAngleRange) - kAngleRange / 2.0f;
    self->speed = (MELPoint) {
        .x = cosf(angle) * (96.f + MELRandomFloat(32.0f)),
        .y = -192.0f - MELRandomFloat(64.0f)
    };
    self->createdAt = playdate->system->getCurrentTimeMilliseconds();
    self->isMagnetized = false;
    if (currentScene->type == SceneTypeGame) {
        GameScene *gameScene = (GameScene *)currentScene;
        self->player = gameScene->player;

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push BonusConstructorWithCenter(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
        LCDSpriteRefListPush(&gameScene->crystals, sprite);
    }
    playdate->sprite->setUpdateFunction(sprite, updateFalling);
    playdate->sprite->setZIndex(sprite, ZINDEX_BONUS);
    return sprite;
}

LCDSprite * _Nonnull BonusConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    playdate->system->error("BonusConstructor is not supported.");
    return playdate->sprite->newSprite();
}

MELSprite * _Nullable BonusLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    GameScene *gameScene = (GameScene *)currentScene;
    const MELPoint speed = MELInputStreamReadPoint(inputStream);
    const uint32_t createdAt = MELInputStreamReadUInt32(inputStream);
    const BonusType type = MELInputStreamReadByte(inputStream);
    const MELBoolean isMagnetized = MELInputStreamReadBoolean(inputStream);

    playdate->sprite->setUpdateFunction(sprite, isMagnetized ? updateMagnetized : updateFalling);

    Bonus *self = playdate->system->realloc(NULL, sizeof(Bonus));
    *self = (Bonus) {
        .super = {
            .class = &BonusClass,
        },
        .player = gameScene->player,
        .speed = speed,
        .createdAt = createdAt,
        .type = type,
        .isMagnetized = isMagnetized,
    };
    return &self->super;
}

void BonusCreate(MELSprite * _Nonnull melSprite, float angle, float speed) {
    MELRectangle frame = melSprite->frame;
    BonusType type;
    int index = 0;
    for (int value = melSprite->score, count = 0; value > 0 && count < 80; value -= kBonusTypeScore[type], count++) {
        type = MELIntMin(index / 3, BonusTypeBig);
        MELPoint center = frame.origin;
        center.x += MELRandomFloat(frame.size.width) - frame.size.width / 2.0f;
        center.y += MELRandomFloat(frame.size.height) - frame.size.height / 2.0f;
        BonusConstructorWithCenter(type, center, angle, speed);
        index++;
    }
}

const MELSpriteClass * _Nonnull BonusGetClass(void) {
    return &BonusClass;
}

static void destroy(LCDSprite * _Nonnull sprite) {
    Bonus *self = playdate->sprite->getUserdata(sprite);
    if (currentScene->type == SceneTypeGame) {
        GameScene *gameScene = GameSceneGetCurrent();
        LCDSpriteRefListRemoveSwapEntry(&gameScene->crystals, sprite);
    }
    MELAnimationDealloc(self->super.animation);
    if (self->super.hitbox != NULL) {
        MELHitboxDeinit(self->super.hitbox);
        playdate->system->realloc(self->super.hitbox, 0);
        self->super.hitbox = NULL;
    }
    playdate->system->realloc(self, 0);
    playdate->sprite->removeSprite(sprite);
    playdate->sprite->freeSprite(sprite);
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    Bonus *self = (Bonus *) sprite;
    MELOutputStreamWritePoint(outputStream, self->speed);
    MELOutputStreamWriteUInt32(outputStream, self->createdAt);
    MELOutputStreamWriteByte(outputStream, self->type);
    MELOutputStreamWriteBoolean(outputStream, self->isMagnetized);
}

static void updateFalling(LCDSprite * _Nonnull sprite) {
    Bonus *self = playdate->sprite->getUserdata(sprite);

    MELRectangle frame = self->super.frame;

    uint32_t elapsed = playdate->system->getCurrentTimeMilliseconds() - self->createdAt;

    Player *player = self->player;
    if (elapsed > kMinimumDurationBeforeMagnetization
        && currentScene->type == SceneTypeGame
        && MELPointDistanceToPoint(frame.origin, player->super.frame.origin) < kMagnetRange) {
        self->isMagnetized = true;
        playdate->sprite->setUpdateFunction(sprite, updateMagnetized);
        return;
    }

    MELPoint speed = self->speed;
    speed.y = fminf(speed.y + kWeight * DELTA, kMaximumFallSpeed);

    frame.origin.x += speed.x * DELTA;
    frame.origin.y += speed.y * DELTA;

    if (elapsed > kMinimumDurationBeforeDestruction
        && !MELRectangleIntersectsWithRectangle(frame, kScreen)) {
        destroy(sprite);
        return;
    }

    self->super.frame = frame;
    self->speed = speed;

    MELAnimation *animation = self->super.animation;
    MELAnimationUpdate(animation, DELTA);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
}

static void updateMagnetized(LCDSprite * _Nonnull sprite) {
    Bonus *self = playdate->sprite->getUserdata(sprite);

    // TODO: Gérer le cas où le joueur meurt avant d'attraper le bonus ?
    if (currentScene->type != SceneTypeGame) {
        self->speed = MELPointZero;
        playdate->sprite->setUpdateFunction(sprite, updateFalling);
        return;
    }

    MELRectangle frame = self->super.frame;

    Player *player = self->player;
    MELPoint playerOrigin = player->super.frame.origin;
    if (MELPointDistanceToPoint(frame.origin, playerOrigin) < kCollectRange) {
        GameSceneScoreAdd(kBonusTypeScore[self->type], 0);

        // Bruit de collecte
        SamplePlay(SamplePickUpBonus);

        destroy(sprite);
        return;
    }

    float speed = self->speed.x;
    speed = fminf(speed + kMagnetAcceleration * DELTA, kMaximumMagnetSpeed);

    float angleToPlayer = MELPointAngleToPoint(playerOrigin, frame.origin);

    frame.origin.x += speed * cosf(angleToPlayer) * DELTA;
    frame.origin.y += speed * sinf(angleToPlayer) * DELTA;

    self->super.frame = frame;
    self->speed.x = speed;

    MELAnimation *animation = self->super.animation;
    MELAnimationUpdate(animation, DELTA);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
}
