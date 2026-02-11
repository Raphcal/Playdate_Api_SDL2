//
//  titlescene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 06/03/2023.
//

#include "titlescene.h"

#include "characterselectscene.h"
#include "camera.h"
#include "rankingboard.h"
#include "rankingitem.h"
#include "rankingsectionheader.h"
#include "text.h"
#include "transitionout.h"
#include "samples.h"

#include "../gen/palettenames.h"
#include "../gen/spritenames.h"
#include "../gen/spritevictorytransition.h"

static const float kMaximumScrollSpeed = 400.0f;
static const float kScrollAcceleration = 600.0f;

static const int kRankingsTop = LCD_ROWS + kRankingSectionHeaderMargin;

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull self);
static int update(void * _Nonnull userdata);

static void clearScores(TitleScene * _Nonnull self);
static void drawScores(TitleScene * _Nonnull self);

static void updateRankingsTitle(LCDSprite * _Nonnull sprite);
static void updateCrankToTitle(LCDSprite * _Nonnull sprite);
static void updateWithScrollRate(LCDSprite * _Nonnull sprite);

static LCDSprite * _Nonnull createSprite(MELSpriteInstance instance) {
    MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
    definition->palette = SpriteNameLoadBitmapTable(instance.name);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->moveTo(sprite, instance.center.x, instance.center.y);
    playdate->sprite->setZIndex(sprite, instance.zIndex);

    MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
    *melSprite = (MELSprite) {
        .class = &MELSpriteClassDefault,
        .definition = *definition,
        .frame = (MELRectangle) {
            .origin = instance.center,
            .size = definition->size
        },
        .direction = instance.direction,
    };
    MELSpriteSetAnimation(melSprite, AnimationNameStand);

    playdate->sprite->setUserdata(sprite, melSprite);
    playdate->sprite->setUpdateFunction(sprite, &MELSpriteUpdate);
    playdate->sprite->addSprite(sprite);
    return sprite;
}

TitleScene * _Nonnull TitleSceneAlloc(void) {
    TitleScene *self = playdate->system->realloc(NULL, sizeof(TitleScene));
    *self = (TitleScene) {
        .super = {
            .type = SceneTypeTitle,
            .init = init,
            .dealloc = dealloc,
            .update = update,
            .sprites = LCDSpriteRefListEmpty,
        },
        .map = MELMapOpen(kMapNameFileNames[MapNameTitle])
    };
    return self;
}

static void init(MELScene * _Nonnull scene) {
    TitleScene *self = (TitleScene *)scene;
    MELMap *map = self->map;
    MELSpriteInstanceList instances = map->instances;
    MELSprite *melSprite;
    float logoLeft = 0.0f;
    for (unsigned int index = 0; index < instances.count; index++) {
        const MELSpriteInstance instance = instances.memory[index];
        LCDSprite *sprite = createSprite(instance);
        switch (instance.name) {
            case SpriteNameTitleRankings:
                self->rankingsMenuItem = playdate->sprite->getUserdata(sprite);
                playdate->sprite->setUpdateFunction(sprite, updateRankingsTitle);
                playdate->sprite->setZIndex(sprite, ZINDEX_GUI);
                break;
            case SpriteNameTitleBelt:
            case SpriteNameTitleWaist:
                playdate->sprite->setUpdateFunction(sprite, updateWithScrollRate);
                playdate->sprite->setZIndex(sprite, 1);
                break;
            case SpriteNameTitleLogo:
                melSprite = playdate->sprite->getUserdata(sprite);
                logoLeft = MELRectangleOriginIsCenterGetLeft(melSprite->frame);
                playdate->sprite->setZIndex(sprite, 2);
                break;
            case SpriteNameCrankToTitle:
                self->crankToTitle = sprite;
                playdate->sprite->setUpdateFunction(sprite, updateCrankToTitle);
                playdate->sprite->setVisible(sprite, false);
                break;
            default:
                // Pas de traitement.
                break;
        }
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        melSprite = playdate->sprite->getUserdata(sprite);
        playdate->system->logToConsole("Push TitleScene#init(%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }

#if ENABLE_ONLINE_SCOREBOARDS
    // Chargement des scores en ligne.
    self->scoreLastUpdateDate = scoreLastUpdateDate;
    OnlineScoresLoad();
#endif

    self->versionNumber = TextConstructor(MELPointMake(LCD_COLUMNS / 2, LCD_ROWS + 30), nontendoLightFont, kDrawModeCopy, "Version " VERSION_NUMBER, 99);

    drawScores(self);

    LCDSprite *startText = TextConstructor(MELPointZero, romajiFont, kDrawModeCopy, "\a start", 99);
    self->startText = startText;
    MELSprite *startTextSprite = playdate->sprite->getUserdata(startText);
    MELRectangle startTextFrame = startTextSprite->frame;
    startTextFrame.origin.x = logoLeft / 2.0f;
    MELRectangleOriginIsCenterSetBottom(&startTextFrame, LCD_ROWS - 16);
    startTextSprite->frame = startTextFrame;

    if (self->showTransition) {
        loadSpriteVictoryTransitionPalette();
        TransitionOutConstructor(&spriteVictoryTransition);
    }

    camera.frame.origin = MELPointZero;
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeTitle) {
        playdate->system->error("Given scene is not a TitleScene instance");
        return;
    }
    TitleScene *self = (TitleScene *)scene;
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->map);
    playdate->graphics->freeBitmapTable(self->palette);
    playdate->graphics->freeBitmap(self->faceBackground);
    playdate->graphics->freeBitmapTable(self->faces);
    playdate->system->realloc(self, 0);
}

