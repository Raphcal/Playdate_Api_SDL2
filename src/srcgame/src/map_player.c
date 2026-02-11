//
//  map_player.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 04/04/2024.
//

#include "map_player.h"

#include "../gen/spritestoryselectcharacter.h"
#include "../gen/spritemap.h"

typedef struct {
    MELSprite super;
    MELPoint from;
    MELPoint translation;
    float time;
    uint8_t statePart;
} MapPlayer;

static MELPoint convertCoordinates(MELPoint point);

static void mainStatePart0(LCDSprite * _Nonnull sprite);
static void mainStatePart1(LCDSprite * _Nonnull sprite);
static void mainStatePart2(LCDSprite * _Nonnull sprite);
static void mainStatePart3(LCDSprite * _Nonnull sprite);
static void mainStatePart4(LCDSprite * _Nonnull sprite);

static void goToCurrentState(MapPlayer * _Nonnull self, LCDSprite * _Nonnull sprite);

LCDSprite * _Nonnull MapPlayerConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    MapPlayer *self = playdate->system->realloc(NULL, sizeof(MapPlayer));
    LCDSprite *sprite = MELSpriteInit(&self->super, definition, instance);
    self->statePart = 0;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart0);
    return sprite;
}

LCDSprite * _Nonnull MapPlayerConstructorWithCharacterAndFromAndTo(Character character, MELPoint from, MELPoint to) {
    MapPlayer * _Nonnull self = playdate->system->realloc(NULL, sizeof(MapPlayer));
    from = convertCoordinates(from);
    to = convertCoordinates(to);
    loadSpriteStorySelectCharacterPalette();

    LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, &spriteStorySelectCharacter, from);
    MELSpriteSetAnimation(&self->super, CharacterAnimationName[character]);
    self->from = from;
    self->translation = MELPointSubstract(to, from);
    self->statePart = 0;

    playdate->sprite->setUpdateFunction(sprite, mainStatePart0);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static MELPoint convertCoordinates(MELPoint point) {
    return (MELPoint) {
        .x = point.x,
        .y = point.y - spriteStorySelectCharacter.size.height / 2
    };
}

MELSprite * _Nullable MapPlayerLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    MapPlayer *self = playdate->system->realloc(NULL, sizeof(MapPlayer));
    const uint8_t statePart = MELInputStreamReadByte(inputStream);
    const float time = MELInputStreamReadFloat(inputStream);

    *self = (MapPlayer) {
        .super = {
            .class = &MELSpriteClassDefault,
        },
        .statePart = statePart,
        .time = time,
    };
    goToCurrentState(self, sprite);
    return &self->super;
}

static void goToCurrentState(MapPlayer * _Nonnull self, LCDSprite * _Nonnull sprite) {
    switch (self->statePart) {
        case 1:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart1);
            break;
        case 2:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart2);
            break;
        case 3:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart3);
            break;
        case 4:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart4);
            break;
        default:
            playdate->sprite->setUpdateFunction(sprite, mainStatePart0);
            break;
    }
}

static void draw(MapPlayer * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimationUpdate(self->super.animation, DELTA);

    const MELPoint origin = self->super.frame.origin;
    camera.frame.origin.x = MELIntBound(0, origin.x - LCD_COLUMNS / 2, spriteMap.size.width - LCD_COLUMNS);
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, self->super.animation->frame.atlasIndex), MELDirectionFlip[self->super.direction]);
}

static void mainStatePart0(LCDSprite * _Nonnull sprite) {
    MapPlayer *self = playdate->sprite->getUserdata(sprite);

    // during
    self->time = 0;
    self->statePart = 1;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart1);

    draw(self, sprite);
}

static void mainStatePart1(LCDSprite * _Nonnull sprite) {
    MapPlayer *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = 1.5f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = newTime / duration;
        self->time = newTime;

        self->super.frame.origin.y = self->from.y - 8 * fabsf(sinf(12.56637f * progress));
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    self->statePart = 2;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart2);

    draw(self, sprite);
}

static void mainStatePart2(LCDSprite * _Nonnull sprite) {
    MapPlayer *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = 0.5f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        self->time = newTime;

        // wait
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    self->statePart = 3;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart3);

    draw(self, sprite);
}

static void mainStatePart3(LCDSprite * _Nonnull sprite) {
    MapPlayer *self = playdate->sprite->getUserdata(sprite);

    // during
    const float duration = 1.0f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        const float progress = newTime / duration;
        self->time = newTime;

        self->super.frame.origin = MELPointAdd(self->from, MELPointMultiplyByValue(self->translation, progress));
        draw(self, sprite);
        return;
    }
    // during
    self->time = 0;
    self->statePart = 4;
    playdate->sprite->setUpdateFunction(sprite, &mainStatePart4);

    draw(self, sprite);
}

static void mainStatePart4(LCDSprite * _Nonnull sprite) {
    MapPlayer *self = playdate->sprite->getUserdata(sprite);

    // while
    // wait
    draw(self, sprite);
}
