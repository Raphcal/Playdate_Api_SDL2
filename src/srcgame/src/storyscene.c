//
//  storyscene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/08/2023.
//

#include "storyscene.h"

#include "camera.h"
#include "gamescene.h"
#include "layersprite.h"
#include "image.h"
#include "text.h"
#include "textbubble.h"
#include "animationlink.h"

static const float kMaximumScrollSpeed = 280.0f; // Pour la démo : 180 ou 210
static const float kScrollAcceleration = 600.0f;

static const float kFrameMargin = 16.0f;
static const float kFrame0Top = 0.0f;
static const float kFrame1Top = kFrame0Top + LCD_ROWS + kFrameMargin;
static const float kFrame2Top = kFrame1Top + LCD_ROWS + kFrameMargin;
static const float kFrame3Top = kFrame2Top + LCD_ROWS + kFrameMargin;

static void init(MELScene * _Nonnull scene);
static void initSprite(StoryScene * _Nonnull self, SpriteName spriteName, LCDSprite * _Nonnull sprite, MELSprite * _Nonnull melSprite);
static void dealloc(MELScene * _Nonnull self);
static int update(void * _Nonnull userdata);

static float SouHaSasenaiAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);
static float MoveSpriteAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);
static float MechRisingSpriteAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);
static float GoTransitionAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange);

static void updateTalkOnCrank(LCDSprite * _Nonnull sprite);

static AnimationLink kAnimationChain[] = {
    { .update = BToSkipSpriteAnimationLinkUpdate },
    {
        .update = WaitSpriteAnimationLinkUpdate,
        .to = 1.0f,
    },
    {
        .update = ScrollTextBubbleAnimationLinkUpdate,
        .text = "\n\
\n\
How are you ladies and\n\
gentlemen!!\n\
\n\
You refused to fund\n\
my robots and called\n\
me mad.\n\
\n\
Well, the time has\n\
come for you to pay.",
        .textLineCount = 11,
        .TEXT_X = LCD_COLUMNS / 2,
        .TEXT_Y = kFrame0Top + LCD_ROWS - TEXT_BUBBLE_HEIGHT / 2 - GUI_PADDING,
    },
    { .update = WaitSpriteAnimationLinkUpdate },
    {
        .update = CameraAnimationLinkUpdate,
        .to = kFrame1Top,
    },
    {
        .update = ScrollTextBubbleAnimationLinkUpdate,
        .text = "\n\
\n\
In a mere 3 minutes,\n\
my satellite will fire\n\
and wipe this country\n\
off the map!\n\
\n\
It is too late for\n\
regrets!",
        .textLineCount = 9,
        .TEXT_X = LCD_COLUMNS / 2,
        .TEXT_Y = 400.0f, // Calculé dans MapMaker
    },
    { .update = SouHaSasenaiAnimationLinkUpdate },
    { .update = MoveSpriteAnimationLinkUpdate },
    { .update = WaitSpriteAnimationLinkUpdate },
    {
        .update = CameraAnimationLinkUpdate,
        .from = kFrame1Top,
        .to = kFrame2Top,
    },
    { .update = MechRisingSpriteAnimationLinkUpdate },
    { .update = WaitSpriteAnimationLinkUpdate },
    {
        .update = CameraAnimationLinkUpdate,
        .from = kFrame2Top,
        .to = kFrame3Top,
    },
    { .update = GoTransitionAnimationLinkUpdate }
};

StoryScene * _Nonnull StorySceneAlloc(PlayerCharacter character) {
    return StorySceneAllocWithMapNameAndLinkCount(character, MapNameStory, ANIMATION_LINK_COUNT(kAnimationChain));
}

StoryScene * _Nonnull StorySceneAllocWithMapNameAndLinkCount(PlayerCharacter character, MapName mapName, uint8_t linkCount) {
    StoryScene *self = playdate->system->realloc(NULL, sizeof(StoryScene));
    *self = (StoryScene) {
        .super = {
            .type = SceneTypeStory,
            .init = init,
            .dealloc = dealloc,
            .update = update,
        },
        .initSprite = initSprite,
        .character = character,
        .mapName = mapName,
        .map = MELMapOpen(kMapNameFileNames[mapName]),
        .linkCount = linkCount,
    };
    return self;
}

