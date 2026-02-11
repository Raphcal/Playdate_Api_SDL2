//
//  punch.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 12/09/2023.
//

#include "punch.h"

#include "../gen/spritehero.h"

static const float kChargeDuration = 3.0f;
static const float kMinimumForce = 250.0f;
static const float kVariableForce = 400.0f;
static const float kMinimumDistance = 32.0f;
static const float kVariableDistance = 6.0f;
static const float kPreparePunchDistance = 3.0f;
static const float kComingBackAcceleration = 600.0f;
static const float kComingBackMaximumSpeed = 400.0f;

static void updateCharging(LCDSprite * _Nonnull sprite);
static void updatePunching(LCDSprite * _Nonnull sprite);
static void updateComingBack(LCDSprite * _Nonnull sprite);

Punch * _Nonnull PunchAlloc(LCDSprite * _Nonnull parent, MELBoolean isRightHand) {
    Punch *self = playdate->system->realloc(NULL, sizeof(Punch));
    MELSubSpriteInit(&self->super, parent, &spriteHero, AnimationNameAppear);
    MELSingleFrameAnimationReuse(self->super.super.animation);
    MELAnimationSetFrameIndex(self->super.super.animation, isRightHand);

    self->state = Idle + isRightHand;
    self->time = 0.0f;
    self->offset = isRightHand
        ? (MELPoint) { .x = 11, .y = 8 }
        : (MELPoint) { .x = 18, .y = 3 };

    MELSprite *player = playdate->sprite->getUserdata(parent);
    self->playerOrigin = &player->frame.origin;

    LCDSprite *sprite = self->super.sprite;
    playdate->sprite->setZIndex(sprite, isRightHand ? ZINDEX_HANDS : ZINDEX_PLAYER - 1);
    playdate->sprite->setUpdateFunction(sprite, updateCharging);
    return self;
}

void PunchDidLand(Punch * _Nonnull self) {
    self->time = 0.0f;
    self->speed = 0.0f;
    self->state = ComingBack;
    playdate->sprite->setUpdateFunction(self->super.sprite, updateComingBack);

    MELAnimation *animation = self->super.super.animation;
    MELAnimationSetFrameIndex(animation, animation->frameIndex - 2);
}

// - Lancer les poings au release du bouton A
// - Utiliser la direction left/right pour gérer le prochain poing
// - Faire clignoter/animer le chargement des poings (comment ?) -> se baser sur le temps pour faire clignoter plus vite. (1 - (progress / 3)) * vitesse clignotement, si 0, tout noir ? Faire plutôt un modulo du temps passé en fonction du temps restant ?
// - Faire varier la vitesse de poings de 100 à 600 suivant le chargement (max de Katsuo = 800)
// - Temps de chargement max = 3s
// - Gérer l'update des poings dans un fichier dédié
// - Mouvement en ligne droite suivant toujours le y de Saki, allonge de 48px à 53px suivant la charge
// - Faire un cooldown entre chaque coup de poing : attendre l'impact/le retour du poing. Regarder par rapport à la direction du poing actuel et la vitesse ?

static void draw(Punch * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimation *animation = self->super.super.animation;
    MELAnimationUpdate(animation, DELTA);

    const MELPoint origin = self->super.super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.super.definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
}

static void updateCharging(LCDSprite * _Nonnull sprite) {
    Punch *self = playdate->sprite->getUserdata(sprite);
    PDButtons pressing;
    PDButtons released;
    playdate->system->getButtonState(&pressing, NULL, &released);

    float squareProgress = 0.0f;

    const PunchState state = self->state;
    const PunchState otherPunchState = *self->otherPunchState;

    if (state == Idle && (otherPunchState == ComingBack || otherPunchState == Idle)) {
        self->state = CanCharge;
    }
    else if (pressing & kButtonA && state == CanCharge) {
        const float time = self->time += DELTA;
        const float progress = MELProgress(0.0f, kChargeDuration, time);
        squareProgress = progress * progress;

        const LCDBitmapDrawMode drawMode = ((int)(squareProgress * 100.0f) % 20 < 10) ? kDrawModeInverted : kDrawModeCopy;
        if (drawMode != self->super.super.drawMode) {
            self->super.super.drawMode = drawMode;
            playdate->sprite->setDrawMode(sprite, drawMode);
        }
    }
    else if (released & kButtonA && self->time > 0.0f) {
        const float progress = MELProgress(0.0f, kChargeDuration, self->time);
        squareProgress = progress * progress;
        self->time = 0.0f;
        self->speed = squareProgress * kVariableForce + kMinimumForce;
        self->prepareOffset = squareProgress * kPreparePunchDistance;
        self->distance = squareProgress * (kVariableDistance + kPreparePunchDistance) + kMinimumDistance;
        self->super.super.drawMode = kDrawModeCopy;
        self->state = Punching;
        playdate->sprite->setUpdateFunction(sprite, updatePunching);
        playdate->sprite->setDrawMode(sprite, kDrawModeCopy);

        MELAnimation *animation = self->super.super.animation;
        MELAnimationSetFrameIndex(animation, animation->frameIndex + 2);
    }

    MELPoint playerOrigin = *self->playerOrigin;
    MELPoint offset = self->offset;
    self->super.super.frame.origin = (MELPoint) {
        .x = playerOrigin.x + offset.x - kPreparePunchDistance * squareProgress,
        .y = playerOrigin.y + offset.y,
    };

    // TODO: Faire clignoter en fonction du chargement ?
    draw(self, sprite);
}

static void updatePunching(LCDSprite * _Nonnull sprite) {
    Punch *self = playdate->sprite->getUserdata(sprite);
    self->time = MELFloatMin(self->time + self->speed * DELTA, self->distance);

    MELPoint playerOrigin = *self->playerOrigin;
    MELPoint offset = self->offset;
    self->super.super.frame.origin = (MELPoint) {
        .x = playerOrigin.x + offset.x - self->prepareOffset + self->time,
        .y = playerOrigin.y + offset.y - offset.y * MELEaseInOut(0.0f, self->distance, self->time),
    };

    if (self->time == self->distance) {
        PunchDidLand(self);
    }

    draw(self, sprite);
}

static void updateComingBack(LCDSprite * _Nonnull sprite) {
    Punch *self = playdate->sprite->getUserdata(sprite);
    const float speed = self->speed = MELFloatMin(self->speed + kComingBackAcceleration * DELTA, kComingBackMaximumSpeed);

    MELPoint playerOrigin = *self->playerOrigin;
    MELPoint offset = self->offset;
    MELPoint origin = self->super.super.frame.origin;
    origin.x = MELFloatMax(origin.x - speed * DELTA, playerOrigin.x + offset.x);
    origin.y = MELFloatMin(origin.y + speed * DELTA, playerOrigin.y + offset.y);
    self->super.super.frame.origin = origin;

    if (origin.x <= playerOrigin.x + offset.x) {
        self->time = 0.0f;
        self->speed = 0.0f;
        self->state = Idle;
        playdate->sprite->setUpdateFunction(sprite, updateCharging);
    }

    draw(self, sprite);
}
