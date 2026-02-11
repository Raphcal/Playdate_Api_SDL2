//
//  rallyscorescene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 10/07/2025.
//

#include "rallyscorescene.h"

#include "music.h"
#include "titlescene.h"
#include "scoreattackselectscene.h"
#include "rallyintroscene.h"
#include "rallyscene.h"
#include "rallyresultsscene.h"
#include "text.h"
#include "image.h"
#include "scores.h"
#include "scoreboard.h"
#include "recentscores.h"
#include "stride.h"
#include "scrollbar.h"
#include "../gen/spritensselect8x66x5.h"
#include "../gen/spriteactivityindicator.h"
#include "../gen/spritemenuitems.h"

static const float kMaximumScrollSpeed = 400.0f;
static const float kScrollAcceleration = 600.0f;

static const int kTitleMarginBottom = 5;
static const int kPadding = 10;

typedef enum {
    TagTitle = 1,
    TagHowToPlay,
    TagStartButton,
    TagLastResults,
    TagLeaderboad,
    TagLeaderboardSprite,
} Tag;

static const MELLocalizedString kHowToPlay = (MELLocalizedString) {
    .text = {
        "Beat all 20 opponents in a row!",
        "Battez les 20 adversaires à la suite !",
        "全20キャラと対戦する。",
        "Derrote todos os 20 adversários em uma fila!",
    }
};

static const MELLocalizedString kStart = (MELLocalizedString) {
    .text = {
        "Start",
        "Commencer",
        "スタート",
        "Iniciar",
    }
};
static const MELLocalizedString kResumeOrRestart[2] = {
    {
        .text = {
            "Resume",
            "Reprendre",
            "つづきから",
        }
    },
    {
        .text = {
            "Restart",
            "Recommencer",
            "はじめから",
        }
    },
};
static const MELLocalizedString kDescription = {
    .text = {
        "Lvl %d  Score %u",
        "Nv %d  Score %u",
        "レベル%d  スコア%u",
    }
};

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

static LCDSprite * _Nonnull createLevelSelectGrid(RallyScoreScene * _Nonnull self, int top);
static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected);
static LCDBitmap * _Nonnull createStartButton(RallyScoreScene * _Nonnull self);

static void onLanguageChanged(void * _Nonnull userdata, int language);

RallyScoreScene * _Nonnull RallyScoreSceneAlloc(void) {
    RallyScoreScene *self = new(RallyScoreScene);
    loadSpriteNsSelect8x66x5Palette();
    *self = (RallyScoreScene) {
        .super = {
            .type = SceneTypeRallyScores,
            .init = init,
            .dealloc = dealloc,
            .update = update,
        },
        .nineSlice = MELNineSliceMakeWithBitmapTable(spriteNsSelect8x66x5.palette, 0, (MELIntRectangle) {
            .origin = { .x = 8, .y = 6 },
            .size = { .width = 4, .height = 5 }
        }),
    };
    return self;
}

