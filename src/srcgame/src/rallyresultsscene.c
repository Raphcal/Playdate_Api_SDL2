//
//  rallyresultsscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 12/08/2025.
//

#include "rallyresultsscene.h"

#include "counter.h"
#include "rallyscorescene.h"
#include "samples.h"
#include "music.h"
#include "confetti.h"
#include "savegame.h"

static const int kCounterRight = 380;

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

RallyResultsScene * _Nonnull RallyResultsSceneAlloc(ArcadeGame game, MELBoolean success) {
    RallyResultsScene *self = new(RallyResultsScene);
    *self = (RallyResultsScene) {
        .super = {
            .type = SceneTypeRallyResults,
            .init = init,
            .update = update,
            .dealloc = dealloc,
        },
        .map = MELMapOpen(kMapNameFileNames[MapNameArcadeResult]),
        .game = game,
        .success = success,
    };
    return self;
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeRallyResults) {
        playdate->system->error("Given scene is not a RallyResultsScene instance");
        return;
    }
    RallyResultsScene *self = (RallyResultsScene *)scene;

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    LCDSpriteRefListDeinit(&self->labels);
    LCDSpriteRefListDeinit(&self->counters);
    MELMapDealloc(self->map);

    playdate->system->realloc(self, 0);
}

static void init(MELScene * _Nonnull scene) {
    RallyResultsScene *self = (RallyResultsScene *)scene;

    if (currentSaveGame.rallyRun < kScoreAttackHistoryCount) {
        currentSaveGame.rallyLastScores[currentSaveGame.rallyRun] = self->game.score;
    } else {
        const int lastIndex = kScoreAttackHistoryCount - 1;
        memmove(currentSaveGame.rallyLastScores, currentSaveGame.rallyLastScores + 1, lastIndex * sizeof(uint32_t));
        currentSaveGame.rallyLastScores[lastIndex] = self->game.score;
    }
    currentSaveGame.rallyRun++;
    
    LCDSpriteRefListEnsureCapacity(&self->labels, RallyResultsCounterCount);
    LCDSpriteRefListEnsureCapacity(&self->counters, RallyResultsCounterCount);

    MELMap *map = self->map;
    MELSpriteInstanceList instances = map->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette == NULL) {
            definition->palette = SpriteNameLoadBitmapTable(instance.name);
        }
        MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
        LCDSprite *sprite = MELSpriteInit(melSprite, definition, instances.memory + index);
        LCDSpriteRefListPush(&self->super.sprites, sprite);

        if (instance.name == SpriteNamePupCup) {
            playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
        } else if (instance.name == SpriteNameScoreAttackLabels) {
            playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
            MELSpriteSetAnimationAndDirection(melSprite, instance.animationName, (MELAnimationDirection)currentLanguage);
            LCDSpriteRefListPush(&self->labels, sprite);
        } else if (instance.name == SpriteNameWinLoseMessage) {
            self->title = melSprite;
            playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);

            switch (self->game.stage) {
                case 1 ... 5:
                    MELSpriteSetAnimationAndDirection(melSprite, AnimationNameWalk, MELAnimationDirectionRight);
                    break;
                case 6 ... 18:
                    MELSpriteSetAnimationAndDirection(melSprite, AnimationNameStand, MELAnimationDirectionRight);
                    break;
                default:
                    break;
            }
        }
        MELSpriteUpdate(sprite);
    }

    // 400,106 / top 96 total time
    LCDSpriteRefListPush(&self->counters, CounterConstructorWithFloatPointer(MELPointMake(kCounterRight, 100), MELHorizontalAlignmentRight, fontMagic, &self->counts.time));
    // 400,134 / top 124 level
    LCDSpriteRefListPush(&self->counters, CounterConstructorWithUInt8Pointer(MELPointMake(kCounterRight, 126), MELHorizontalAlignmentRight, fontMagic, &self->counts.stage));
    // 400,162 / top 152 win streak
    LCDSpriteRefListPush(&self->counters, CounterConstructorWithUInt8Pointer(MELPointMake(kCounterRight, 152), MELHorizontalAlignmentRight, fontMagic, &self->counts.bestWinStreak));
    // 400,190 / top 180 continues
    LCDSpriteRefListPush(&self->counters, CounterConstructorWithUInt32Pointer(MELPointMake(kCounterRight, 178), MELHorizontalAlignmentRight, fontMagic, &self->counts.continueCount));
    // 400,218 / top 208 score
    LCDSpriteRefListPush(&self->counters, CounterConstructorWithUInt32Pointer(MELPointMake(kCounterRight, 210), MELHorizontalAlignmentRight, fontMagic, &self->counts.score));

    MusicManagerPlay("musics/school", 0, 0.5f);
}

