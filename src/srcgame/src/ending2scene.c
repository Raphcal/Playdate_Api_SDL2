//
//  ending2scene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 12/10/2023.
//

#include "ending2scene.h"

#include "animationlink.h"
#include "../gen/maps.h"

#include "titlescene.h"
#include "scoreentryscene.h"
#include "savegame.h"

static const float kFrameMargin = 16.0f;
static const float kFrame0Top = 0;
static const float kFrame0Bottom = kFrame0Top + 11 * 32 - LCD_ROWS;
static const float kFrame1Top = kFrame0Bottom + LCD_ROWS + kFrameMargin + kFrameMargin;
static const float kFrame2Bottom = 28 * 32 - LCD_ROWS;

static void initSprite(StoryScene * _Nonnull self, SpriteName spriteName, LCDSprite * _Nonnull sprite, MELSprite * _Nonnull melSprite);
static int update(void * _Nonnull userdata);
static void dealloc(MELScene * _Nonnull scene);

static float flyingTowardCamera(AnimationLink * _Nonnull self, float crankChange);
static float falling(AnimationLink * _Nonnull self, float crankChange);
static float landing(AnimationLink * _Nonnull self, float crankChange);
static float owariLink(AnimationLink * _Nonnull self, float crankChange);
static float victoryTransitionAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);

static AnimationLink kAnimationChain[] = {
    { .update = BToSkipSpriteAnimationLinkUpdate },
    { .update = CameraAnimationLinkUpdate, .from = kFrame0Top, .to = kFrame0Bottom },
    { .update = flyingTowardCamera },
    { .update = CameraAnimationLinkUpdate, .from = kFrame0Bottom, .to = kFrame1Top },
    { .update = falling },
    { .update = landing },
    { .update = CameraAnimationLinkUpdate, .from = kFrame1Top, .to = kFrame2Bottom },
    { .update = owariLink },
    { .update = WaitSpriteAnimationLinkUpdate, .to = 50 },
    {
        .update = ScrollTextBubbleAnimationLinkUpdate,
        .text = "\n\nThanks for playing!\n-- Raphael Calabro", .textLineCount = 4,
        .TEXT_X = LCD_COLUMNS / 2,
        .TEXT_Y = kFrame2Bottom + LCD_ROWS - TEXT_BUBBLE_HEIGHT / 2 - GUI_PADDING,
    },
    { .update = victoryTransitionAnimationLinkUpdate },
};

StoryScene * _Nonnull Ending2SceneSceneAlloc(PlayerCharacter character, unsigned int score, Ranks ranks) {
    StoryScene *self = StorySceneAllocWithMapNameAndLinkCount(character, MapNameEnding2, ANIMATION_LINK_COUNT(kAnimationChain));
    self->initSprite = initSprite;
    self->super.dealloc = dealloc;
    self->super.update = update;
    self->super.save = StorySceneSaveState;
    self->score = score;
    self->ranks = ranks;
    return self;
}

static void initSprite(StoryScene * _Nonnull self, SpriteName spriteName, LCDSprite * _Nonnull sprite, MELSprite * _Nonnull melSprite) {
    switch (spriteName) {
        case SpriteNameFlying:
            // +1 car l'animation "stand" affiche une image plus simple à gérer dans MapMaker.
            MELSpriteSetAnimation(melSprite, (AnimationName) self->character + 1);
            MELSingleFrameAnimationReuse(melSprite->animation);
            playdate->sprite->setZIndex(sprite, ZINDEX_PLAYER + 1);
            playdate->sprite->setVisible(sprite, false);
            break;
        case SpriteNameLanding:
            MELSpriteSetAnimation(melSprite, (AnimationName) self->character);
            MELSingleFrameAnimationReuse(melSprite->animation);
            break;
        case SpriteNameEnding2:
            MELSpriteSetAnimation(melSprite, (AnimationName) self->character);
            break;
        case SpriteNameOwari:
            // Déplace le message "fin" dessous l'écran.
            melSprite->frame.origin.y += LCD_ROWS;
            break;
        case SpriteNameVictoryTransition:
            melSprite->frame.origin.x += LCD_COLUMNS;
            playdate->sprite->setZIndex(sprite, ZINDEX_FADE);
            MELSpriteSetAnimation(melSprite, (AnimationName) self->character * 2);
            break;
        default:
            // Pas de traitement.
            break;
    }
}

static void dealloc(MELScene * _Nonnull scene) {
    StorySceneDealloc(scene, kAnimationChain);
}

static int update(void * _Nonnull userdata) {
    StoryScene *self = userdata;
    StorySceneUpdateAnimationChain(self, kAnimationChain);

    PDButtons pressed;
    playdate->system->getButtonState(NULL, &pressed, NULL);

    if (pressed & kButtonB) {
        ScoreEntryScene *scoreEntryScene = ScoreEntryAlloc(self->character, self->score, self->ranks);
        if (scoreEntryScene) {
            MELSceneMakeCurrent(&scoreEntryScene->super);
        } else {
            TitleScene *titleScene = TitleSceneAlloc();
            MELSceneMakeCurrent(&titleScene->super);
        }
    }
    return true;
}

