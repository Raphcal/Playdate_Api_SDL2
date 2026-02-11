//
//  lessonscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 21/02/2024.
//

#include "lessonscene.h"

#include "samples.h"
#include "music.h"
#include "carddeck.h"
#include "cardsprite.h"
#include "layersprite.h"
#include "lessonselectscene.h"
#include "lessons.h"
#include "savegame.h"
#include "text.h"
#include "image.h"
#include "win_lose_message.h"
#include "../gen/maps.h"

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

static void playerLost(void * _Nullable userdata, int _);
static void playerMayLose(void * _Nullable userdata, int mayLose);
static void playerRemovedCards(void * _Nullable userdata, int count);
static void playerRemovedChips(void * _Nullable userdata, int count);
static void playerSentChips(void * _Nullable userdata, int count);
static void playerMadeCombo(void * _Nullable userdata, int value);

static const MELLocalizedString kCardsToRemove = (MELLocalizedString) {
    .text = {
        "Remaining: 00",
        "À escamoter : 00",
        "トランプ：00",
        "Restante: 00"
    }
};
static const MELLocalizedString kChipsToRemove = (MELLocalizedString) {
    .text = {
        "Remaining: 00",
        "À escamoter : 00",
        "チップ：00",
        "Restante: 00"
    }
};
static const MELLocalizedString kChipsToSend = (MELLocalizedString) {
    .text = {
        "Remaining: 00",
        "À envoyer : 00",
        "チップ：00",
        "Restante: 00"
    }
};
static const MELLocalizedString kComboToDo = (MELLocalizedString) {
    .text = {
        "Remaining: 00",
        "Combo : 00",
        "れんさ：00",
        "Combo: 00"
    }
};


LessonScene * _Nonnull LessonSceneAlloc(uint32_t lessonIndex) {
    LessonScene *self = playdate->system->realloc(NULL, sizeof(LessonScene));

    const Lesson lesson = kLessons.memory[lessonIndex];

    CardList cards = CardListMakeWithInitialCapacity(DECK_CARD_COUNT);
    CardDeck deck = (CardDeck) {};
    unsigned int index = 0;
    for (; index < lesson.objectives.deckCount; index++) {
        const Card card = CardForCharacter(lesson.objectives.deck[index]);
        cards.memory[index] = CardDeckPickCard(&deck, card);
    }
    for (; index < DECK_CARD_COUNT; index++) {
        cards.memory[index] = CardDeckPickRandomCard(&deck);
    }
    cards.count = DECK_CARD_COUNT;
    CardDeckDeinit(&deck);

    *self = (LessonScene) {
        .super = {
            .type = SceneTypeLesson,
            .dealloc = dealloc,
            .init = init,
            .update = update,
            .sprites = LCDSpriteRefListEmpty,
        },
        .background = MELMapOpen(kMapNameFileNames[MapNameLesson]),
        .backgroundName = MapNameLesson,
        .cards = cards,
        .lessonIndex = lessonIndex,
        .speed = 1.0f,
    };
    return self;
}

#pragma mark - Fonctions privées

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeLesson) {
        playdate->system->error("Given scene is not a LessonScene instance");
        return;
    }
    LessonScene *self = (LessonScene *)scene;

    MELEventBusRemoveListeners(self);

    // Désallocation des sprites
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->background);

    GameFlowDeinit(&self->gameFlow);

    CardListDeinit(&self->cards);

    playdate->system->realloc(self, 0);
}

static void initializeBoard(LessonScene * _Nonnull self) {
    const Lesson lesson = kLessons.memory[self->lessonIndex];
    const char *cards = lesson.objectives.board;
    GameFlowPopulateWithCards(&self->gameFlow, cards, lesson.objectives.boardCount);
}

static LCDSprite * _Nonnull createObjectiveLabel(const MELLocalizedString * _Nonnull string, int value, int top) {
    char *text = MELStringCopy(string->text[currentLanguage]);
    const uint32_t length = (uint32_t) strlen(text);
    text[length - 1] = '0' + (value % 10);
    text[length - 2] = '0' + (value / 10);
    LCDSprite * sprite = TextConstructor(MELPointZero, currentFont, kDrawModeCopy, text, length);
    ImageSetOrigin(sprite, (MELPoint) { .x = 207, .y = top }, MELHorizontalAlignmentLeft, MELVerticalAlignmentTop);
    playdate->sprite->setZIndex(sprite, ZINDEX_DIALOG);
    playdate->system->realloc(text, 0);
    return sprite;
}

