//
//  rallyintroscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 13/07/2025.
//

#include "rallyintroscene.h"

#include "rallyscene.h"
#include "rallyscorescene.h"
#include "tiledbackground.h"
#include "image.h"
#include "stride.h"
#include "savegame.h"
#include "music.h"
#include "samples.h"
#include "confetti.h"
#include "rect.h"
#include "text.h"

#include "../gen/spriteportaits.h"
#include "../gen/spritenscard6x21x1.h"
#include "../gen/spritensselect8x66x5.h"
#include "../gen/spritewinlosemessage.h"

static const int kCellWidth = 94;
static const int kCellHeight = 89;

static const LCDColor kGrey50Pattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = noir, 1 = blanc
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    // Mask, 0 = transparent, 1 = visible
    0b11001100,
    0b10011001,
    0b00110011,
    0b01100110,
    0b11001100,
    0b10011001,
    0b00110011,
    0b01100110,
};

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull scene);
static void beforeQuit(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected);

RallyIntroScene * _Nonnull RallyIntroSceneAlloc(ArcadeGame game) {
    RallyIntroScene *self = new(RallyIntroScene);
    loadSpriteNsCard6x21x1Palette();
    loadSpriteNsSelect8x66x5Palette();
    *self = (RallyIntroScene) {
        .super = {
            .type = SceneTypeRallyIntro,
            .init = init,
            .update = update,
            .dealloc = dealloc,
            .beforeQuit = beforeQuit,
        },
        .border = MELNineSliceMakeWithBitmapTable(spriteNsCard6x21x1.palette, 0, (MELIntRectangle) {
            .origin = { .x = 6, .y = 2 },
            .size = { .width = 1, .height = 1 }
        }),
        .selectedBorder = MELNineSliceMakeWithBitmapTable(spriteNsSelect8x66x5.palette, 0, (MELIntRectangle) {
            .origin = { .x = 8, .y = 6 },
            .size = { .width = 4, .height = 5 }
        }),
        .game = game,
    };
    return self;
}

static void init(MELScene * _Nonnull scene) {
    RallyIntroScene *self = (RallyIntroScene *)scene;
    LCDSprite *background = TiledBackgroundAlloc(loadMapLayer(MapNameFileSelect, 1), MELPointMake(-32.0f, 32.0f));
    LCDSpriteRefListPush(&self->super.sprites, background);

    // NOTE: Fait un push automatiquement
    ImageConstructor(MELPointMake(LCD_COLUMNS / 2, LCD_ROWS / 2), loadMapLayer(MapNameFileSelect, 2));

    const ArcadeGame game = self->game;
    const int encounterCount = (game.deathCount == 0 || game.stage == kRallyEncounters.count - 1)
        ? kRallyEncounters.count
        : kRallyEncounters.count - 1;

    // Taille du portrait : 82x77
    // Avec bordure simple : 86x82
    // Avec bordure sélection : 94x89
    LCDSprite *gridView = MELGridViewMake((MELRectangle) {
        .origin = {
            .x = LCD_COLUMNS / 2,
            .y = LCD_ROWS / 2,
        },
        .size = {
            .width = LCD_COLUMNS,
            .height = kCellHeight,
        }
    }, MELIntSizeMake(encounterCount, 1), MELIntSizeMake(kCellWidth, kCellHeight), drawCell, self);
    LCDSpriteRefListPush(&self->super.sprites, gridView);
    self->gridView = playdate->sprite->getUserdata(gridView);
    self->gridView->inset.left = 32;
    self->gridView->userdata = self;
    self->gridView->disableInputs = true;
    self->gridViewSprite = gridView;
    MELGridViewSetSelection(gridView, MELIntPointMake(self->game.stage == 0 ? 0 : self->game.stage - 1, 0));
    playdate->sprite->setZIndex(gridView, 1);

    if (self->game.stage == 0) {
        MELPoint origin = self->gridView->super.frame.origin;
        origin.x += LCD_COLUMNS;
        self->gridView->super.frame.origin = origin;
        playdate->sprite->moveTo(gridView, origin.x, origin.y);
        self->gridView->super.class->update(gridView);
        StrideSpriteTo(gridView, MELPointMake(LCD_COLUMNS / 2, LCD_ROWS / 2), 0.0f, 1.0f);
    }

    MusicManagerPlay("musics/map", 1, 0.0f);
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeRallyIntro) {
        playdate->system->error("Given scene is not a RallyIntroScene instance");
        return;
    }
    RallyIntroScene *self = (RallyIntroScene *)scene;

    if (self->border) {
        MELNineSliceDealloc(self->border);
        self->border = NULL;
    }
    if (self->selectedBorder) {
        MELNineSliceDealloc(self->selectedBorder);
        self->border = NULL;
    }

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);

    playdate->system->realloc(self, 0);
}