static void init(MELScene * _Nonnull scene) {
    RallyScoreScene * _Nonnull self = (RallyScoreScene *)scene;

    // Chargement des scores
    OnlineScoresAddRallyScore(0);

    int top = 10;
    LCDSprite *sprite;

    loadSpriteMenuItemsPalette();
    MELAnimationDefinition *animation = MELSpriteDefinitionGetAnimationDefinition(spriteMenuItems, TitleMenuItemArcade, (MELAnimationDirection) currentLanguage);
    if (animation != NULL) {
        LCDBitmap *label = playdate->graphics->getTableBitmap(spriteMenuItems.palette, animation->frames[0].atlasIndex);
        sprite = ImageConstructor(MELPointZero, label);

        MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop, MELPointMake(10.0f, top));
        playdate->sprite->setTag(sprite, TagTitle);
        top += MELSpriteGetHeight(sprite) + kTitleMarginBottom;
    }

    sprite = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kHowToPlay), 99);
    MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop, MELPointMake(10.0f, top));
    playdate->sprite->setTag(sprite, TagHowToPlay);
    top += MELSpriteGetHeight(sprite) + kTitleMarginBottom;

    if (currentSaveGame.arcadeSaveState.game.stage) {
        sprite = createLevelSelectGrid(self, top);
    } else {
        sprite = ImageConstructor(MELPointZero, createStartButton(self));
        MELAlignmentSet(sprite, MELHorizontalAlignmentCenter, MELVerticalAlignmentTop, MELPointMake(LCD_COLUMNS / 2, top));
    }
    top += MELSpriteGetHeight(sprite) + kPadding;

    if (currentSaveGame.rallyRun > 1) {
        sprite = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kLastResults), 99);
        MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop, MELPointMake(10.0f, top));
        playdate->sprite->setTag(sprite, TagLastResults);
        top += MELSpriteGetHeight(sprite) + kTitleMarginBottom;

        static const int recentScoresHeight = 50;
        RecentScoresConstructor(MELRectangleMake(LCD_COLUMNS / 2, top + recentScoresHeight / 2, LCD_COLUMNS, recentScoresHeight), currentSaveGame.rallyLastScores, currentSaveGame.rallyRun);
        top += recentScoresHeight + kPadding;
    }

    sprite = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kLeaderboad), 99);
    MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop, MELPointMake(10.0f, top));
    playdate->sprite->setTag(sprite, TagLeaderboad);
    top += MELSpriteGetHeight(sprite) + kTitleMarginBottom;

    if (OnlineScoresAreLoading()) {
        self->loadingMessage = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kLoading), 99);
        const int textWidth = MELSpriteGetWidth(self->loadingMessage);

        loadSpriteActivityIndicatorPalette();
        MELSprite *activityIndicator = new(MELSprite);
        self->activityIndicator = MELSpriteInitWithCenter(activityIndicator, &spriteActivityIndicator, MELPointZero);
        playdate->sprite->setUpdateFunction(self->activityIndicator, MELSpriteUpdate);
        LCDSpriteRefListPush(&self->super.sprites, self->activityIndicator);

        const int groupWidth = textWidth + 5 + spriteActivityIndicator.size.width;
        MELAlignmentSet(self->loadingMessage, MELHorizontalAlignmentLeft, MELVerticalAlignmentMiddle, MELPointMake((LCD_COLUMNS - groupWidth) / 2, top + spriteActivityIndicator.size.height / 2));
        MELAlignmentSet(self->activityIndicator, MELHorizontalAlignmentRight, MELVerticalAlignmentMiddle, MELPointMake(LCD_COLUMNS / 2 + groupWidth / 2, top + spriteActivityIndicator.size.height / 2));

        top += spriteActivityIndicator.size.height + kTitleMarginBottom;
    } else {
        LocalScoresSave();
    }

    self->scoreboard = ScoreboardConstructor(&rallyScores, top);
    playdate->sprite->setTag(self->scoreboard, TagLeaderboardSprite);

    MELSprite *scoreboard = playdate->sprite->getUserdata(self->scoreboard);
    float bottom = MELFloatMax(MELRectangleOriginIsCenterGetBottom(scoreboard->frame), LCD_ROWS);
    if (rallyScores.count == 0) {
        bottom -= kTitleMarginBottom;
    }

    self->scrollbar = ScrollBarConstructor(0, bottom);

    MELEventBusAddListener(EventLanguageChanged, onLanguageChanged, self);

    MusicManagerPlay("musics/menu", 0, 0.25f);

    camera.frame.origin = MELPointZero;
    setRefreshRate(DEFAULT_REFRESH_RATE);
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeRallyScores) {
        playdate->system->error("Given scene is not a RallyScoreScene instance");
        return;
    }
    RallyScoreScene *self = (RallyScoreScene *)scene;

    if (self->nineSlice) {
        MELNineSliceDealloc(self->nineSlice);
        self->nineSlice = NULL;
    }

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);

    playdate->system->realloc(self, 0);
}

