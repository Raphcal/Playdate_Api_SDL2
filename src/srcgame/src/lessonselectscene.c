//
//  lessonselectscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 21/03/2024.
//

#include "lessonselectscene.h"

#include "music.h"
#include "layersprite.h"
#include "titlescene.h"
#include "dialogscene.h"
#include "lessons.h"

#include "../gen/spritemenucursor.h"
#include "../gen/spritecheckmark.h"
#include "../gen/spritestoryclear.h"

static const MapName mapName = MapNameSchool;

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull self);

static void createMenuGrid(LessonSelectScene * _Nonnull self);
static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected);

static void setTitleLanguage(LCDSprite * _Nonnull sprite);
static void onLanguageChanged(void * _Nonnull userdata, int language);

static int update(void * _Nonnull userdata);

LessonSelectScene * _Nonnull LessonSelectSceneAlloc(void) {
    LessonSelectScene *self = playdate->system->realloc(NULL, sizeof(LessonSelectScene));
    *self = (LessonSelectScene) {
        .super = {
            .type = SceneTypeLessonSelect,
            .init = init,
            .dealloc = dealloc,
            .update = update,
        },
        .map = MELMapOpen(kMapNameFileNames[mapName]),
    };
    return self;
}

static void init(MELScene * _Nonnull scene) {
    LessonSelectScene *self = (LessonSelectScene *)scene;
    MELMap *map = self->map;
    for (unsigned int index = 0; index < map->layerCount; index++) {
        MELLayer *layer = map->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);
        LCDSpriteRefListPush(&self->super.sprites, LayerSpriteConstructor(layer, image));
    }

    MELSpriteInstanceList instances = map->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette == NULL) {
            definition->palette = SpriteNameLoadBitmapTable(instance.name);
        }
        MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
        LCDSprite *sprite = MELSpriteInit(melSprite, definition, instances.memory + index);
        playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
        playdate->sprite->setZIndex(sprite, ZINDEX_PORTAITS);
        switch (definition->name) {
            case SpriteNameLessonSelectTitle:
                setTitleLanguage(sprite);
                break;
            default:
                break;
        }
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

    createMenuGrid(self);

    MELEventBusAddListener(EventLanguageChanged, onLanguageChanged, self);

    MusicManagerPlay("musics/menu", 0, 0.25f);

    camera.frame.origin = MELPointZero;
    setRefreshRate(50);
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeLessonSelect) {
        playdate->system->error("Given scene is not a LessonSelect instance");
        return;
    }
    LessonSelectScene *self = (LessonSelectScene *)scene;

    MELEventBusRemoveListeners(self);

    LCDSpriteRefListDeallocReverse(&self->super.sprites);

    MELSpriteInstanceList instances = self->map->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette != NULL) {
            playdate->graphics->freeBitmapTable(definition->palette);
            definition->palette = NULL;
        }
    }

    MELMapDealloc(self->map);
    playdate->system->realloc(self, 0);
}

#pragma mark - Grille