static void updateObjectiveLabel(LCDSprite * _Nonnull sprite, const MELLocalizedString * _Nonnull string, int value) {
    char *text = MELStringCopy(string->text[currentLanguage]);
    const uint32_t length = (uint32_t) strlen(text);
    text[length - 1] = '0' + (value % 10);
    text[length - 2] = '0' + (value / 10);
    TextSet(sprite, currentFont, kDrawModeCopy, text, length);
    playdate->system->realloc(text, 0);
}

static void init(MELScene * _Nonnull scene) {
    LessonScene *self = (LessonScene *)scene;

    // Création des couches du fond
    const MapName mapName = self->backgroundName;
    MELMap *background = self->background;
    for (unsigned int index = 0; index < background->layerCount; index++) {
        MELLayer *layer = background->layers + index;
        LCDBitmap *image = loadMapLayer(mapName, index);
        LCDSpriteRefListPush(&self->super.sprites, LayerSpriteConstructor(layer, image));
    }

    // Création des sprites du fond
    AnimationName doodle = 0;
    MELSpriteInstanceList instances = background->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        MELSpriteInstance instance = instances.memory[index];
        MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
        if (definition->palette == NULL) {
            definition->palette = SpriteNameLoadBitmapTable(instance.name);
        }
        MELSprite *melSprite = playdate->system->realloc(NULL, sizeof(MELSprite));
        LCDSprite *sprite = MELSpriteInit(melSprite, definition, instances.memory + index);
        LCDSpriteRefListPush(&self->super.sprites, sprite);

        playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
        playdate->sprite->setZIndex(sprite, ZINDEX_PORTAITS);

        if (instance.name == SpriteNameDoodle) {
            MELSpriteSetAnimation(melSprite, doodle++);
        } else if (instance.name == SpriteNameTodo) {
            MELSpriteSetAnimationAndDirection(melSprite, AnimationNameStand, (MELAnimationDirection)currentLanguage);
        }
    }

    // GameFlow
    GameFlowInit(&self->gameFlow, SideLeft, BoardAlloc(SideLeft), self->cards);
    self->gameFlow.actor = ActorMakePlayer();

    // Initialisation de la leçon
    const Lesson lesson = kLessons.memory[self->lessonIndex];
    self->progress = lesson.objectives;

    if (lesson.objectives.boardCount > 0) {
        initializeBoard(self);
    }

    int top = 60;
    if (lesson.objectives.cardsToRemove > 0) {
        self->cardsToRemoveLabel = createObjectiveLabel(&kCardsToRemove, lesson.objectives.cardsToRemove, top);
        top += playdate->graphics->getFontHeight(currentFont) + 4;

        MELEventBusAddListener(EventLeftSideRemovedCards, playerRemovedCards, self);
    }
    if (lesson.objectives.chipsToRemove > 0) {
        self->chipsToRemoveLabel = createObjectiveLabel(&kChipsToRemove, lesson.objectives.chipsToRemove, top);
        top += playdate->graphics->getFontHeight(currentFont) + 4;

        MELEventBusAddListener(EventLeftSideRemovedChips, playerRemovedChips, self);
    }
    if (lesson.objectives.chipsToSend > 0) {
        self->chipsToSendLabel = createObjectiveLabel(&kChipsToSend, lesson.objectives.chipsToSend, top);
        top += playdate->graphics->getFontHeight(currentFont) + 4;

        MELEventBusAddListener(EventLeftSideSentChips, playerSentChips, self);
    }
    if (lesson.objectives.comboToDo > 0) {
        self->comboToDoLabel = createObjectiveLabel(&kComboToDo, lesson.objectives.comboToDo, top);
        // NOTE: Pas d'ajout à "top" car il n'y a pas d'objectifs après celui-ci.

        MELEventBusAddListener(EventLeftSideMadeCombo, playerMadeCombo, self);
    }

    MELEventBusAddListener(EventLeftSideLost, playerLost, self);
    MELEventBusAddListener(EventLeftSideMayLose, playerMayLose, self);
    // TODO: Faire des événements lors de la suppression de cartes/jetons.

    // Première mise à jour pour initialiser les écoutes des événements.
    self->gameFlow.update(&self->gameFlow);
    // Envoi des jetons de la leçon
    if (lesson.objectives.chips > 0) {
        MELEventBusFireEvent(EventRightSideSentChips, lesson.objectives.chips);
    }

    MusicManagerPlay("musics/learning", 0, 0.25f);

    camera.frame.origin = MELPointZero;
    setRefreshRate(DEFAULT_REFRESH_RATE);
}

#pragma mark - Boucle de jeu