static int update(void * _Nonnull userdata) {
    RallyScoreScene * _Nonnull self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;

    // Déplace et affiche les sprites.
    playdate->sprite->updateAndDrawSprites();

    // Cache l'activité de chargement après chargement des sprites
    if (!OnlineScoresRallyIsLoading() && self->activityIndicator) {
        LocalScoresSave();

        MELSprite *activityIndicator = playdate->sprite->getUserdata(self->activityIndicator);
        playdate->sprite->setUpdateFunction(self->activityIndicator, activityIndicator->class->destroy);
        self->activityIndicator = NULL;
       
        MELSprite *loadingMessage = playdate->sprite->getUserdata(self->loadingMessage);
        if (rallyLastUpdateError) {
            if (!fontNontendoBold) {
                fontNontendoBold = LCDFontLoadOrError("fonts/Nontendo-Bold");
            }
            TextSet(self->loadingMessage, fontNontendoBold, kDrawModeCopy, rallyLastUpdateError, (int) strlen(rallyLastUpdateError));
            loadingMessage->frame.origin.x = LCD_COLUMNS / 2;
        } else if (rallyScores.count == 0) {
            if (!fontNontendoBold) {
                fontNontendoBold = LCDFontLoadOrError("fonts/Nontendo-Bold");
            }
            TextSet(self->loadingMessage, fontNontendoBold, kDrawModeCopy, MELLocalizedStringGetText(kEmpty), 99);
            loadingMessage->frame.origin.x = LCD_COLUMNS / 2;
        } else {
            ScoreboardSetTop(self->scoreboard, ScoreboardGetTop(self->scoreboard) - spriteActivityIndicator.size.height - kTitleMarginBottom);

            playdate->sprite->setUpdateFunction(self->loadingMessage, loadingMessage->class->destroy);
            self->loadingMessage = NULL;
        }

        MELSprite *scoreboard = playdate->sprite->getUserdata(self->scoreboard);
        const float bottom = MELFloatMax(MELRectangleOriginIsCenterGetBottom(scoreboard->frame), LCD_ROWS);
        ScrollBarSetHeight(self->scrollbar, bottom);
    }

    const MELController controller = MELControllerMake();
    if (controller.pressedA) {
        MELScene *nextScene = NULL;
        if (currentSaveGame.arcadeSaveState.game.stage && self->gridView->selection.x == 0) {
            const ArcadeSaveState saveState = currentSaveGame.arcadeSaveState;
            const ArcadeGame game = saveState.game;
            // NOTE: Pas sûr de garder la reprise en plein jeu !!
            if (saveState.leftBoard) {
                RallyScene *rallyScene = RallySceneAllocWithSaveState(saveState);
                nextScene = &rallyScene->super;
            } else {
                RallyIntroScene *rallyIntroScene = RallyIntroSceneAlloc(game);
                nextScene = &rallyIntroScene->super;
            }
        } else {
            RallyIntroScene *rallyIntroScene = RallyIntroSceneAlloc((ArcadeGame) {
                .speed = 1.0f
            });
            nextScene = &rallyIntroScene->super;
        }
        currentSaveGame.arcadeSaveState = (ArcadeSaveState) {};
        MELSceneMakeCurrent(nextScene);
        return true;
    }
    if (controller.pressedB) {
        TitleScene *titleScene = TitleSceneAlloc();
        titleScene->initialMenuSelection = TitleMenuItemArcade;
        MELSceneMakeCurrent(&titleScene->super);
        return true;
    }
#if DEBUG
    if (controller.axe.x > 0) {
        RallyResultsScene *resultsScene = RallyResultsSceneAlloc((ArcadeGame) {
            .time = 632.15f,
            .stage = 7,
            .speed = 1.5f,
            .score = 12345,
            .deathCount = 2,
            .winStreak = 5,
        }, true);
        MELSceneMakeCurrent(&resultsScene->super);
        return true;
    }
#endif

    // Gestion du scrolling
    if (controller.axe.y < 0) {
        self->scrollSpeed = MELFloatMax(self->scrollSpeed - kScrollAcceleration * DELTA, -kMaximumScrollSpeed);
    }
    else if (controller.axe.y > 0) {
        self->scrollSpeed = MELFloatMin(self->scrollSpeed + kScrollAcceleration * DELTA, kMaximumScrollSpeed);
    } else {
        self->scrollSpeed = 0.0f;
    }
    const float originYPlusCrank = camera.frame.origin.y + MELCrankGetAcceleratedChange() + self->scrollSpeed * DELTA;

    MELSprite *scoreboard = playdate->sprite->getUserdata(self->scoreboard);
    const float bottom = MELFloatMax(MELRectangleOriginIsCenterGetBottom(scoreboard->frame) - LCD_ROWS + kPadding, 0);
    camera.frame.origin.y = MELFloatBound(0.0f, originYPlusCrank, bottom);

    return true;
}

static void drawCentered(const char * _Nonnull text, LCDFont * _Nonnull font, int width, int y) {
    const struct playdate_graphics *gfx = playdate->graphics;
    const int textWidth = gfx->getTextWidth(font, text, 200, kUTF8Encoding, 0);
    gfx->setFont(font);
    gfx->drawText(text, 200, kUTF8Encoding, (width - textWidth) / 2, y);
}

#pragma mark - Grille

