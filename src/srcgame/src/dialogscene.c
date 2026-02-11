//
//  dialogscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#include "dialogscene.h"

#include "samples.h"
#include "music.h"
#include "layersprite.h"
#include "textbubble.h"
#include "savegame.h"
#include "gamescene.h"
#include "endingscene.h"
#include "lessonselectscene.h"
#include "lessonscene.h"
#include "lessons.h"
#include "charactersprite.h"
#include "../lib/camera.h"
#include "../gen/maps.h"

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);
static void dialogChanged(void * _Nonnull userdata, int _);
static void dialogHasEnded(void * _Nonnull userdata, int _);
static void setAnimation(MELSprite * _Nonnull self, AnimationName animationName);
static void setUpdateFunctionAndZIndex(LCDSprite * _Nonnull sprite, int zindex);

DialogScene * _Nonnull DialogSceneAlloc(const Story * _Nullable story, unsigned int encounterIndex) {
    DialogScene *self = playdate->system->realloc(NULL, sizeof(DialogScene));

    PhraseList dialog = (PhraseList) {};
    Background dialogBackground = BackgroundSchoolClassroom;
    if (encounterIndex < story->encounters.count) {
        Encounter *encounter = story->encounters.memory + encounterIndex;
        dialog = encounter->dialog;
        dialogBackground = encounter->background;
    }

    *self = (DialogScene) {
        .super = {
            .type = SceneTypeDialog,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .sprites = LCDSpriteRefListEmpty,
        },
        .background = MELMapOpen(kMapNameFileNames[MapNameDialog]),
        .backgroundName = MapNameDialog,
        .story = story,
        .encounterIndex = encounterIndex,
        .dialog = dialog,
        .dialogBackground = dialogBackground,
    };
    return self;
}

DialogScene * _Nonnull DialogSceneAllocWithLesson(const Lesson * _Nullable lesson) {
    DialogScene *self = playdate->system->realloc(NULL, sizeof(DialogScene));

    *self = (DialogScene) {
        .super = {
            .type = SceneTypeDialog,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .sprites = LCDSpriteRefListEmpty,
        },
        .background = MELMapOpen(kMapNameFileNames[MapNameDialog]),
        .backgroundName = MapNameDialog,
        .lesson = lesson,
        .dialog = lesson->dialog,
        .dialogBackground = BackgroundSchoolClassroom,
    };
    return self;
}

#pragma mark - Fonctions privées

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeDialog) {
        playdate->system->error("Given scene is not a DialogScene instance");
        return;
    }
    DialogScene *self = (DialogScene *)scene;

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->background);
    playdate->system->realloc(self, 0);
}

static void init(MELScene * _Nonnull scene) {
    DialogScene *self = (DialogScene *)scene;

    const MapName mapName = self->backgroundName;
    MELMap *background = self->background;
    for (unsigned int index = 0; index < background->layerCount; index++) {
        MELLayer *layer = background->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);
        LCDSpriteRefListPush(&self->super.sprites, LayerSpriteConstructor(layer, image));
    }

    MELSpriteInstanceList instances = background->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette == NULL) {
            definition->palette = SpriteNameLoadBitmapTable(instance.name);
        }
        MELSprite *melSprite;
        LCDSprite *sprite;

        switch (instance.name) {
            case SpriteNameLargePortaits:
                if (self->lesson && instance.direction == MELDirectionRight) {
                    CharacterSpriteMakeImage(instances.memory + index, self->dialog);
                } else {
                    CharacterSpriteMake(instances.memory + index, instance.direction == MELDirectionRight ? SideLeft : SideRight, self->dialog);
                }
                break;
            case SpriteNameDialogBackground:
                melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
                sprite = MELSpriteInit(melSprite, definition, instances.memory + index);
                LCDSpriteRefListPush(&self->super.sprites, sprite);
                setAnimation(melSprite, (AnimationName)self->dialogBackground);
                setUpdateFunctionAndZIndex(sprite, ZINDEX_BG);
                break;
            case SpriteNameWhiteGrid:
                melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
                sprite = MELSpriteInit(melSprite, definition, instances.memory + index);
                LCDSpriteRefListPush(&self->super.sprites, sprite);
                setUpdateFunctionAndZIndex(sprite, ZINDEX_WHITE_GRID);
                break;
            default:
                break;
        }
    }
    MELEventBusFireEvent(EventDialogChanged, 0);

    const float backgroundAndCharactersHeight = 8.0f * 16.0f;
    const float marginTop = 2.0f;
    const float height = LCD_ROWS - backgroundAndCharactersHeight - marginTop;
    TextBubbleConstructor(self->dialog, (MELRectangle) {
        .origin = {
            .x = 8 + (LCD_COLUMNS - 16) / 2,
            .y = backgroundAndCharactersHeight + marginTop + height / 2
        },
        .size = {
            .width = LCD_COLUMNS - 16,
            .height = height
        }
    });

    MELEventBusAddListener(EventDialogChanged, dialogChanged, self);
    MELEventBusAddListener(EventDialogHasEnded, dialogHasEnded, self);

    if (self->story && !self->story->encounters.memory[self->encounterIndex].actor) {
        // Dialogue avant la fin.
        MusicManagerPlay("musics/school", 0, 0.25f);
    } else {
        MusicManagerPlay("musics/dialog", 0, 0.25f);
    }

    camera.frame.origin = MELPointZero;
    setRefreshRate(DEFAULT_REFRESH_RATE);
}

