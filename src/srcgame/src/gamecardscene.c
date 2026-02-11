//
//  gamecardscene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 12/09/2023.
//

#include "gamecardscene.h"

#include "camera.h"
#include "../gen/spritetitlebelt.h"
#include "../gen/spritetitlelogo.h"
#include "../gen/spritetitlewaist.h"
#include "../gen/spritecharacterselectgrid.h"
#if ENABLE_GAME_CARD
#include "../gen/spritecomingearly2024.h"
#include "../gen/spriteonlyonplaydate.h"
#include "../gen/spritebyme.h"
#endif
 
#include "titlescene.h"

typedef struct {
    MELScene super;

    LCDSprite * _Nullable belt;
    LCDSprite * _Nullable byMe;
    LCDSprite * _Nullable comingEarly2024;
    LCDSprite * _Nullable grid1;
    LCDSprite * _Nullable grid2;
    LCDSprite * _Nullable grid3;
    LCDSprite * _Nullable grid4;
    LCDSprite * _Nullable logo;
    LCDSprite * _Nullable onlyOnPlaydate;
    LCDSprite * _Nullable waist;
    float time;
} GameCardScene;

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull self);
static int update(void * _Nonnull userdata);

static LCDSprite * _Nonnull createSprite(MELSpriteDefinition * _Nonnull definition, MELPoint origin);

MELScene * _Nonnull GameCardSceneAlloc(void) {
    GameCardScene *self = playdate->system->realloc(NULL, sizeof(GameCardScene));
    *self = (GameCardScene) {
        .super = {
            .init = init,
            .dealloc = dealloc,
            .update = update,
        }
    };
    return &self->super;
}

static void init(MELScene * _Nonnull scene) {
    GameCardScene *self = (GameCardScene *)scene;
    self->belt = createSprite(&spriteTitleBelt, (MELPoint) { .x = 179.0f, .y = 141.0f });
    self->waist = createSprite(&spriteTitleWaist, (MELPoint) { .x = 388.0f, .y = 121.0f });
    self->grid1 = createSprite(&spriteCharacterSelectGrid, (MELPoint) { .x = 60.0f, .y = 120.0f });
    self->grid2 = createSprite(&spriteCharacterSelectGrid, (MELPoint) { .x = 180.0f, .y = 120.0f });
    self->grid3 = createSprite(&spriteCharacterSelectGrid, (MELPoint) { .x = 300.0f, .y = 120.0f });
    self->grid4 = createSprite(&spriteCharacterSelectGrid, (MELPoint) { .x = 420.0f, .y = 120.0f });
    playdate->sprite->setVisible(self->grid1, false);
    playdate->sprite->setVisible(self->grid2, false);
    playdate->sprite->setVisible(self->grid3, false);
    playdate->sprite->setVisible(self->grid4, false);
    self->logo = createSprite(&spriteTitleLogo, (MELPoint) { .x = 290.0f, .y = 120.0f });
#if ENABLE_GAME_CARD
    self->comingEarly2024 = createSprite(&spriteComingEarly2024, (MELPoint) { .x = 512.0f, .y = 120.0f }); // 260
    self->byMe = createSprite(&spriteByMe, (MELPoint) { .x = 460.0f, .y = 215.0f }); // 170
    self->onlyOnPlaydate = createSprite(&spriteOnlyOnPlaydate, (MELPoint) { .x = 473.0f, .y = 210.0f }); // 318
#endif
}

static void dealloc(MELScene * _Nonnull scene) {
    GameCardScene *self = (GameCardScene *)scene;
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    playdate->system->realloc(self, 0);
}

static LCDSprite * _Nonnull createSprite(MELSpriteDefinition * _Nonnull definition, MELPoint origin) {
    definition->palette = SpriteNameLoadBitmapTable(definition->name);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->moveTo(sprite, origin.x, origin.y);

    MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
    *melSprite = (MELSprite) {
        .class = &MELSpriteClassDefault,
        .definition = *definition,
        .frame = (MELRectangle) {
            .origin = origin,
            .size = definition->size
        },
        .direction = MELDirectionRight,
    };
    MELSpriteSetAnimation(melSprite, AnimationNameStand);

    playdate->sprite->setUserdata(sprite, melSprite);
    playdate->sprite->setUpdateFunction(sprite, &MELSpriteUpdate);
    playdate->sprite->addSprite(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_PLAYER);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void setSpriteX(LCDSprite * _Nonnull sprite, float x) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->frame.origin.x = x;
}

static int update(void * _Nonnull userdata) {
    GameCardScene *self = userdata;

    const float transitionStart = 1.0f;
    const float transitionEnd = 2.0f;
    if (self->time < transitionEnd) {
        const float newTime = self->time += DELTA;
        if (newTime >= transitionStart) {
            MELSpriteSetAnimation(playdate->sprite->getUserdata(self->grid1), AnimationNameAppear);
            MELSpriteSetAnimation(playdate->sprite->getUserdata(self->grid2), AnimationNameAppear);
            MELSpriteSetAnimation(playdate->sprite->getUserdata(self->grid3), AnimationNameAppear);
            MELSpriteSetAnimation(playdate->sprite->getUserdata(self->grid4), AnimationNameAppear);
            playdate->sprite->setVisible(self->grid1, true);
            playdate->sprite->setVisible(self->grid2, true);
            playdate->sprite->setVisible(self->grid3, true);
            playdate->sprite->setVisible(self->grid4, true);
        }

        const float progress = MELEaseInOut(transitionStart, transitionEnd, newTime);
        setSpriteX(self->logo, 290 - 220 * progress);
        setSpriteX(self->comingEarly2024, 512 + (260 - 512) * progress);
        setSpriteX(self->byMe, 460 + (170 - 460) * progress);
        setSpriteX(self->onlyOnPlaydate, 473 + (318 - 473) * progress);
    }

    playdate->sprite->updateAndDrawSprites();

    PDButtons pressed;
    playdate->system->getButtonState(NULL, &pressed, NULL);
    if (pressed & kButtonB) {
        TitleScene *titleScene = TitleSceneAlloc();
        MELSceneMakeCurrent(&titleScene->super);
    }
    return true;
}