static LCDSprite * _Nonnull createLevelSelectGrid(RallyScoreScene * _Nonnull self, int top) {
    const int left = 10;
    const int cellWidth = (LCD_COLUMNS - left - left) / 2;
    const int cellHeight = 48;
    LCDSprite *sprite = MELGridViewMake((MELRectangle) {
        .origin = {
            .x = LCD_COLUMNS / 2,
            .y = top + cellHeight / 2
        },
        .size = {
            .width = LCD_COLUMNS - left - left,
            .height = cellHeight
        }
    }, MELIntSizeMake(2, 1), MELIntSizeMake(cellWidth, cellHeight), drawCell, self);
    playdate->sprite->setZIndex(sprite, 1);

    MELGridView *gridView = playdate->sprite->getUserdata(sprite);
    gridView->enableScrolling = false;
    self->gridView = gridView;
    self->gridViewSprite = sprite;
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    return sprite;
}

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected) {
    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->setDrawMode(kDrawModeCopy);
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);

    const MELSize size = (MELSize) {
        .width = gridView->cellSize.width,
        .height = gridView->cellSize.height
    };

    ScoreAttackSelectScene *self = gridView->userdata;
    if (isSelected) {
        MELNineSliceDrawInRect(self->nineSlice, (MELRectangle) {
            .size = size,
        });
    }
    drawCentered(MELLocalizedStringGetText(kResumeOrRestart[x]), currentFont, size.width, 6);
    if (x == 0) {
        char *text = NULL;
        playdate->system->formatString(&text, MELLocalizedStringGetText(kDescription), currentSaveGame.arcadeSaveState.game.stage + 1, currentSaveGame.arcadeSaveState.game.score);
        drawCentered(text, smallFont, size.width, 26 - (currentLanguage == MELLanguageJapanese) * 2);
    }

    graphics->popContext();
}

#pragma mark - Bouton

static LCDBitmap * _Nonnull createStartButton(RallyScoreScene * _Nonnull self) {
    const struct playdate_graphics *graphics = playdate->graphics;
    const int left = 10;
    const int cellWidth = LCD_COLUMNS - left - left;
    const int cellHeight = 48;
    

    LCDBitmap *image = graphics->newBitmap(cellWidth, cellHeight, kColorWhite);

    graphics->setDrawMode(kDrawModeCopy);
    graphics->pushContext(image);

    MELNineSliceDrawInRect(self->nineSlice, (MELRectangle) {
        .size = {
            .width = cellWidth,
            .height = cellHeight
        },
    });

    drawCentered(MELLocalizedStringGetText(kStart), currentFont, cellWidth, 12);

    graphics->popContext();
    return image;
}

#pragma mark - Changement de langue

static void alignLeft(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentMiddle, MELPointMake(10.0f, self->frame.origin.y));
}

static void onLanguageChanged(void * _Nonnull userdata, int language) {
    RallyScoreScene *self = userdata;
    if (self->gridViewSprite) {
        MELGridViewRepaint(self->gridViewSprite);
    }
    if (self->loadingMessage && self->activityIndicator) {
        TextSet(self->loadingMessage, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kLoading), 99);
        const MELRectangle frame = MELSpriteGetFrame(self->loadingMessage);

        const int groupWidth = frame.size.width + 5 + spriteActivityIndicator.size.width;
        MELAlignmentSet(self->loadingMessage, MELHorizontalAlignmentLeft, MELVerticalAlignmentMiddle, MELPointMake((LCD_COLUMNS - groupWidth) / 2, frame.origin.y));
        MELAlignmentSet(self->activityIndicator, MELHorizontalAlignmentRight, MELVerticalAlignmentMiddle, MELPointMake(LCD_COLUMNS / 2 + groupWidth / 2, frame.origin.y));
    }
    const LCDSpriteRefList sprites = self->super.sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        switch (playdate->sprite->getTag(sprite)) {
            case TagTitle:
                loadSpriteMenuItemsPalette();
                MELAnimationDefinition *animation = MELSpriteDefinitionGetAnimationDefinition(spriteMenuItems, TitleMenuItemArcade, (MELAnimationDirection) currentLanguage);
                if (animation != NULL) {
                    LCDBitmap *label = playdate->graphics->getTableBitmap(spriteMenuItems.palette, animation->frames[0].atlasIndex);
                    playdate->sprite->setImage(sprite, label, kBitmapUnflipped);
                }
                break;
            case TagHowToPlay:
                TextSet(sprite, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kHowToPlay), 99);
                alignLeft(sprite);
                break;
            case TagStartButton:
                playdate->graphics->freeBitmap(playdate->sprite->getImage(sprite));
                playdate->sprite->setImage(sprite, createStartButton(self), kBitmapUnflipped);
                alignLeft(sprite);
                break;
            case TagLastResults:
                TextSet(sprite, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kLastResults), 99);
                alignLeft(sprite);
                break;
            case TagLeaderboad:
                TextSet(sprite, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kLeaderboad), 99);
                alignLeft(sprite);
                break;
            case TagLeaderboardSprite:
                ScoreboardRepaint(sprite);
                break;
            default:
                break;
        }
    }
}
