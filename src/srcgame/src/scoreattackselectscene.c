//
//  scoreattackselectscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 10/11/2024.
//

#include "scoreattackselectscene.h"

#include "music.h"
#include "titlescene.h"
#include "scoreattackscene.h"
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
    TagSelectDifficulty,
    TagLastResults,
    TagLeaderboad,
    TagLeaderboardSprite,
} Tag;

static const MELLocalizedString kSelectDifficulty = (MELLocalizedString) {
    .text = {
        "Select level to start from",
        "Choisissez le niveau de départ",
        "スタートレベル選択",
        "Selecione o nível inicial",
    }
};

const MELLocalizedString kLastResults = (MELLocalizedString) {
    .text = {
        "Your latest scores",
        "Vos derniers scores",
        "あなたのスコア",
        "Seus placares recentes",
    }
};

const MELLocalizedString kLeaderboad = (MELLocalizedString) {
    .text = {
        "High scores",
        "Classement des meilleurs scores",
        "ランキング",
        "Melhores placares",
    }
};

static const MELLocalizedString kDifficulties[ScoreAttackDifficultyCount] = {
    (MELLocalizedString) {
        .text = {
            "Level 1",
            "Niveau 1",
            "レベル１",
            "Nível 1",
        }
    },
    {
        .text = {
            "Level 20",
            "Niveau 20",
            "レベル20",
            "Nível 20",
        }
    },
    {
        .text = {
            "Level 40",
            "Niveau 40",
            "レベル40",
            "Nível 40",
        }
    }
};
static const MELLocalizedString kDescription[ScoreAttackDifficultyCount] = {
    (MELLocalizedString) {
        .text = {
            "",
            "",
            ""
        }
    },
    {
        .text = {
            "Score +20,000",
            "Score +20 000",
            "スコア +2万",
            "+20.000 pontos",
        }
    },
    {
        .text = {
            "Score +50,000",
            "Score +50 000",
            "スコア +5万",
            "+50.000 pontos",
        }
    }
};

const MELLocalizedString kLoading = (MELLocalizedString) {
    .text = {
        "Loading scores...",
        "Chargement des scores...",
        "ランキングロード中",
        "Carregando placares...",
    }
};

const MELLocalizedString kEmpty = (MELLocalizedString) {
    .text = {
        "No score yet!",
        "Aucun score !",
        "スコアはまだありません！",
        "Ainda não há pontuação!",
    }
};

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

static LCDSprite * _Nonnull createLevelSelectGrid(ScoreAttackSelectScene * _Nonnull self, int top);
static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected);

static void onLanguageChanged(void * _Nonnull userdata, int language);

