//
//  transitionout.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 24/11/2023.
//

#include "transitionout.h"

#include "image.h"

static void update(LCDSprite * _Nonnull sprite);

void TransitionOutConstructor(MELSpriteDefinition * _Nonnull definition) {
    LCDBitmap *image = playdate->graphics->getTableBitmap(definition->palette, 1);
    // NOTE: Vérifier si l'utilisation d'ImageConstructor ne risque pas de gêner. Il va y avoir un free du bitmap de la table. Je ne vois pas d'erreur mais c'est peut-être juste de la chance.
    LCDSprite *sprite = ImageConstructor((MELPoint) { .x = definition->size.width / 2, .y = LCD_ROWS / 2 }, image);
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    melSprite->definition.name = definition->name;
    playdate->sprite->setZIndex(sprite, ZINDEX_FADE);
    playdate->sprite->setUpdateFunction(sprite, update);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    const float time = self->hitTimer;

    // during
    const float duration = 1.0f;
    if (time < duration) {
        const float newTime = self->hitTimer = MELFloatMin(time + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);

        MELRectangle frame = self->frame;
        frame.origin.x = frame.size.width / 2 - (LCD_COLUMNS + frame.size.width / 2) * progress;
        self->frame.origin.x = frame.origin.x;

        playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    } else {
        MELSpriteDealloc(sprite);
    }
}