static int update(void * _Nonnull userdata) {
    TitleScene *self = userdata;

    if (self->scrollToRankings) {
        const float duration = 2.0f;
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        self->time = newTime;
        const float progress = MELEaseInOut(0.0f, duration, newTime);
        const float to = LCD_ROWS;
        const float from = self->height;
        camera.frame.origin.y = from + (to - from) * progress;
        playdate->sprite->updateAndDrawSprites();
        if (newTime == duration) {
            playdate->sprite->setVisible(self->crankToTitle, true);
            self->scrollToRankings = false;
        }
        return true;
    }

    // Rafraichissement des scores
    if (self->scoreLastUpdateDate < scoreLastUpdateDate) {
        self->scoreLastUpdateDate = scoreLastUpdateDate;
        clearScores(self);
        drawScores(self);
    }

    playdate->sprite->updateAndDrawSprites();
#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    SampleLoadNextSample();

    const unsigned int time = playdate->system->getCurrentTimeMilliseconds();
    playdate->sprite->setVisible(self->startText, time % 1500 < 750);

    PDButtons pressed, pressing;
    playdate->system->getButtonState(&pressing, &pressed, NULL);
    if (pressed & kButtonA) {
        // Début du jeu
        CharacterSelectScene *characterSelectScene = CharacterSelectSceneAlloc();
        MELSceneMakeCurrent(&characterSelectScene->super);
        return true;
#if ENABLE_ONLINE_SCOREBOARDS
    } else if (pressed & kButtonB && !OnlineScoresAreLoading()) {
        // Rechargement des scores
        scoreLastUpdateDate = 0;
        OnlineScoresLoad();
#endif
    }

    if (self->showTransition && MELSceneFindSpriteByName(SpriteNameVictoryTransition) != NULL) {
        // Transition en cours, pas de scrolling.
        return true;
    }
    self->showTransition = false;

    // Gestion du scrolling
    if (pressing & kButtonUp) {
        self->scrollSpeed = MELFloatMax(self->scrollSpeed - kScrollAcceleration * DELTA, -kMaximumScrollSpeed);
    }
    else if (pressing & kButtonDown) {
        self->scrollSpeed = MELFloatMin(self->scrollSpeed + kScrollAcceleration * DELTA, kMaximumScrollSpeed);
    } else {
        self->scrollSpeed = 0.0f;
    }
    const float originYPlusCrank = camera.frame.origin.y + MELCrankGetAcceleratedChange() + self->scrollSpeed * DELTA;
    const float bottom = self->height - LCD_ROWS;
    camera.frame.origin.y = MELFloatBound(0.0f, originYPlusCrank, bottom);

    return true;
}

#pragma mark - Titre "rankings"