static void setAnimation(MELSprite * _Nonnull self, AnimationName animationName) {
    MELAnimation *currentAnimation = self->animation;
    if (animationName != self->animationName || !currentAnimation) {
        self->animationName = animationName;
        MELAnimationDealloc(currentAnimation);
        MELAnimation *anAnimation;
        anAnimation = MELSpriteDefinitionGetAnimation(self->definition, animationName, MELAnimationDirectionRight);
        anAnimation->class->start(anAnimation);
        self->animation = anAnimation;
    }
}

static void setUpdateFunctionAndZIndex(LCDSprite * _Nonnull sprite, int zindex) {
    MELSpriteUpdate(sprite);
    playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
    playdate->sprite->setZIndex(sprite, zindex);
}

static int update(void * _Nonnull userdata) {
    DialogScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;
    playdate->sprite->updateAndDrawSprites();

    const MELBoolean goToNextScene = self->goToGameScene || MELControllerMake().pressedB;
    if (goToNextScene && self->story) {
        Encounter encounter = self->encounterIndex < self->story->encounters.count
            ? self->story->encounters.memory[self->encounterIndex]
            : (Encounter) {};
        MELScene *nextScene;
        if (encounter.actor) {
            GameScene *gameScene = GameSceneAlloc(self->story, self->encounterIndex);
            nextScene = &gameScene->super;
        } else {
            const StoryName storyName = StoryNameByCharacter(self->story->character);
            currentSaveGame.completion[storyName] = playdate->system->getSecondsSinceEpoch(NULL);
            currentSaveGame.progression[storyName] = 0;
            SaveGameToDisk(&currentSaveGame);

            unlockAchievement((Achievement)storyName);

            EndingScene *endingScene = EndingSceneAlloc();
            nextScene = &endingScene->super;
        }
        MELSceneMakeCurrent(nextScene);
    } else if (goToNextScene && self->lesson) {
        const int lessonIndex = (int) (self->lesson - kLessons.memory);
        if (LessonObjectivesAreEmpty(self->lesson->objectives)) {
            markLessonDone(lessonIndex);
            LessonSelectScene *lessonSelectScene = LessonSelectSceneAlloc();
            MELSceneMakeCurrent(&lessonSelectScene->super);
        } else {
            LessonScene *lessonScene = LessonSceneAlloc(lessonIndex);
            MELSceneMakeCurrent(&lessonScene->super);
        }
    }
    return true;
}

static void dialogChanged(void * _Nonnull userdata, int phraseIndex) {
    DialogScene *self = userdata;
    const Phrase phrase = self->dialog.memory[phraseIndex];
    if (phrase.effect == EffectShake) {
        LCDSprite *sprite = MELCameraShakeConstructor(1.0f, 6.0f);
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }
}

static void dialogHasEnded(void * _Nonnull userdata, int _) {
    DialogScene *self = userdata;
    self->goToGameScene = true;
}