static int update(void * _Nonnull userdata) {
    LessonScene *self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    currentSaveGame.playtime += DELTA;

    // Gère les boutons et l'IA.
    ActorUpdate(self->gameFlow.actor);

    // Déplace et affiche les sprites.
    playdate->sprite->updateAndDrawSprites();

    // Identifie les changements dans le tableau de jeu.
    self->gameFlow.update(&self->gameFlow);

    return true;
}

static int updateLessonSuccess(void * _Nonnull userdata) {
    LessonScene *self = userdata;

    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    if (self->time < 1.0f) {
        self->time += DELTA;
        return true;
    }

    // TODO: Faire une animation et jouer un son
    if (MELControllerMake().pressedA) {
        LessonSelectScene *lessonSelectScene = LessonSelectSceneAlloc();
        MELSceneMakeCurrent(&lessonSelectScene->super);
    }
    return true;
}

static int updateLessonFailed(void * _Nonnull userdata) {
    LessonScene *self = userdata;
    DELTA = MELFloatMax(playdate->system->getElapsedTime(), DEFAULT_FRAME_TIME);
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();

    if (self->time < 1.0f) {
        self->time += DELTA;
        return true;
    }

    // TODO: Faire une animation et jouer un son
    MELController controller = MELControllerMake();
    if (controller.pressedA) {
        LessonScene *lessonScene = LessonSceneAlloc(self->lessonIndex);
        MELSceneMakeCurrent(&lessonScene->super);
    }
    else if (controller.pressedB) {
        LessonSelectScene *lessonSelectScene = LessonSelectSceneAlloc();
        MELSceneMakeCurrent(&lessonSelectScene->super);
    }
    return true;
}

static void lessonSuccess(LessonScene * _Nonnull self) {
    GameFlowSetState(&self->gameFlow, GameFlowStateWon);
    WinLoseMessageConstructorWithBoard(self->gameFlow.board, true);
    CardSpriteRemoveMainAndExtra();

    currentSaveGame.schoolProgression = currentSaveGame.schoolProgression | (1 << self->lessonIndex);
    SaveGameToDisk(&currentSaveGame);

    MusicManagerPlay("musics/masterful", 1, 0.1f);

    playdate->system->setUpdateCallback(updateLessonSuccess, self);
}

static void playerLost(void * _Nullable userdata, int _) {
    // TODO: Afficher un logo FAILURE et faire un jingle.
    playdate->system->setUpdateCallback(updateLessonFailed, userdata);
}

static void playerMayLose(void * _Nullable userdata, int mayLose) {
    MusicManagerPlay(mayLose ? "musics/pinch" : "musics/learning", 0, 0.25f);
}

static uint8_t safeSubstract(uint8_t current, int valueToSubstract) {
    return valueToSubstract < current ? current - valueToSubstract : 0;
}

static void playerRemovedCards(void * _Nullable userdata, int count) {
    LessonScene *self = userdata;
    const int remaining = self->progress.cardsToRemove = safeSubstract(self->progress.cardsToRemove, count);
    updateObjectiveLabel(self->cardsToRemoveLabel, &kCardsToRemove, remaining);

    if (LessonObjectivesAreEmpty(self->progress)) {
        lessonSuccess(self);
    }
}

static void playerRemovedChips(void * _Nullable userdata, int count) {
    LessonScene *self = userdata;
    const int remaining = self->progress.chipsToRemove = safeSubstract(self->progress.chipsToRemove, count);
    updateObjectiveLabel(self->chipsToRemoveLabel, &kChipsToRemove, remaining);

    if (LessonObjectivesAreEmpty(self->progress)) {
        lessonSuccess(self);
    }
}

static void playerSentChips(void * _Nullable userdata, int count) {
    LessonScene *self = userdata;
    const int remaining = self->progress.chipsToSend = safeSubstract(self->progress.chipsToSend, count);
    updateObjectiveLabel(self->chipsToSendLabel, &kChipsToSend, remaining);

    if (LessonObjectivesAreEmpty(self->progress)) {
        lessonSuccess(self);
    }
}

static void playerMadeCombo(void * _Nullable userdata, int value) {
    LessonScene *self = userdata;
    const Lesson *lesson = kLessons.memory + self->lessonIndex;
    const int remaining = self->progress.comboToDo = safeSubstract(lesson->objectives.comboToDo, value);
    updateObjectiveLabel(self->comboToDoLabel, &kComboToDo, remaining);

    if (LessonObjectivesAreEmpty(self->progress)) {
        lessonSuccess(self);
    }
}
