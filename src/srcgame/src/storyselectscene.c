//
//  storyselectscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 25/01/2024.
//

#include "storyselectscene.h"

#include "story.h"
#include "mapscene.h"
#include "titlescene.h"
#include "dialogscene.h"
#include "savegame.h"
#include "tiledbackground.h"
#include "image.h"
#include "music.h"
#include "storyselect_character.h"
#include "storyselect_statusbar.h"

#include "../gen/spriteportaits.h"
#include "../gen/spritestoryclear.h"
#include "../gen/spritenscard6x21x1.h"
#include "../gen/spritensselect8x66x5.h"

static const int cellWidth = 112;
static const int cellHeight = 176;

static const int kRefreshRate = 50;
static const float kFrameTime = 1.0f / 50.0f;

static const MELLocalizedString ending = (MELLocalizedString) {
    .text = {
        "Ending",
        "Fin",
        "エンディング",
        "Final"
    }
};

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull self);
static int update(void * _Nonnull userdata);
static int updateDialog(void * _Nonnull userdata);

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected);
static void getChoiceLabel(const void * _Nullable userdata, int index, char * _Nonnull buffer, int bufferLength);

StorySelectScene * _Nonnull StorySelectSceneAlloc(void) {
    StorySelectScene *self = playdate->system->realloc(NULL, sizeof(StorySelectScene));
    *self = (StorySelectScene) {
        .super = {
            .type = SceneTypeStorySelect,
            .init = init,
            .dealloc = dealloc,
            .update = update,
        },
        .backgroundMap = MELMapOpen(kMapNameFileNames[MapNameFileSelect]),
        .cellBackground = loadMapLayer(MapNameFileSelect, 4),
        .selected = loadMapLayer(MapNameFileSelect, 5),
    };
    return self;
}


static void init(MELScene * _Nonnull scene) {
    StorySelectScene *self = (StorySelectScene *)scene;

    LCDSprite *background = TiledBackgroundAlloc(loadMapLayer(MapNameFileSelect, 1), MELPointMake(-32.0f, 32.0f));
    LCDSpriteRefListPush(&self->super.sprites, background);

    // NOTE: Fait un push automatiquement
    ImageConstructor(MELPointMake(LCD_COLUMNS / 2, LCD_ROWS / 2), loadMapLayer(MapNameFileSelect, 2));

    LCDSprite *gridView = MELGridViewMake((MELRectangle) {
        .origin = {
            .x = LCD_COLUMNS / 2,
            .y = LCD_ROWS / 2 - 8,
        },
        .size = {
            .width = LCD_COLUMNS,
            .height = cellHeight,
        }
    }, MELIntSizeMake(kStoryCount, 1), MELIntSizeMake(cellWidth, cellHeight), drawCell, self);
    LCDSpriteRefListPush(&self->super.sprites, gridView);
    self->gridView = playdate->sprite->getUserdata(gridView);
    self->gridView->inset.left = 32;
    self->gridView->userdata = self;
    MELGridViewSetSelection(gridView, MELIntPointMake(1, 0));
    playdate->sprite->setZIndex(gridView, 1);

    for (unsigned int index = 0; index < kStoryCount; index++) {
        LCDSprite *sprite = StorySelectCharacterConstructor(index, self->gridView);
        playdate->sprite->setZIndex(sprite, 2);
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    StorySelectStatusBarConstructor(self->gridView);

    MusicManagerPlay("musics/menu", 0, 0.25f);

    camera.frame.origin = MELPointZero;
    setRefreshRate(kRefreshRate);
}

static void dealloc(MELScene * _Nonnull scene) {
    StorySelectScene *self = (StorySelectScene *)scene;

    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->backgroundMap);
    if (self->cellBackground) {
        playdate->graphics->freeBitmap(self->cellBackground);
        self->cellBackground = NULL;
    }
    if (self->selected) {
        playdate->graphics->freeBitmap(self->selected);
        self->selected = NULL;
    }
    if (spriteStoryClear.palette) {
        playdate->graphics->freeBitmapTable(spriteStoryClear.palette);
        spriteStoryClear.palette = NULL;
    }
    if (self->menuBorder) {
        MELNineSliceDealloc(self->menuBorder);
        self->menuBorder = NULL;
    }
    if (self->menuSelection) {
        MELNineSliceDealloc(self->menuSelection);
        self->menuSelection = NULL;
    }
    playdate->system->realloc(self, 0);
}