static void setCeresAnimation(uint8_t stage) {
    LCDSprite *sprite = MELSceneFindSpriteByName(SpriteNamePupCup);
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    switch (stage) {
        case 1 ... 5:
            MELSpriteSetAnimationAndDirection(melSprite, AnimationNameStand, MELAnimationDirectionTopLeft);
            break;
        case 6 ... 10:
            MELSpriteSetAnimationAndDirection(melSprite, AnimationNameStand, MELAnimationDirectionBottomRight);
            break;
        case 11 ... 15:
            MELSpriteSetAnimationAndDirection(melSprite, AnimationNameStand, MELAnimationDirectionTop);
            break;
        case 16 ... 18:
            MELSpriteSetAnimationAndDirection(melSprite, AnimationNameStand, MELAnimationDirectionBottom);
            break;
        case 19 ... 20:
            MELSpriteSetAnimationAndDirection(melSprite, AnimationNameStand, MELAnimationDirectionLeft);
            break;
        default:
            break;
    }
}

static int update(void * _Nonnull userdata) {
    RallyResultsScene *self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();

    const float time = self->time;
    self->time = time + DELTA;

    const float endTime = 4.4f;
    if (time < 5.0f) {
        self->title->frame.origin.y = MELEaseInOut(0.0f, 1.0f, time) * self->title->frame.size.height - self->title->frame.size.height / 2;
        for (unsigned int index = 0; index < self->counters.count; index++) {
            const float delay = 0.2f * index;
            
            LCDSprite *sprite = self->counters.memory[index];
            MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
            melSprite->frame.origin.x = (1.0f - MELEaseInOut(0.0f + delay, 1.0f + delay, time)) * 100.0f + kCounterRight - melSprite->frame.size.width / 2;
            
            sprite = self->labels.memory[index];
            melSprite = playdate->sprite->getUserdata(sprite);
            melSprite->frame.origin.x = (1.0f - MELEaseInOut(0.0f + delay, 1.0f + delay, time)) * 300.0f + 170 + melSprite->frame.size.width / 2;
        }
    }
    if (time < endTime && self->time >= endTime) {
        SamplePlay(SampleAudience);
        setCeresAnimation(self->game.stage);
    } else if (self->time > endTime && (int)(self->time * 10) % 10 == 0) {
        ConfettiConstructor(MELRectangleMake(0, 0, LCD_COLUMNS, 12), 5);
    }

    self->counts.time = MELProgress(1.5f, 2.0f, time) * self->game.time;
    self->counts.stage = MELProgress(2.1f, 2.6f, time) * self->game.stage;
    self->counts.bestWinStreak = MELProgress(2.7f, 3.2f, time) * self->game.bestWinStreak;
    self->counts.continueCount = MELProgress(3.3f, 3.8f, time) * self->game.continueCount;
    self->counts.score = MELProgress(3.9f, endTime, time) * self->game.score;

    playdate->sprite->updateAndDrawSprites();

    const MELController controller = MELControllerMake();
    if (controller.pressedA && self->time < endTime) {
        self->time = endTime;
        SamplePlay(SampleAudience);
        setCeresAnimation(self->game.stage);
    } else if (controller.pressedA) {
        RallyScoreScene *rallyScoreScene = RallyScoreSceneAlloc();
        MELSceneMakeCurrent(&rallyScoreScene->super);
    }

    return true;
}