static float flyingTowardCamera(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *sprite = self->sprite;
    if (sprite == NULL) {
        sprite = self->sprite = MELSceneFindSpriteByName(SpriteNameFlying);
    }
    if (!playdate->sprite->isVisible(sprite)) {
        playdate->sprite->setVisible(sprite, true);
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    const int direction = melSprite->animationName == AnimationNameRun ? 1 : -1;
    const MELPoint distance = (MELPoint) { .x = 128 * direction, .y = -160 };
    const MELPoint from = melSprite->instance->center;
    const MELPoint to = MELPointAdd(from, distance);

    const float crankFactor = 300.0f;
    const float progress = self->progress += crankChange / crankFactor;

    const int lastFrame = melSprite->animation->definition->frameCount - 1;
    MELAnimationSetFrameIndex(melSprite->animation, lastFrame * MELFloatBound(0.0f, progress, 1.0f));
    if (progress < 0.0f) {
        melSprite->frame.origin = from;
        self->increment = -1;
        self->progress = 0.0f;
        playdate->sprite->setVisible(sprite, false);
        return progress * crankFactor;
    }
    else if (progress >= 1.0f) {
        melSprite->frame.origin = to;
        self->increment = 1;
        self->progress = 1.0f;
        return (progress - 1.0f) * crankFactor;
    }
    else {
        self->increment = 0;
        melSprite->frame.origin = (MELPoint) {
            .x = from.x + MELEaseInOut(0, 1, progress) * distance.x,
            .y = from.y - 432 * MELFloatSquare(progress) + 272 * progress,
        };
        return 0.0f;
    }
}

static float falling(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *sprite = self->sprite;
    if (sprite == NULL) {
        sprite = self->sprite = MELSceneFindSpriteByName(SpriteNameLanding);
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    const float from = melSprite->instance->center.y;
    const float to = 552; // Calculé dans MapMaker
    const float distance = to - from;

    const float crankFactor = 3.0f;
    const float progress = self->progress += crankChange / crankFactor;

    if (progress < 0.0f) {
        melSprite->frame.origin.y = from;
        self->increment = -1;
        self->progress = 0.0f;
        return progress * crankFactor;
    }
    else if (progress >= distance) {
        melSprite->frame.origin.y = to;
        self->increment = 1;
        self->progress = distance;
        return (progress - distance) * crankFactor;
    }
    else {
        self->increment = 0;
        melSprite->frame.origin.y = from + MELEaseInOut(0, distance, progress) * distance;
        return 0.0f;
    }
}

static float landing(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *sprite = self->sprite;
    if (sprite == NULL) {
        sprite = self->sprite = MELSceneFindSpriteByName(SpriteNameLanding);
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    const float lastFrame = melSprite->animation->definition->frameCount - 1;

    const float crankFactor = 9.0f;
    const float progress = self->progress += crankChange / crankFactor;

    MELAnimationSetFrameIndex(melSprite->animation, MELFloatBound(0, progress, lastFrame));
    if (progress < 0.0f) {
        self->increment = -1;
        self->progress = 0.0f;
        return progress * crankFactor;
    }
    else if (progress >= lastFrame) {
        self->increment = 1;
        self->progress = lastFrame;
        return (progress - lastFrame) * crankFactor;
    }
    else {
        self->increment = 0;
        return 0.0f;
    }
}

static float owariLink(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *sprite = self->sprite;
    if (sprite == NULL) {
        sprite = self->sprite = MELSceneFindSpriteByName(SpriteNameOwari);
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);

    const float crankFactor = 2.0f;
    const float startY = kFrame2Bottom + LCD_ROWS + melSprite->frame.size.height / 2.0f;
    const float endY = melSprite->instance->center.y;
    const float distance = startY - endY;
    const float progress = self->progress += crankChange / crankFactor;

    if (progress <= 0.0f) {
        melSprite->frame.origin.y = startY;
        self->increment = -1;
        return progress * crankFactor;
    }
    else if (progress >= distance) {
        melSprite->frame.origin.y = endY;
        self->increment = 1;
        return (progress - distance) * crankFactor;
    } else {
        melSprite->frame.origin.y = startY - MELEaseInOut(0, distance, progress) * distance;
        self->increment = 0;
        return 0.0f;
    }
}

#pragma mark - Transition avec ombre du personnage

static float victoryTransitionAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *sprite = self->sprite;
    if (sprite == NULL) {
        sprite = self->sprite = MELSceneFindSpriteByName(SpriteNameVictoryTransition);
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    const float from = LCD_COLUMNS + melSprite->frame.size.width / 2;
    const float to = LCD_COLUMNS - melSprite->frame.size.width / 2;
    const float distance = to - from;

    const float crankFactor = 0.5f;
    const float progress = self->progress += crankChange / crankFactor;

    if (progress < 0.0f) {
        melSprite->frame.origin.x = from;
        self->increment = -1;
        self->progress = 0.0f;
        return progress * crankFactor;
    }
    else if (progress >= -distance) {
        melSprite->frame.origin.x = to;
        self->increment = 0;
        self->progress = distance;

        StoryScene *self = (StoryScene *)currentScene;
        ScoreEntryScene *scoreEntryScene = ScoreEntryAlloc(self->character, self->score, self->ranks);
        if (scoreEntryScene) {
            scoreEntryScene->showTransition = true;
            MELSceneMakeCurrent(&scoreEntryScene->super);
        } else {
            TitleScene *titleScene = TitleSceneAlloc();
            titleScene->showTransition = true;
            MELSceneMakeCurrent(&titleScene->super);
        }

        return 0.0f;
    }
    else {
        self->increment = 0;
        melSprite->frame.origin.x = from + MELEaseInOut(0, -distance, progress) * distance;
        return 0.0f;
    }
}