static void createDialog(StorySelectScene *self, const StoryName storyName) {
    const Story *story = kStories[storyName];
    const int8_t progression = currentSaveGame.progression[storyName];
    const MELBoolean completed = currentSaveGame.completion[storyName] != 0;

    if (!self->menuBorder) {
        loadSpriteNsCard6x21x1Palette();
        self->menuBorder = MELNineSliceMakeWithBitmapTable(spriteNsCard6x21x1.palette, 0, (MELIntRectangle) {
            .origin = { .x = 6, .y = 2 },
            .size = { .width = 1, .height = 1 }
        });
    }
    if (!self->menuSelection) {
        loadSpriteNsSelect8x66x5Palette();
        self->menuSelection = MELNineSliceMakeWithBitmapTable(spriteNsSelect8x66x5.palette, 0, (MELIntRectangle) {
            .origin = { .x = 8, .y = 6 },
            .size = { .width = 4, .height = 5 }
        });
    }
    MELPoint gridSelectionCenter = MELGridViewGetSelectionCenter(self->gridView);
    int count;
    if (completed) {
#if ENABLE_ENDING_SELECTION_IF_COMPLETED
        count = MELIntMax(story->encounters.count, progression + 1);
#else
        count = MELIntMax(story->encounters.count - 1, progression + 1);
#endif
    } else {
        count = progression + 1;
    }
    LCDSprite *dialog = MELDialogMake((MELDialogConfig) {
        .count = count,
        .getChoiceLabel = getChoiceLabel,
        .userdata = story,
        .initialSelection = progression,
        .font = smallFont,
        .border = self->menuBorder,
        .selectionBorder = self->menuSelection,
        .padding = {
            .top = 1,
            .right = 1,
            .bottom = 1,
            .left = 1,
        },
        .frame = {
            .origin = {
                .x = gridSelectionCenter.x + 64.0f,
            }
        }
    });
    LCDSpriteRefListPush(&self->super.sprites, dialog);
    self->dialog = dialog;
}

static int update(void * _Nonnull userdata) {
    StorySelectScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), kFrameTime);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;
    playdate->sprite->updateAndDrawSprites();

    PDButtons pressed;
    playdate->system->getButtonState(NULL, &pressed, NULL);
    if (pressed & kButtonA) {
        const MELIntPoint selection = self->gridView->selection;
        const StoryName storyName = selection.x;
        const Story *story = kStories[storyName];
        if (story == NULL) {
            playdate->system->error("Selected story is null!");
            return true;
        }
        const MELBoolean isUnlocked = StoryIsUnlocked(storyName);
        const int8_t progression = currentSaveGame.progression[storyName];
        const MELBoolean completed = currentSaveGame.completion[storyName] != 0;
        if (isUnlocked && (progression || completed)) {
            self->gridView->disableInputs = true;
            createDialog(self, storyName);
            playdate->system->setUpdateCallback(updateDialog, self);
        } else if (isUnlocked) {
            currentSaveGame.lastCharacter = story->character;
            SaveGameToDisk(&currentSaveGame);
            MapScene *mapScene = MapSceneAlloc(story, 0);
            MELSceneMakeCurrent(&mapScene->super);
        } else {
            // TODO: Jouer un son !
        }
        return true;
    }
    else if (pressed & kButtonB) {
        TitleScene *titleScene = TitleSceneAlloc();
        titleScene->initialMenuSelection = TitleMenuItemStory;
        MELSceneMakeCurrent(&titleScene->super);
        return true;
    }
    return true;
}

