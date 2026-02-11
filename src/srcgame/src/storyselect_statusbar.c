//
//  storyselect_statusbar.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 03/09/2024.
//

#include "storyselect_statusbar.h"

#include "image.h"
#include "text.h"
#include "story.h"

typedef struct {
    MELSprite super;
    MELGridView * _Nullable gridView;
    int32_t oldSelection;
    LCDSprite * _Nullable characterName;
    LCDSprite * _Nullable difficulty;
} StorySelectStatusBar;

static const int height = 32;
static const int horizontalPadding = 40;

static const MELLocalizedString DifficultyEasy = (MELLocalizedString) {
    .text = {
        "Easy",
        "Facile",
        "簡単",
        "Fácil",
    }
};
static const MELLocalizedString DifficultyNormal = (MELLocalizedString) {
    .text = {
        "Normal",
        "Normal",
        "通常",
        "Normal",
    }
};
static const MELLocalizedString DifficultyHard = (MELLocalizedString) {
    .text = {
        "Hard",
        "Difficile",
        "難しい",
        "Difícil",
    }
};
static const MELLocalizedString DifficultyVeryHard = (MELLocalizedString) {
    .text = {
        "Very Hard",
        "Très Difficile",
        "超難しい",
        "Muito Difícil",
    }
};
static const MELLocalizedString CharacterIsLocked = (MELLocalizedString) {
    .text = {
        "Locked",
        "Verrouillé",
        "未解放",
        "Bloqueado",
    }
};

static void dealloc(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void onLanguageChanged(void * _Nullable userdata, int language);

static const MELSpriteClass StorySelectStatusBarClass = (MELSpriteClass) {
    .destroy = dealloc,
};

LCDSprite * _Nonnull StorySelectStatusBarConstructor(MELGridView * _Nonnull gridView) {
    const struct playdate_sprite *spriteAPI = playdate->sprite;

    StorySelectStatusBar *self = new(StorySelectStatusBar);
    LCDSprite *sprite = spriteAPI->newSprite();

    const MELRectangle frame = (MELRectangle) {
        .origin = {
            .x = LCD_COLUMNS / 2,
            .y = LCD_ROWS - height / 2
        },
        .size = {
            .width = LCD_COLUMNS,
            .height = height
        }
    };
    spriteAPI->moveTo(sprite, frame.origin.x, frame.origin.y);

    const MELLocalizedString name = CharacterName[CharacterCaroline];
    *self = (StorySelectStatusBar) {
        .super = {
            .class = &StorySelectStatusBarClass,
            .frame = frame,
        },
        .gridView = gridView,
        .characterName = TextConstructor(MELPointZero, currentFont, kDrawModeCopy, MELLocalizedStringGetText(name), 99),
        .difficulty = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, MELLocalizedStringGetText(DifficultyNormal), 99),
        .oldSelection = 1,
    };

    MELAlignmentSet(self->characterName, MELHorizontalAlignmentLeft, MELVerticalAlignmentMiddle, MELPointMake(horizontalPadding, frame.origin.y));
    MELAlignmentSet(self->difficulty, MELHorizontalAlignmentRight, MELVerticalAlignmentMiddle, MELPointMake(LCD_COLUMNS - horizontalPadding, frame.origin.y));

    spriteAPI->setZIndex(sprite, 5);
    spriteAPI->setZIndex(self->characterName, 6);
    spriteAPI->setZIndex(self->difficulty, 6);

    LCDBitmap *image = playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorWhite);
    const struct playdate_graphics *gfx = playdate->graphics;
    gfx->pushContext(image);
    gfx->drawLine(0, 0, LCD_COLUMNS, 0, 1, kColorBlack);
    gfx->popContext();

    spriteAPI->setImage(sprite, image, kBitmapUnflipped);

    spriteAPI->setUserdata(sprite, self);
    spriteAPI->setUpdateFunction(sprite, update);
    spriteAPI->addSprite(sprite);
    LCDSpriteRefListPush(&currentScene->sprites, sprite);

    MELEventBusAddListener(EventLanguageChanged, onLanguageChanged, self);
    return sprite;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    StorySelectStatusBar *self = playdate->sprite->getUserdata(sprite);
    MELEventBusRemoveListeners(self);
    ImageDealloc(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    StorySelectStatusBar *self = playdate->sprite->getUserdata(sprite);
    MELGridView *gridView = self->gridView;
    const int oldSelection = self->oldSelection;
    const int selection = gridView->selection.x;
    if (selection == oldSelection) {
        return;
    }
    self->oldSelection = selection;

    if (StoryIsUnlocked(selection)) {
        const Character character = kStories[selection]->character;
        const MELLocalizedString name = CharacterName[character];
        TextSet(self->characterName, currentFont, kDrawModeCopy, MELLocalizedStringGetText(name), 99);
        switch (selection) {
            case 0:
                TextSet(self->difficulty, smallFont, kDrawModeCopy, MELLocalizedStringGetText(DifficultyEasy), 99);
                break;
            case 1:
                TextSet(self->difficulty, smallFont, kDrawModeCopy, MELLocalizedStringGetText(DifficultyNormal), 99);
                break;
            case 2:
            case 3:
                TextSet(self->difficulty, smallFont, kDrawModeCopy, MELLocalizedStringGetText(DifficultyHard), 99);
                break;
            default:
                TextSet(self->difficulty, smallFont, kDrawModeCopy, MELLocalizedStringGetText(DifficultyVeryHard), 99);
                break;
        }
    } else {
        TextSet(self->characterName, currentFont, kDrawModeCopy, MELLocalizedStringGetText(CharacterIsLocked), 99);
        TextSet(self->difficulty, smallFont, kDrawModeCopy, "", 0);
    }
    const float y = self->super.frame.origin.y;
    MELAlignmentSet(self->characterName, MELHorizontalAlignmentLeft, MELVerticalAlignmentMiddle, MELPointMake(horizontalPadding, y));
    MELAlignmentSet(self->difficulty, MELHorizontalAlignmentRight, MELVerticalAlignmentMiddle, MELPointMake(LCD_COLUMNS - horizontalPadding, y));
}

static void onLanguageChanged(void * _Nullable userdata, int language) {
    StorySelectStatusBar *self = userdata;
    // Invalide la sélection actuelle pour forcer une mise à jour.
    self->oldSelection = -1;
}