static void createMenuGrid(LessonSelectScene * _Nonnull self) {
    const int left = 24;
    const int top = 32;
    const int bottom = 20;
    const int cellWidth = 216;
    const int cellHeight = playdate->graphics->getFontHeight(currentFont) + (currentLanguage == MELLanguageJapanese ? 4 : 0);
    const float height = LCD_ROWS - top - bottom;

    const uint16_t victories = currentSaveGame.wins;
    int unlockedLessonCount = 0;
    for (unsigned int index = 0; index < kLessons.count; index++) {
        if (victories >= kLessons.memory[index].objectives.victoriesToUnlock) {
            unlockedLessonCount++;
        }
    }

    LCDSprite *sprite = MELGridViewMake((MELRectangle) {
        .origin = {
            .x = left + cellWidth / 2,
            .y = top + height / 2
        },
        .size = {
            .width = cellWidth,
            .height = height
        }
    }, MELIntSizeMake(1, unlockedLessonCount), MELIntSizeMake(cellWidth, cellHeight), drawCell, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_PORTAITS);

    MELGridView *gridView = playdate->sprite->getUserdata(sprite);
    gridView->cellSpacing = MELIntSizeMake(0, 16);
    gridView->scrollingSpeed = 400.0f;
    gridView->inset.top = 8;
    self->gridView = gridView;
    self->gridViewSprite = sprite;
    LCDSpriteRefListPush(&self->super.sprites, sprite);
}

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected) {
    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->setDrawMode(kDrawModeCopy);
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);

    const uint8_t fontHeight = playdate->graphics->getFontHeight(currentFont);

    const int left = spriteMenuCursor.size.width + 2;
    Lesson *lesson = kLessons.memory + y;
    const char *text = MELLocalizedStringGetText(lesson->title);

    graphics->setDrawMode(kDrawModeInverted);
    if (isSelected) {
        loadSpriteMenuCursorPalette();
        const int top = (fontHeight - spriteMenuCursor.size.height) / 2;
        graphics->drawBitmap(graphics->getTableBitmap(spriteMenuCursor.palette, 0), 0, top, kBitmapUnflipped);

        const int width = graphics->getTextWidth(currentFont, text, 99, kUTF8Encoding, 0);
        graphics->drawLine(spriteMenuCursor.size.width + 4, gridView->cellSize.height - 1, spriteMenuCursor.size.width + width, gridView->cellSize.height - 1, 2, kColorWhite);
    }

    playdate->graphics->setFont(currentFont);
    playdate->graphics->drawText(text, 99, kUTF8Encoding, left, 0);

    const uint16_t victories = currentSaveGame.wins;
    const uint16_t victoriesToUnlock = lesson->objectives.victoriesToUnlock;

    if (isLessonDone(y)) {
        // Coche pour indiquer que la leçon est terminée.
        loadSpriteCheckmarkPalette();
        LCDBitmap *checkmark = graphics->getTableBitmap(spriteCheckmark.palette, 0);
        int width;
        graphics->getBitmapData(image, &width, NULL, NULL, NULL, NULL);
        graphics->drawBitmap(checkmark, width - spriteCheckmark.size.width, (fontHeight - spriteCheckmark.size.height) / 2, kBitmapUnflipped);
    } else if (victoriesToUnlock > 0 && victories >= victoriesToUnlock) {
        loadSpriteStoryClearPalette();
        MELAnimationDefinition *animation = MELSpriteDefinitionGetAnimationDefinition(spriteStoryClear, 1, MELAnimationDirectionRight);
        LCDBitmap *icon = graphics->getTableBitmap(spriteStoryClear.palette, animation->frames[0].atlasIndex);
        const int x = gridView->cellSize.width - spriteStoryClear.size.width;
        graphics->setDrawMode(kDrawModeCopy);
        graphics->drawBitmap(icon, x, (gridView->cellSize.height - spriteStoryClear.size.height) / 2, kBitmapUnflipped);
    }

    graphics->popContext();
}

#pragma mark - Changement de langue

static void setTitleLanguage(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELAnimationDefinition *definition = MELSpriteDefinitionGetAnimationDefinition(self->definition, AnimationNameStand, (MELAnimationDirection) currentLanguage);
    if (definition == NULL) {
        return;
    }
    MELAnimation *currentAnimation = self->animation;
    if (currentAnimation) {
        MELAnimationDealloc(currentAnimation);
    }
    MELAnimation *animation = MELAnimationAlloc(definition);
    animation->class->start(animation);
    self->animation = animation;
}

static void onLanguageChanged(void * _Nonnull userdata, int language) {
    LessonSelectScene *self = userdata;
    if (self->gridViewSprite) {
        MELGridViewRepaint(self->gridViewSprite);
    }
    LCDSprite *title = MELSceneFindSpriteByName(SpriteNameLessonSelectTitle);
    if (title) {
        setTitleLanguage(title);
    }
}

#pragma mark - Update

static int update(void * _Nonnull userdata) {
    LessonSelectScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;
    playdate->sprite->updateAndDrawSprites();

    PDButtons pressed, pressing;
    playdate->system->getButtonState(&pressing, &pressed, NULL);
    if (pressed & kButtonA) {
        DialogScene *dialogScene = DialogSceneAllocWithLesson(kLessons.memory + self->gridView->selection.y);
        MELSceneMakeCurrent(&dialogScene->super);
        return true;
    } else if (pressed & kButtonB) {
        TitleScene *titleScene = TitleSceneAlloc();
        titleScene->initialMenuSelection = TitleMenuItemSchool;
        MELSceneMakeCurrent(&titleScene->super);
        return true;
    }

    return true;
}