static void beforeQuit(MELScene * _Nonnull scene) {
    RallyIntroScene *self = (RallyIntroScene *)scene;

    currentSaveGame.arcadeSaveState = (ArcadeSaveState) {
        .game = self->game,
    };
    SaveGameToDisk(&currentSaveGame);
}

static int update(void * _Nonnull userdata) {
    RallyIntroScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;

    // Déplace et affiche les sprites.
    playdate->sprite->updateAndDrawSprites();

    const float time = self->time;
    self->time = time + DELTA;

    const ArcadeGame game = self->game;

    if (game.stage && time < 0.25f && self->time >= 0.25f) {
        MELPoint origin = MELGridViewGetSelectionCenter(self->gridView);
        ConfettiConstructor((MELRectangle) {
            .origin = origin,
            .size = {
                .width = kCellWidth,
                .height = kCellHeight,
            }
        }, 32);
        SamplePlay(SampleBang);
    }
    else if (game.stage && time < 1.0f && self->time >= 1.0f) {
        MELGridViewSetSelectionAnimated(self->gridViewSprite, MELIntPointMake(game.stage, 0), 0);
    }

    const MELController controller = MELControllerMake();
    if (controller.pressedB || self->time >= 5.0f) {
        RallyScene *rallyScene = RallySceneAlloc(game);
        MELSceneMakeCurrent(&rallyScene->super);
    }
    return true;
}

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected) {
    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->setDrawMode(kDrawModeCopy);
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);

    RallyIntroScene *self = gridView->userdata;

    loadSpritePortaitsPalette();
    const Character character = kRallyEncounters.memory[x].opponent;
    const MELAnimationDefinition *animationDefinition = MELSpriteDefinitionGetAnimationDefinition(spritePortaits, CharacterAnimationName[character], CharacterAnimationDirection[character]);
    LCDBitmap *portrait = graphics->getTableBitmap(spritePortaits.palette, animationDefinition->frames[0].atlasIndex);
    // TODO: Ne pas dessiner le contour "sélection" ici mais plutôt faire un sprite au dessus et faire juste bouger le scrolling vers la cellule suivante.
    // TODO: Masquer le portrait 20 avec des ???
    int portraitTop, portraitLeft;
    if (isSelected) {
        MELNineSliceDrawInRect(self->selectedBorder, MELRectangleMake(0, 0, 94, 89));
        portraitLeft = 6;
        portraitTop = 5;
    } else {
        const int left = (94 - 86) / 2;
        const int top = (89 - 82) / 2;
        MELNineSliceDrawInRect(self->border, MELRectangleMake(left, top, 86, 82));
        portraitLeft = left + 2;
        portraitTop = top + 1;
    }
    graphics->drawBitmap(portrait, portraitLeft, portraitTop, kBitmapUnflipped);
    if (x < self->game.stage) {
        graphics->fillRect(portraitLeft, portraitTop, spritePortaits.size.width, spritePortaits.size.height, kGrey50Pattern);
    }

    graphics->popContext();
}