static LCDSprite * _Nonnull createSprite(MELSpriteInstance * _Nonnull instance) {
    const SpriteName name = instance->name;
    MELSpriteDefinition *definition = SpriteNameGetDefinition(name);
    definition->palette = SpriteNameLoadBitmapTable(name);

    LCDSprite *sprite = playdate->sprite->newSprite();
    const MELPoint center = instance->center;
    playdate->sprite->moveTo(sprite, center.x, center.y);

    MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
    *melSprite = (MELSprite) {
        .class = &MELSpriteClassDefault,
        .definition = *definition,
        .frame = (MELRectangle) {
            .origin = center,
            .size = definition->size
        },
        .direction = instance->direction,
        .instance = instance,
    };
    MELSpriteSetAnimation(melSprite, AnimationNameStand);

    playdate->sprite->setUserdata(sprite, melSprite);
    playdate->sprite->setUpdateFunction(sprite, &MELSpriteUpdate);
    playdate->sprite->addSprite(sprite);
    playdate->sprite->setZIndex(sprite, ZINDEX_PLAYER);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push StoryScene#createSprite(%x, %x): %d", sprite, melSprite, melSprite->definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void init(MELScene * _Nonnull scene) {
    StoryScene *self = (StoryScene *)scene;
    MELMap *map = self->map;

    for (unsigned int index = 0; index < map->layerCount; index++) {
        MELLayer *layer = map->layers + index;
        LCDBitmap *image = loadMapLayer(self->mapName, index);
        LayerSpriteConstructor(layer, image, false);
    }

    MELSpriteInstanceList instances = map->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        LCDSprite *sprite = createSprite(instances.memory + index);
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        self->initSprite(self, instances.memory[index].name, sprite, melSprite);
    }

    camera.frame.origin = MELPointZero;
}

static void initSprite(StoryScene * _Nonnull self, SpriteName spriteName, LCDSprite * _Nonnull sprite, MELSprite * _Nonnull melSprite) {
    switch (spriteName) {
        case SpriteNameHeroLookingAtScreen:
        case SpriteNameGoneFarAway:
        case SpriteNameMecha:
            // Affichage du bon personnage.
            MELSpriteSetAnimation(melSprite, (AnimationName) self->character);
            break;
        case SpriteNameSouHaSasenai:
            // Affichage du message en fonction du personnage et le masque.
            MELSpriteSetAnimation(melSprite, (AnimationName) self->character);
            melSprite->class = ImageGetClass();
            playdate->sprite->setVisible(sprite, false);
            playdate->sprite->setZIndex(sprite, ZINDEX_PLAYER + 2);
            break;
        case SpriteNameBadguy:
            MELSpriteSetAnimation(melSprite, AnimationNameWalk);
            playdate->sprite->setUpdateFunction(sprite, updateTalkOnCrank);
            break;
        case SpriteNameGo:
            melSprite->frame.origin.x += LCD_COLUMNS;
            playdate->sprite->setZIndex(sprite, ZINDEX_FADE);
            break;
        default:
            // Pas de traitement.
            break;
    }
}

static void dealloc(MELScene * _Nonnull scene) {
    StorySceneDealloc(scene, kAnimationChain);
}

void StorySceneDealloc(MELScene * _Nonnull scene, AnimationLink * _Nonnull chain) {
    if (scene->type != SceneTypeStory) {
        playdate->system->error("Given scene is not a SceneTypeStory instance");
        return;
    }
    StoryScene *self = (StoryScene *)scene;
    LCDSpriteRefListDeallocReverse(&self->super.sprites);

    MELSpriteInstanceList instances = self->map->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette) {
            playdate->graphics->freeBitmapTable(definition->palette);
            definition->palette = NULL;
        }
    }
    MELMapDealloc(self->map);

    const unsigned int count = self->linkCount;
    for (unsigned int index = 0; index < count; index++) {
        chain[index].sprite = NULL;
        chain[index].increment = 0;
        chain[index].progress = 0.0f;
    }

    playdate->system->realloc(self, 0);
}

void StorySceneUpdateAnimationChain(StoryScene * _Nonnull self, AnimationLink * _Nonnull animationChain) {
    playdate->sprite->updateAndDrawSprites();
#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    PDButtons pressing;
    playdate->system->getButtonState(&pressing, NULL, NULL);
    if (pressing & kButtonUp) {
        self->scrollSpeed = MELFloatMax(self->scrollSpeed - kScrollAcceleration * DELTA, -kMaximumScrollSpeed);
    }
    else if (pressing & kButtonDown) {
        self->scrollSpeed = MELFloatMin(self->scrollSpeed + kScrollAcceleration * DELTA, kMaximumScrollSpeed);
    } else {
        self->scrollSpeed = 0.0f;
    }

    float remaining = MELCrankGetAcceleratedChange();
    if (remaining == 0.0f) {
        remaining = self->scrollSpeed * DELTA;
    }
    int link = self->link;
    do {
        remaining = animationChain[link].update(animationChain + link, remaining);
        link += animationChain[link].increment;
    } while (remaining != 0.0f);
    if (currentScene->type == SceneTypeStory) {
        self->link = link;
    }
}