ScoreAttackSelectScene * _Nonnull ScoreAttackSelectSceneAlloc(void) {
    ScoreAttackSelectScene *self = new(ScoreAttackSelectScene);
    loadSpriteNsSelect8x66x5Palette();
    *self = (ScoreAttackSelectScene) {
        .super = {
            .type = SceneTypeScoreAttackSelect,
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
    ScoreAttackSelectScene * _Nonnull self = (ScoreAttackSelectScene *)scene;

    // Chargement des scores
    OnlineScoresAddScoreAttackScore(0);

    int top = 10;
    LCDSprite *sprite;

    loadSpriteMenuItemsPalette();
    MELAnimationDefinition *animation = MELSpriteDefinitionGetAnimationDefinition(spriteMenuItems, TitleMenuItemScoreAttack, (MELAnimationDirection) currentLanguage);
    if (animation != NULL) {
        LCDBitmap *label = playdate->graphics->getTableBitmap(spriteMenuItems.palette, animation->frames[0].atlasIndex);
        sprite = ImageConstructor(MELPointZero, label);

        MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop, MELPointMake(10.0f, top));
        playdate->sprite->setTag(sprite, TagTitle);
        top += MELSpriteGetHeight(sprite) + kTitleMarginBottom;
    }

    sprite = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kSelectDifficulty), 99);
    MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop, MELPointMake(10.0f, top));
    playdate->sprite->setTag(sprite, TagSelectDifficulty);
    top += MELSpriteGetHeight(sprite) + kTitleMarginBottom;

    sprite = createLevelSelectGrid(self, top);
    top += MELSpriteGetHeight(sprite) + kPadding;

    if (currentSaveGame.scoreAttackRun > 1) {
        sprite = TextConstructor(MELPointZero, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kLastResults), 99);
        MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop, MELPointMake(10.0f, top));
        playdate->sprite->setTag(sprite, TagLastResults);
        top += MELSpriteGetHeight(sprite) + kTitleMarginBottom;

        static const int recentScoresHeight = 50;
        RecentScoresConstructor(MELRectangleMake(LCD_COLUMNS / 2, top + recentScoresHeight / 2, LCD_COLUMNS, recentScoresHeight), currentSaveGame.scoreAttackLastScores, currentSaveGame.scoreAttackRun);
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

    self->scoreboard = ScoreboardConstructor(&scoreAttackScores, top);
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
    if (scene->type != SceneTypeScoreAttackSelect) {
        playdate->system->error("Given scene is not a ScoreAttackSelectScene instance");
        return;
    }
    ScoreAttackSelectScene *self = (ScoreAttackSelectScene *)scene;

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
    ScoreAttackSelectScene * _Nonnull self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;

    // Déplace et affiche les sprites.
    playdate->sprite->updateAndDrawSprites();

    // Cache l'activité de chargement après chargement des sprites
    if (!OnlineScoresScoreAttackIsLoading() && self->activityIndicator) {
        LocalScoresSave();

        MELSprite *activityIndicator = playdate->sprite->getUserdata(self->activityIndicator);
        playdate->sprite->setUpdateFunction(self->activityIndicator, activityIndicator->class->destroy);
        self->activityIndicator = NULL;
       
        MELSprite *loadingMessage = playdate->sprite->getUserdata(self->loadingMessage);
        if (scoreAttackLastUpdateError) {
            if (!fontNontendoBold) {
                fontNontendoBold = LCDFontLoadOrError("fonts/Nontendo-Bold");
            }
            TextSet(self->loadingMessage, fontNontendoBold, kDrawModeCopy, scoreAttackLastUpdateError, (int) strlen(scoreAttackLastUpdateError));
            loadingMessage->frame.origin.x = LCD_COLUMNS / 2;
        } else if (scoreAttackScores.count == 0) {
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
        ScoreAttackScene *scoreAttackScene = ScoreAttackSceneAlloc(self->gridView->selection.x);
        MELSceneMakeCurrent(&scoreAttackScene->super);
        return true;
    }
    if (controller.pressedB) {
        TitleScene *titleScene = TitleSceneAlloc();
        titleScene->initialMenuSelection = TitleMenuItemScoreAttack;
        MELSceneMakeCurrent(&titleScene->super);
        return true;
    }

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

#pragma mark - Grille

static LCDSprite * _Nonnull createLevelSelectGrid(ScoreAttackSelectScene * _Nonnull self, int top) {
    const int left = 10;
    const int cellWidth = (LCD_COLUMNS - left - left) / 3;
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
    }, MELIntSizeMake(ScoreAttackDifficultyCount, 1), MELIntSizeMake(cellWidth, cellHeight), drawCell, self);
    playdate->sprite->setZIndex(sprite, 1);

    MELGridView *gridView = playdate->sprite->getUserdata(sprite);
    gridView->enableScrolling = false;
    self->gridView = gridView;
    self->gridViewSprite = sprite;
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    return sprite;
}

static void drawCentered(const char * _Nonnull text, LCDFont * _Nonnull font, int width, int y) {
    const struct playdate_graphics *gfx = playdate->graphics;
    const int textWidth = gfx->getTextWidth(font, text, 200, kUTF8Encoding, 0);
    gfx->setFont(font);
    gfx->drawText(text, 200, kUTF8Encoding, (width - textWidth) / 2, y);
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
    drawCentered(MELLocalizedStringGetText(kDifficulties[x]), currentFont, size.width, 6);
    drawCentered(MELLocalizedStringGetText(kDescription[x]), smallFont, size.width, 26 - (currentLanguage == MELLanguageJapanese) * 2);

    graphics->popContext();
}

#pragma mark - Changement de langue

static void alignLeft(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELAlignmentSet(sprite, MELHorizontalAlignmentLeft, MELVerticalAlignmentMiddle, MELPointMake(10.0f, self->frame.origin.y));
}

static void onLanguageChanged(void * _Nonnull userdata, int language) {
    ScoreAttackSelectScene *self = userdata;
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
                MELAnimationDefinition *animation = MELSpriteDefinitionGetAnimationDefinition(spriteMenuItems, TitleMenuItemScoreAttack, (MELAnimationDirection) currentLanguage);
                if (animation != NULL) {
                    LCDBitmap *label = playdate->graphics->getTableBitmap(spriteMenuItems.palette, animation->frames[0].atlasIndex);
                    playdate->sprite->setImage(sprite, label, kBitmapUnflipped);
                }
                break;
            case TagSelectDifficulty:
                TextSet(sprite, smallFont, kDrawModeCopy, MELLocalizedStringGetText(kSelectDifficulty), 99);
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