static int updateDialog(void * _Nonnull userdata) {
    StorySelectScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), kFrameTime);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;
    playdate->sprite->updateAndDrawSprites();

    MELDialog *dialog = playdate->sprite->getUserdata(self->dialog);
    switch (dialog->state) {
        case MELDialogStateClosed:
            playdate->system->setUpdateCallback(update, self);
            self->gridView->disableInputs = false;
            dialog->super.class->destroy(self->dialog);
            self->dialog = NULL;
            // fallthrough
        case MELDialogStateOpening:
        case MELDialogStateClosing:
            // Attente de la fin de l'animation.
            return true;
        default:
            break;
    }

    PDButtons pressed;
    playdate->system->getButtonState(NULL, &pressed, NULL);
    if (pressed & kButtonA) {
        // TODO: Fermer la boîte de dialogue avant de passer à la scène suivante.
        const MELIntPoint selection = self->gridView->selection;
        const StoryName storyName = selection.x;
        const Story *story = kStories[storyName];
        currentSaveGame.lastCharacter = story->character;
        SaveGameToDisk(&currentSaveGame);
        const int encounterIndex = MELDialogGetSelection(self->dialog);
        if (encounterIndex == story->encounters.count - 1) {
            DialogScene *dialogScene = DialogSceneAlloc(story, encounterIndex);
            MELSceneMakeCurrent(&dialogScene->super);
        } else {
            MapScene *mapScene = MapSceneAlloc(story, encounterIndex);
            MELSceneMakeCurrent(&mapScene->super);
        }
    } else if (pressed & kButtonB) {
        MELDialogClose(self->dialog);
    }
    return true;
}

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected) {
    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->setDrawMode(kDrawModeCopy);
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);

    StorySelectScene *self = gridView->userdata;

    graphics->drawBitmap(self->cellBackground, 0, 0, kBitmapUnflipped);
    loadSpritePortaitsPalette();
    const StoryName storyName = x;
    const Story *story = kStories[storyName];
    const Character character = story != NULL ? story->character : CharacterNone;
    const MELAnimationDefinition *animationDefinition = MELSpriteDefinitionGetAnimationDefinition(spritePortaits, CharacterAnimationName[character], CharacterAnimationDirection[character]);
    LCDBitmap *portrait = graphics->getTableBitmap(spritePortaits.palette, animationDefinition->frames[0].atlasIndex);
    if (portrait && StoryIsUnlocked(storyName)) {
        graphics->drawBitmap(portrait, 16, 18, kBitmapUnflipped);
    }
    if (isSelected) {
        graphics->drawBitmap(self->selected, 0, 0, kBitmapUnflipped);
    }
    if (currentSaveGame.completion[storyName]) {
        loadSpriteStoryClearPalette();
        LCDBitmap *clear = graphics->getTableBitmap(spriteStoryClear.palette, 0);
        graphics->drawBitmap(clear, cellWidth - 8 - spriteStoryClear.size.width, 8, kBitmapUnflipped);
    }

    graphics->popContext();
}

static void getChoiceLabel(const void * _Nullable userdata, int index, char * _Nonnull buffer, int bufferLength) {
    const Story *story = userdata;
    const Character opponent = story->encounters.memory[index].opponent;
    const MELLocalizedString name = opponent == CharacterNone ? ending : CharacterName[opponent];
    const char *localizedName = MELLocalizedStringGetText(name);

    const int order = index + 1;

    int next;
    if (order < 10) {
        buffer[0] = '0' + order;
        buffer[1] = '.';
        buffer[2] = ' ';
        next = 3;
    } else {
        buffer[0] = '0' + (order / 10);
        buffer[1] = '0' + (order % 10);
        buffer[2] = '.';
        buffer[3] = ' ';
        next = 4;
    }

    const unsigned int length = (unsigned int) strlen(localizedName);
    memcpy(buffer + next, localizedName, length * sizeof(char));
    buffer[next + length] = '\0';
}