static int update(void * _Nonnull userdata) {
    StoryScene *self = userdata;
    StorySceneUpdateAnimationChain(self, kAnimationChain);

    PDButtons pressed;
    playdate->system->getButtonState(NULL, &pressed, NULL);

    if (pressed & kButtonB) {
        GameScene *gameScene = GameSceneAlloc(self->character);
        MELSceneMakeCurrent(&gameScene->super);
    }
    return true;
}


#pragma mark - Affichage de la bulle "Sou ha sasenai"

static void SouHaSasenaiScale(MELSprite * _Nonnull self, LCDSprite * _Nonnull sprite, float zoom) {
    LCDBitmap *image = playdate->graphics->getTableBitmap(self->definition.palette, self->animation->frame.atlasIndex);

    LCDBitmap *currentImage = playdate->sprite->getImage(sprite);
    if (currentImage != NULL && currentImage != image) {
        playdate->graphics->freeBitmap(currentImage);
    }

    if (zoom == 1.0f) {
        playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    } else {
        const MELSize size = self->frame.size;
        LCDBitmap *zoomedImage = playdate->graphics->newBitmap(size.width * zoom, size.height * zoom, kColorClear);
        playdate->graphics->pushContext(zoomedImage);
        playdate->graphics->drawScaledBitmap(image, 0, 0, zoom, zoom);
        playdate->graphics->popContext();
        playdate->sprite->setImage(sprite, zoomedImage, kBitmapUnflipped);
    }
}

static void SouHaSasenaiUpdateAppear(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float duration = 0.4f;
    if (self->hitTimer < duration) {
        const float newTime = self->hitTimer = MELFloatMin(self->hitTimer + DELTA, duration);
        const float progress = MELEaseOutElastic(0, duration, newTime);
        const float zoom = 0.75f + 0.25f * progress;

        SouHaSasenaiScale(self, sprite, zoom);

        const MELPoint origin = self->frame.origin;
        playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
        return;
    }
    MELSpriteUpdate(sprite);
    playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
    self->hitTimer = 0.0f;
}

static void SouHaSasenaiUpdateDisappear(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const float duration = 0.4f;
    if (self->hitTimer < duration) {
        const float newTime = self->hitTimer = MELFloatMin(self->hitTimer + DELTA, duration);
        const float progress = MELEaseInOut(0, duration, newTime);
        const float zoom = 1.0f - 0.25f * progress;

        SouHaSasenaiScale(self, sprite, zoom);

        const MELRectangle frame = self->frame;
        playdate->sprite->moveTo(sprite, frame.origin.x - camera.frame.origin.x, frame.origin.y - camera.frame.origin.y);
        playdate->sprite->setSize(sprite, frame.size.width * zoom, frame.size.height * zoom);
        playdate->sprite->markDirty(sprite);
        return;
    }
    playdate->sprite->setVisible(sprite, false);
    playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
}

static float SouHaSasenaiAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *souHaSasenai = self->sprite;
    if (souHaSasenai == NULL) {
        souHaSasenai = self->sprite = MELSceneFindSpriteByName(SpriteNameSouHaSasenai);
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(souHaSasenai);
    if (!playdate->sprite->isVisible(souHaSasenai)) {
        melSprite->hitTimer = 0.0f;
        playdate->sprite->setVisible(souHaSasenai, true);
        playdate->sprite->setUpdateFunction(souHaSasenai, SouHaSasenaiUpdateAppear);
    }
    const float crankFactor = 12.0f;
    const float distance = 16.0f;
    const float startY = melSprite->instance->center.y;
    const float endY = startY - distance;
    const float y = melSprite->frame.origin.y - crankChange / crankFactor;
    if (y > startY) {
        melSprite->hitTimer = 0.0f;
        melSprite->frame.origin.y = startY;
        playdate->sprite->setUpdateFunction(souHaSasenai, SouHaSasenaiUpdateDisappear);
        self->increment = -1;
        return (startY - y) * crankFactor;
    }
    else if (y <= endY) {
        melSprite->hitTimer = 0.0f;
        melSprite->frame.origin.y = endY;
        playdate->sprite->setUpdateFunction(souHaSasenai, SouHaSasenaiUpdateDisappear);
        self->increment = 1;
        return (endY - y) * crankFactor;
    }
    else {
        melSprite->frame.origin.y = y;
        self->increment = 0;
        return 0.0f;
    }
}

