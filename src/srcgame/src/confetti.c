//
//  confetti.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 04/08/2025.
//

#include "confetti.h"

#include "../gen/spriteconfetti.h"

typedef struct {
    MELSprite super;
    MELPoint speed;
    LCDBitmapDrawMode drawMode;
} Confetti;

static const float kAerobrake = 200.0f;
static const float kGravity = 64.0f;

static void update(LCDSprite * _Nonnull sprite);

void ConfettiConstructor(MELRectangle zone, int count) {
    loadSpriteConfettiPalette();

    LCDSpriteRef memory[count];
    const struct playdate_sprite *spriteAPI = playdate->sprite;
    for (unsigned int index = 0; index < count; index++) {
        Confetti *self = new(Confetti);
        const MELPoint center = (MELPoint) {
            .x = zone.origin.x - zone.size.width / 2 + MELRandomFloat(zone.size.width),
            .y = zone.origin.y - zone.size.height / 4 + MELRandomFloat(zone.size.height),
        };
        LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, &spriteConfetti, center);
        self->speed = (MELPoint) {
            .x = MELRandomFloat(zone.size.width) - zone.size.width / 2,
            .y = -MELRandomFloat(zone.size.height) * 6.5f - zone.size.height
        };
        self->drawMode = MELRandomInt(2) == 0 ? kDrawModeFillBlack : kDrawModeCopy;
        MELSpriteSetAnimation(&self->super, MELRandomInt(3));
        spriteAPI->setUpdateFunction(sprite, update);
        spriteAPI->setZIndex(sprite, 32);
        memory[index] = sprite;
    }
    LCDSpriteRefListAddAll(&currentScene->sprites,  (LCDSpriteRefList) {
        .count = count,
        .memory = memory
    });
}

void update(LCDSprite * _Nonnull sprite) {
    Confetti *self = playdate->sprite->getUserdata(sprite);

    MELPoint origin = self->super.frame.origin;
    origin = MELPointAdd(origin, MELPointMultiplyByValue(self->speed, DELTA));
    self->super.frame.origin = origin;

    self->speed.y += (self->speed.y < 0 ? kAerobrake : kGravity) * DELTA;

    const MELPoint screenCoordinates = MELPointSubstract(origin, camera.frame.origin);
    if (screenCoordinates.x < -spriteConfetti.size.width || screenCoordinates.x > LCD_COLUMNS + spriteConfetti.size.width || screenCoordinates.y > LCD_ROWS) {
        self->super.class->destroy(sprite);
        return;
    }

    MELSpriteUpdate(sprite);
    playdate->sprite->setDrawMode(sprite, self->drawMode);
}