static void updateRankingsTitle(LCDSprite * _Nonnull sprite) {
    if (!playdate->sprite->isVisible(sprite)) {
        return;
    }
    MELSprite * _Nonnull self = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = self->frame;
    const float topMargin = 8.0f;
    if (camera.frame.origin.y > MELRectangleOriginIsCenterGetTop(frame) - topMargin) {
        playdate->sprite->moveTo(sprite, frame.origin.x, frame.size.height / 2 + topMargin);
    } else {
        playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y - camera.frame.origin.y);
    }
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, self->animation->frame.atlasIndex), kBitmapUnflipped);
}

#pragma mark - Affichage des scores

static void clearScores(TitleScene * _Nonnull self) {
    const MELSpriteClass *rankingItemClass = RankingItemGetClass();
    const MELSpriteClass *rankingSectionHeaderClass = RankingSectionHeaderGetClass();

    LCDSpriteRefList sprites = self->super.sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->class == rankingItemClass || melSprite->class == rankingSectionHeaderClass || melSprite->definition.name == SpriteNameActivityIndicator) {
            playdate->sprite->setUpdateFunction(sprite, melSprite->class->destroy);
        }
    }
}

static void drawScores(TitleScene * _Nonnull self) {
#if ENABLE_ONLINE_SCOREBOARDS
    RankingSectionHeaderConstructor(MELPointMake(LCD_COLUMNS / 2, kRankingsTop + kRankingSectionHeaderHeight / 2), RankingSectionHeaderLabelLocal);
    int top = kRankingsTop + kRankingSectionHeaderHeight + kRankingSectionHeaderMargin + kRankingItemHeight / 2;
#else
    int top = kRankingsTop + kRankingItemHeight / 2;
#endif
    if (self->faces == NULL) {
        self->faces = SpriteNameLoadBitmapTable(SpriteNameRank48);
    }
    LCDBitmapTable *faces = self->faces;
    for (unsigned int index = 0; index < localScores.count; index++, top += kRankingItemHeight + kRankingItemMargin) {
        RankingItemConstructor(top, localScores.memory[index], 10, faces);
    }

#if ENABLE_ONLINE_SCOREBOARDS
    top += kRankingItemMargin;

    RankingSectionHeaderConstructor(MELPointMake(LCD_COLUMNS / 2, top + kRankingSectionHeaderHeight / 2), RankingSectionHeaderLabelOnline);
    top += kRankingSectionHeaderHeight + kRankingSectionHeaderMargin + kRankingItemHeight / 2;
    for (unsigned int index = 0; index < onlineScores.count; index++, top += kRankingItemHeight + kRankingItemMargin) {
        const Score score = onlineScores.memory[index];
        RankingItemConstructor(top, score, MELIntMax(score.super.rank, 20), faces);
    }
#endif
    self->height = top - kRankingItemHeight / 2 + 24;

    MELSprite *versionNumber = playdate->sprite->getUserdata(self->versionNumber);
    versionNumber->frame.origin.y = self->height - 10;
}

#pragma mark - Crank to title

static void updateCrankToTitle(LCDSprite * _Nonnull sprite) {
    MELSprite * _Nonnull self = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = self->frame;
    const float bottomMargin = 8.0f;
    if (camera.frame.origin.y + LCD_ROWS > MELRectangleOriginIsCenterGetBottom(frame) + bottomMargin) {
        playdate->sprite->moveTo(sprite, frame.origin.x, LCD_ROWS - frame.size.height / 2 - bottomMargin);
    } else if (camera.frame.origin.y > MELRectangleOriginIsCenterGetTop(frame) - LCD_ROWS) {
        playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y - camera.frame.origin.y);
    } else {
        playdate->sprite->setVisible(sprite, false);
    }
    MELAnimationUpdate(self->animation, DELTA);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, self->animation->frame.atlasIndex), kBitmapUnflipped);
}

#pragma mark - Kimono

static void updateWithScrollRate(LCDSprite * _Nonnull sprite) {
    MELSprite * _Nonnull self = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = self->frame;
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y - camera.frame.origin.y * 0.85f);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, self->animation->frame.atlasIndex), kBitmapUnflipped);
}