#pragma mark - Déplacement du héro vers la droite

static float MoveSpriteAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *sprite = self->sprite;
    if (sprite == NULL) {
        sprite = self->sprite = MELSceneFindSpriteByName(SpriteNameHeroLookingAtScreen);
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    const float from = melSprite->instance->center.x;
    const float oldX = melSprite->frame.origin.x;
    if (oldX == from && crankChange > 0.0f) {
        LCDSprite *souHaSasenai = MELSceneFindSpriteByName(SpriteNameSouHaSasenai);
        if (playdate->sprite->isVisible(souHaSasenai)) {
            self->increment = 0;
            return 0.0f;
        }
    }

    const float to = LCD_COLUMNS + melSprite->frame.size.width / 2.0f;
    const float distance = to - from;
    const float crankFactor = 1.0f;
    const float progress = self->progress += crankChange / crankFactor;

    if (progress < 0.0f) {
        melSprite->frame.origin.x = from;
        self->increment = -1;
        self->progress = 0.0f;
        return progress * crankFactor;
    }
    else if (progress >= distance) {
        melSprite->frame.origin.x = to;
        self->increment = 1;
        self->progress = distance;
        return (progress - distance) * crankFactor;
    }
    else {
        self->increment = 0;
        melSprite->frame.origin.x = from + MELEaseIn(0, distance, progress) * distance;
        return 0.0f;
    }
}

#pragma mark - Sortie du mécha

static float MechRisingSpriteAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *mecha = self->sprite;
    if (mecha == NULL) {
        mecha = self->sprite = MELSceneFindSpriteByName(SpriteNameMecha);
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(mecha);

    const float crankFactor = 2.0f;
    const float startY = melSprite->instance->center.y;
    const float endY = kFrame2Top + LCD_ROWS - melSprite->frame.size.height / 2.0f;
    const float y = melSprite->frame.origin.y - crankChange / crankFactor;
    if (y > startY) {
        melSprite->frame.origin.y = startY;
        self->increment = -1;
        return (startY - y) * crankFactor;
    }
    else if (y <= endY) {  
        melSprite->frame.origin.y = endY;
        self->increment = 1;
        return (endY - y) * crankFactor;
    } else {
        melSprite->frame.origin = (MELPoint) {
            .x = melSprite->instance->center.x + 8.0f * fabsf(2.0f * (y - floorf(y + 0.5f))),
            .y = y
        };
        self->increment = 0;
        return 0.0f;
    }
}

#pragma mark - Transition avec le mot GO !

static float GoTransitionAnimationLinkUpdate(AnimationLink * _Nonnull self, float crankChange) {
    LCDSprite *sprite = self->sprite;
    if (sprite == NULL) {
        sprite = self->sprite = MELSceneFindSpriteByName(SpriteNameGo);
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

        StoryScene *storyScene = (StoryScene *)currentScene;
        GameScene *gameScene = GameSceneAllocWithTransition(storyScene->character);
        MELSceneMakeCurrent(&gameScene->super);

        return 0.0f;
    }
    else {
        self->increment = 0;
        melSprite->frame.origin.x = from + MELEaseInOut(0, -distance, progress) * distance;
        return 0.0f;
    }
}

#pragma mark - Animation de la bouche avec les mouvements de la manivelle

static void updateTalkOnCrank(LCDSprite * _Nonnull sprite) {
    MELSprite * _Nonnull self = playdate->sprite->getUserdata(sprite);
    MELAnimation *animation;
    PDButtons pressing;
    playdate->system->getButtonState(&pressing, NULL, NULL);

    const float crankChange = playdate->system->getCrankChange();
    if (camera.frame.origin.y == 0 && (crankChange != 0.0f || pressing & kButtonDown)) {
        MELSpriteSetAnimation(self, AnimationNameWalk);
        animation = self->animation;
        MELAnimationUpdate(animation, DELTA);
    } else {
        MELSpriteSetAnimation(self, AnimationNameStand);
        animation = self->animation;
    }

    MELPoint origin = self->frame.origin;
    playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
}
