//
//  titlescene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 06/03/2023.
//

#include "titlescene.h"

#include "text.h"
#include "samples.h"
#include "music.h"
#include "layersprite.h"
#include "story.h"
#include "storyselectscene.h"
#include "lessons.h"
#include "lessonselectscene.h"
#include "attractmodescene.h"
#include "openingscene.h"
#include "scoreattackselectscene.h"
#include "rallyscorescene.h"
#include "image.h"
#include "selectfile_card.h"
#include "selectfile_flying_card.h"
#include "selectfile_ceres.h"
#include "stride.h"
#include "scores.h"
#include "achievements.h"

#include "../gen/palettenames.h"
#include "../gen/spritenames.h"
#include "../gen/maps.h"
#include "../gen/spritecards18.h"
#include "../gen/spritetitlesparks.h"
#include "../gen/spritemenuitems.h"
#include "../gen/spritemenucursor.h"
#include "../gen/spritestoryclear.h"

typedef enum {
    TitleScreenTitle,
    TitleScreenSaveSelect,
    TitleScreenMenu
} TitleScreen;

static PDMenuItem * _Nullable deleteSaveGameMenuItem;
static PDMenuItem * _Nullable pickCardMenuItem;

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull self);

static LCDSprite * _Nonnull createSprite(MELSpriteInstance * _Nonnull instanceRef);
static void createMenuGrid(TitleScene * _Nonnull self);

static int updateTitle(void * _Nonnull userdata);
static int updateFileSelectIntro(void * _Nonnull userdata);
static int updateFileSelect(void * _Nonnull userdata);
static int updateMenu(void * _Nonnull userdata);

static void updateLogo(LCDSprite * _Nonnull sprite);
static void updatePressA(LCDSprite * _Nonnull sprite);
static void updateSparks(LCDSprite * _Nonnull sprite);

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected);
static void onLanguageChanged(void * _Nonnull userdata, int language);

static void checkForAlreadyUnlockedAchievements(SaveGame * _Nonnull saveGame);

#if TITLE_SCREEN_CARD_FOUNTAIN
static void updateCard(LCDSprite * _Nonnull sprite);
#endif

static const int kMenuScreenLeft = LCD_COLUMNS + LCD_COLUMNS;

static const float kTimeBeforeAttractMode = 15.0f;
static SceneType nextSceneIfIdle = SceneTypeAttractMode;

#pragma mark - Constructeur

TitleScene * _Nonnull TitleSceneAlloc(void) {
    TitleScene *self = playdate->system->realloc(NULL, sizeof(TitleScene));
    *self = (TitleScene) {
        .super = {
            .type = SceneTypeTitle,
            .init = init,
            .dealloc = dealloc,
            // NOTE: Mettre à jour cette valeur à chaque changement de méthode "update" ?
            .update = updateTitle,
            .sprites = LCDSpriteRefListEmpty,
        },
        .map = MELMapOpen(kMapNameFileNames[MapNameTitle]),
        .selectedSaveGame = -1,
    };
    MELEventBusAddListener(EventLanguageChanged, onLanguageChanged, self);
    return self;
}

static void init(MELScene * _Nonnull scene) {
    TitleScene *self = (TitleScene *)scene;
    MELMap *map = self->map;

    OnlineScoresLoad();

    const MELBoolean saveGameAlreadySelected = currentSaveGame.index != kNoSaveSelected;
    if (saveGameAlreadySelected) {
        // Une sauvegarde est déjà chargée, sauvegarde et préparation de l'écran.
        SaveGameToDisk(&currentSaveGame);
        self->selectedSaveGame = currentSaveGame.index;
        camera.frame.origin = (MELPoint) { .x = kMenuScreenLeft };
        playdate->system->setUpdateCallback(updateMenu, self);
    } else {
        camera.frame.origin = MELPointZero;
    }

    createMenuGrid(self);

    spriteTitleSparks.animations[0]->type = MELAnimationTypeSynchronized;
    spriteTitleSparks.animations[MELAnimationDirectionCount]->type = MELAnimationTypeSynchronized;

    MELSprite *dove = NULL;

    AnimationName sparkAnimation = AnimationNameStand;
    MELSpriteInstanceList instances = map->instances;
    for (unsigned int index = 0; index < instances.count; index++) {
        const MELSpriteInstance instance = instances.memory[index];
        LCDSprite *sprite = createSprite(instances.memory + index);
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        switch (instance.name) {
            case SpriteNameTitleSparks:
                MELSpriteSetAnimation(melSprite, sparkAnimation++);
                playdate->sprite->setUpdateFunction(sprite, updateSparks);
                if (saveGameAlreadySelected) {
                    MELSpriteSetAnimation(melSprite, melSprite->animationName + 2);
                    MELPlayOnceAnimationSkip(melSprite->animation);
                }
                break;
            case SpriteNameTitle:
                playdate->sprite->setUpdateFunction(sprite, updateLogo);
                if (saveGameAlreadySelected) {
                    melSprite->frame.origin = (MELPoint) {
                        .x = melSprite->frame.origin.x,
                        .y = melSprite->frame.size.height / 2.0f
                    };
                }
                break;
            case SpriteNameTitleMagicHat:
                if (saveGameAlreadySelected) {
                    melSprite->frame.origin =(MELPoint) {
                        .x = 486.0f,
                        .y = 155.0f
                    };
                    MELSpriteSetAnimation(melSprite, AnimationNameWalk);
                    MELPlayOnceAnimationSkip(melSprite->animation);
                    playdate->sprite->setVisible(sprite, false);
                }
                break;
            case SpriteNameTitlePressA:
                playdate->sprite->setUpdateFunction(sprite, updatePressA);
                break;
            case SpriteNameFileSelectCaroline:
                self->fileSelectCaroline = melSprite;
                melSprite->frame.origin.x += kCarolineLeftPadding;
                playdate->sprite->setZIndex(sprite, 2);
                if (saveGameAlreadySelected) {
                    MELSpriteSetAnimation(melSprite, AnimationNameWalk);
                    MELPlayOnceAnimationSkip(melSprite->animation);
                }
                break;
            case SpriteNameMenuDove:
                dove = melSprite;
                SelectFileCeresConfigure(sprite, &self->menuGrid->selection.y);
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
    SelectFileCeresSetInitialPosition(dove);

    if (!fontNontendoBold) {
        fontNontendoBold = LCDFontLoadOrError("fonts/Nontendo-Bold");
    }
    const MELMetadata metadata = MELMetadataGet();
    const uint32_t length = (uint32_t)strlen(metadata.version);
    char version[length + 2];
    version[0] = 'v';
    memcpy(version + 1, metadata.version, length);
    version[length + 1] = '\0';
    LCDSprite * _Nonnull versionNumber = TextConstructor(MELPointZero, fontNontendoBold, kDrawModeCopy, version, 20);
    MELAlignmentSet(versionNumber, MELHorizontalAlignmentLeft, MELVerticalAlignmentBottom, MELPointMake(10, LCD_ROWS - 8));

    MusicManagerPlay("musics/title", 0, 0.0f);
    MELSynchronizedLoopingAnimationReset();

    setRefreshRate(DEFAULT_REFRESH_RATE);
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeTitle) {
        playdate->system->error("Given scene is not a TitleScene instance");
        return;
    }
    TitleScene *self = (TitleScene *)scene;
    MELEventBusRemoveListeners(self);
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    MELMapDealloc(self->map);
    SaveGameListDeinit(&self->saveGames);
    if (spriteMenuItems.palette) {
        playdate->graphics->freeBitmapTable(spriteMenuItems.palette);
        spriteMenuItems.palette = NULL;
    }
    if (spriteMenuCursor.palette) {
        playdate->graphics->freeBitmapTable(spriteMenuCursor.palette);
        spriteMenuCursor.palette = NULL;
    }
    playdate->system->realloc(self, 0);
}

/**
 * Créé un nouveau sprite pour l'instance donnée. Fait plus ou moins la même chose que MELSpriteInit mais sans initialiser la hitbox et en définissant MELSpriteUpdate comme méthode d'update.
 *
 *  @param instanceRef Instance parente du sprite.
 *  @returns Un pointeur vers un nouveau LCDSprite.
 */
static LCDSprite * _Nonnull createSprite(MELSpriteInstance * _Nonnull instanceRef) {
    MELSpriteInstance instance = *instanceRef;
    MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
    if (!definition->palette) {
        definition->palette = SpriteNameLoadBitmapTable(instance.name);
    }

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
        .instance = instanceRef,
    };
    MELSpriteSetAnimation(melSprite, AnimationNameStand);

    playdate->sprite->setUserdata(sprite, melSprite);
    playdate->sprite->setUpdateFunction(sprite, &MELSpriteUpdate);
    playdate->sprite->addSprite(sprite);
    return sprite;
}

static void createMenuGrid(TitleScene * _Nonnull self) {
    const int left = 25;
    const int top = 25;
    const int bottom = top;
    const int cursorWidth = spriteMenuCursor.size.width + 2;
    const int cellWidth = spriteMenuItems.size.width + cursorWidth + spriteStoryClear.size.width;
    const int cellHeight = spriteMenuItems.size.height;
    LCDSprite *sprite = MELGridViewMake((MELRectangle) {
        .origin = {
            .x = LCD_COLUMNS + LCD_COLUMNS + left + cellWidth / 2,
            .y = LCD_ROWS / 2
        },
        .size = {
            .width = cellWidth,
            .height = LCD_ROWS - top - bottom
        }
    }, MELIntSizeMake(1, TitleMenuItemCount), MELIntSizeMake(cellWidth, cellHeight), drawCell, self);
    playdate->sprite->setZIndex(sprite, 1);

    MELGridView *gridView = playdate->sprite->getUserdata(sprite);
    gridView->cellSpacing = MELIntSizeMake(0, 20);
    gridView->disableInputs = true;
    self->menuGrid = gridView;
    self->menuGridSprite = sprite;
    LCDSpriteRefListPush(&self->super.sprites, sprite);

    MELGridViewSetSelection(sprite, (MELIntPoint) { .y = self->initialMenuSelection });
}

#pragma mark - Translation

static TitleScreen getTitleScreen(void) {
    return (TitleScreen) ((int)camera.frame.origin.x/LCD_COLUMNS);
}

static int updateTranslation(void * _Nonnull userdata) {
    TitleScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    const float duration = 0.5f;
    const float time = self->time += DELTA;
    const float progress = MELEaseInOut(0.0f, duration, time);
    camera.frame.origin.x = self->from + (self->to - self->from) * progress;

    playdate->sprite->updateAndDrawSprites();

    if (progress == 1.0f) {
        self->time = 0.0f;

        switch (getTitleScreen()) {
            case TitleScreenSaveSelect:
                currentSaveGame = (SaveGame) {
                    .index = kNoSaveSelected,
                };
                playdate->system->setUpdateCallback(updateFileSelectIntro, self);
                break;
            case TitleScreenMenu:
                checkForAlreadyUnlockedAchievements(&currentSaveGame);
                playdate->system->setUpdateCallback(updateMenu, self);
                break;
            default:
                playdate->system->setUpdateCallback(updateTitle, self);
                break;
        }
    }

    return true;
}

static void translateWithDirection(TitleScene * _Nonnull self, MELDirection direction) {
    self->time = 0.0f;
    self->from = camera.frame.origin.x;
    self->to = self->from + MELDirectionValues[direction] * LCD_COLUMNS;
    playdate->system->setUpdateCallback(updateTranslation, self);
}

#pragma mark - Titre

static void createCards(TitleScene * _Nonnull self, MELBoolean skipAnimation) {
    const int cardCount = MELIntMin(self->saveGames.count + 1, kMaximumSaveCount);
    for (unsigned int index = 0; index < cardCount; index++) {
        LCDSprite *sprite = SelectFileCardConstructor((MELRectangle) {
            .origin = {
                .x = 600 + kCarolineLeftPadding,
                .y = 185
            },
            .size = {
                .width = 200,
                .height = 40
            }
        }, index, cardCount, &self->selectedSaveGame);
        if (skipAnimation) {
            SelectFileCardsSkipAnimation(sprite);
        }
    }
}

static void clearAndRecreateCards(TitleScene * _Nonnull self) {
    SavedGameLoad(&self->saveGames);

    LCDSprite *flyingCard = NULL;
    LCDSpriteRefList sprites = self->super.sprites;
    for (int index = sprites.count - 1; index >= 0; index--) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        switch (melSprite->class->name) {
            case SpriteClassNameSelectFileCard:
                melSprite->class->destroy(sprite);
                break;
            case SpriteClassNameSelectFileFlyingCard:
                flyingCard = sprite;
                SelectFileFlyingCardSetSaveGamesAndSelectedGame(flyingCard, &self->saveGames, &self->selectedSaveGame);
                break;
            default:
                break;
        }
    }
    if (flyingCard == NULL) {
        SelectFileFlyingCardConstructor(&self->saveGames, &self->selectedSaveGame);
    }
    createCards(self, true);
}

static int updateTitle(void * _Nonnull userdata) {
    TitleScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();
#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    SampleLoadNextSample();

    float time = self->time + DELTA;
#if TITLE_SCREEN_CARD_FOUNTAIN
    const float duration = 0.1f;
    if (time > duration) {
        time -= duration;

        const Card card = CardDeckPickRandomCard(&self->cards);
        LCDSprite *cardSprite = ImageConstructor((MELPoint) {
            .x = 200,
            .y = 180,
        }, CardGetBitmap(card));
        MELSprite *melSprite = playdate->sprite->getUserdata(cardSprite);
        melSprite->frame.size = (MELSize) {
            .width = 100.0f - MELRandomFloat(200.0f),
            .height = -300.0f - MELRandomFloat(300.0f),
        };
        playdate->sprite->setZIndex(cardSprite, 2);
        playdate->sprite->setUpdateFunction(cardSprite, updateCard);
    }
#endif
    self->time = time;

    PDButtons pressed, pressing;
    playdate->system->getButtonState(&pressing, &pressed, NULL);
    // Début du jeu
    if (pressed & kButtonA) {
        LCDSprite *flyingCard = MELSceneFindSpriteByClassName(SpriteClassNameSelectFileFlyingCard);
        if (flyingCard == NULL) {
            MELAchievementLoadStatus(kAchievementData);
            SavedGameLoad(&self->saveGames);
            SelectFileFlyingCardConstructor(&self->saveGames, &self->selectedSaveGame);
            createCards(self, false);
        } else {
            SelectFileCardsReset();
            SelectFileFlyingCardSetSaveGamesAndSelectedGame(flyingCard, &self->saveGames, &self->selectedSaveGame);
        }
        MELSpriteSetAnimation(self->fileSelectCaroline, AnimationNameStand);
        LCDSpriteRefList sprites = self->super.sprites;
        for (unsigned int index = 0; index < sprites.count; index++) {
            LCDSprite *sprite = sprites.memory[index];
            MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
            MELSpriteInstance *instance = melSprite->instance;
            if (instance != NULL && instance->name == SpriteNameTitleSparks) {
                // Masque les étoiles
                MELSpriteSetAnimation(melSprite, melSprite->animationName + 2);
            } else if (instance != NULL && instance->name == SpriteNameTitle) {
                // Masque le titre
                StrideSpriteTo(sprite, (MELPoint) {
                    .x = melSprite->frame.origin.x,
                    .y = melSprite->frame.size.height / 2.0f
                }, 0.0f, 0.75f);
            } else if (instance != NULL && instance->name == SpriteNameTitleMagicHat) {
                // Déplace le chapeau vers Caroline
                StrideSpriteTo(sprite, (MELPoint) {
                    .x = 486.0f + kCarolineLeftPadding,
                    .y = 155.0f
                }, 0.0f, 0.5f);
                MELSpriteSetAnimation(melSprite, AnimationNameWalk);
            }
        }
        translateWithDirection(self, MELDirectionRight);
    } else if (time >= kTimeBeforeAttractMode) {
        if (nextSceneIfIdle == SceneTypeAttractMode) {
            nextSceneIfIdle = SceneTypeOpening;
            AttractModeScene *attractModeScene = AttractModeSceneAlloc();
            MELSceneFadeTo(&attractModeScene->super, MELFadeToBlackSceneAlloc);
        } else {
            nextSceneIfIdle = SceneTypeAttractMode;
            OpeningScene *openingScene = OpeningSceneAlloc();
            MELSceneMakeCurrent(&openingScene->super);
        }
    }

    return true;
}

static void updateLogo(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    const unsigned int time = playdate->system->getCurrentTimeMilliseconds();

    MELRectangle frame = self->frame;
    frame.origin.y = self->instance->center.y + sinf((time / 2000.f) * MEL_PI) * 4.0f;
    self->frame = frame;

    MELSpriteUpdate(sprite);
}

#if TITLE_SCREEN_CARD_FOUNTAIN
static void updateCard(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = self->frame;
    frame.size.height += DELTA * 600.0f;
    frame.origin.x += frame.size.width * DELTA;
    frame.origin.y += frame.size.height * DELTA;
    self->frame = frame;

    if (!MELRectangleIntersectsWithRectangle(MELScreen, (MELRectangle) {
        .origin = frame.origin,
        .size = (MELSize) {
            .width = spriteCards18.size.width,
            .height = spriteCards18.size.height,
        }
    })) {
        self->class->destroy(sprite);
        return;
    }

    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
}
#endif

static void updatePressA(LCDSprite * _Nonnull sprite) {
    const unsigned int time = playdate->system->getCurrentTimeMilliseconds();
    playdate->sprite->setVisible(sprite, time % 1500 < 750);
    MELSpriteUpdate(sprite);
}

static void updateSparks(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELSpriteDraw(self, sprite);

    if (self->animationName >= AnimationNameJump && MELAnimationIsLastFrame(self->animation)) {
        MELSpriteSetAnimation(self, self->animationName - AnimationNameJump);
    }
}


#pragma mark - Choix de la sauvegarde

static void backFromFileSelectToTitle(TitleScene * _Nonnull self) {
    AnimationName spark = AnimationNameJump;
    LCDSpriteRefList sprites = self->super.sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        MELSpriteInstance *instance = melSprite->instance;
        if (instance != NULL && instance->name == SpriteNameTitleSparks) {
            MELSpriteSetAnimation(melSprite, spark);
            spark++;
        } else if (instance != NULL && instance->name == SpriteNameTitle) {
            StrideSpriteTo(sprite, instance->center, 0.0f, 0.75f);
        } else if (instance != NULL && instance->name == SpriteNameTitleMagicHat) {
            StrideSpriteTo(sprite, instance->center, 0.0f, 0.5f);
            MELSpriteSetAnimation(melSprite, AnimationNameStand);
            playdate->sprite->setVisible(sprite, true);
        }
    }
    if (self->selectedSaveGame >= 0) {
        self->selectedSaveGame = -self->selectedSaveGame - 1;
    }
    if (self->saveGameTitle) {
        MELSpriteDealloc(self->saveGameTitle);
        MELSpriteDealloc(self->saveGameDescription);
        self->saveGameTitle = NULL;
        self->saveGameDescription = NULL;
    }
    translateWithDirection(self, MELDirectionLeft);
}

static void deleteSaveGame(void * _Nullable userdata) {
    TitleScene *self = userdata;
    // TODO: Afficher une popup indiquant "really delete save X?"
    SaveGameDelete(&self->saveGames, self->selectedSaveGame);
    if (self->selectedSaveGame >= self->saveGames.count) {
        self->selectedSaveGame = self->saveGames.count - 1;
    }
    clearAndRecreateCards(self);
    if (self->saveGameTitle) {
        MELSprite *saveGameTitle = playdate->sprite->getUserdata(self->saveGameTitle);
        saveGameTitle->class->destroy(self->saveGameTitle);
        self->saveGameTitle = NULL;
    }
}

/**
 * Change la carte de la sauvegarde sélectionnée.
 */
static void pickCardAgain(void * _Nullable userdata) {
    TitleScene *self = userdata;
    CardDeck deck = (CardDeck) {};
    for (unsigned int index = 0; index < self->saveGames.count; index++) {
        CardDeckPickCard(&deck, self->saveGames.memory[index].card);
    }
    const int selection = self->selectedSaveGame;
    self->saveGames.memory[selection].card = CardDeckPickRandomCard(&deck);
    SaveGameToDisk(self->saveGames.memory + selection);
    CardDeckDeinit(&deck);
}

static void set2DigitNumber(uint8_t value, char * _Nonnull buffer) {
    buffer[0] = '0' + value / 10;
    buffer[1] = '0' + value % 10;
}
static void set4DigitNumber(unsigned int value, char * _Nonnull buffer) {
    if (value < 10) {
        buffer[0] = '0' + value;
        buffer[1] = ' ';
        buffer[2] = ' ';
        buffer[3] = ' ';
    } else if (value < 100) {
        buffer[0] = '0' + value / 10;
        buffer[1] = '0' + value % 10;
        buffer[2] = ' ';
        buffer[3] = ' ';
    } else if (value < 1000) {
        buffer[0] = '0' + value / 100;
        buffer[1] = '0' + (value / 10) % 10;
        buffer[2] = '0' + value % 10;
        buffer[3] = ' ';
    } else if (value < 10000) {
        buffer[0] = '0' + value / 1000;
        buffer[1] = '0' + (value / 100) % 10;
        buffer[2] = '0' + (value / 10) % 10;
        buffer[3] = '0' + value % 10;
    } else {
        buffer[0] = '9';
        buffer[1] = '9';
        buffer[2] = '9';
        buffer[3] = '9';
    }
}

static void setNumber(uint32_t value, unsigned int digitCount, char * _Nonnull buffer) {
    const unsigned int length = MELStringLengthToDisplayUInt(value);
    if (length > digitCount) {
        for (int index = 0; index < digitCount; index++) {
            buffer[index] = '9';
        }
        return;
    }
    for (int index = length - 1; index >= 0; index--) {
        buffer[index] = '0' + (value % 10);
        value /= 10;
    }
    for (int index = length; index < digitCount; index++) {
        buffer[index] = ' ';
    }
}

static void setPlayTime(const float playTime, char * _Nonnull buffer) {
    uint32_t time = (uint32_t) playTime;
    uint32_t hours = time / (60 * 60);
    uint8_t seconds = hours > 99 ? 59 : (time % 60);
    uint8_t minutes = hours > 99 ? 59 : ((time / 60) % 60);
    hours = MELIntMin(hours, 99);

    static const int hoursStart = 0;
    static const int minutesStart = 3;
    static const int secondsStart = 6;

    set2DigitNumber(hours, buffer + hoursStart);
    set2DigitNumber(minutes, buffer + minutesStart);
    set2DigitNumber(seconds, buffer + secondsStart);
}

static int updateFileSelectIntro(void * _Nonnull userdata) {
    TitleScene *self = userdata;
    LCDSprite *hat;

    switch (self->fileSelectCaroline->animationName) {
        default:
        case AnimationNameStand:
            // Caroline a le bras tendu pour attraper le chapeau.
            hat = MELSceneFindSpriteByName(SpriteNameTitleMagicHat);
            playdate->sprite->setVisible(hat, false);
            MELSpriteSetAnimation(self->fileSelectCaroline, AnimationNameRun);
            break;
        case AnimationNameRun:
            // Caroline met le chapeau sur sa tête.
            if (MELAnimationIsLastFrame(self->fileSelectCaroline->animation)) {
                MELSpriteSetAnimation(self->fileSelectCaroline, AnimationNameWalk);
                SelectFileCardsSlide();
            }
            break;
        case AnimationNameWalk:
            // Caroline étale les cartes.
            if (MELAnimationIsLastFrame(self->fileSelectCaroline->animation)) {
                if (self->selectedSaveGame < 0) {
                    self->selectedSaveGame = -self->selectedSaveGame - 1;
                }
                if (self->saveGameTitle) {
                    MELSpriteDealloc(self->saveGameTitle);
                    self->saveGameTitle = NULL;
                }
                playdate->system->setUpdateCallback(updateFileSelect, self);
                updateFileSelect(self);
            }
            break;
    }

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();
#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    SampleLoadNextSample();
    PDButtons pressed, pressing;
    playdate->system->getButtonState(&pressing, &pressed, NULL);
    // Début du jeu
    if (pressed & kButtonA) {
        LCDSpriteRefList sprites = self->super.sprites;
        for (unsigned int index = 0; index < sprites.count; index++) {
            LCDSprite *sprite = sprites.memory[index];
            MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
            if (melSprite->class->name == SpriteClassNameSelectFileCard) {
                SelectFileCardsSkipAnimation(sprite);
            }
        }
        MELSpriteSetAnimation(self->fileSelectCaroline, AnimationNameWalk);
        MELPlayOnceAnimationSkip(self->fileSelectCaroline->animation);
    } else if (pressed & kButtonB) {
        backFromFileSelectToTitle(self);
    }

    return true;
}

static void checkForAlreadyUnlockedAchievements(SaveGame * _Nonnull saveGame) {
    for (unsigned int index = 0; index < AchievementCount; index++) {
        const uint32_t unlockDate = saveGame->achievementUnlockDate[index];
        if (unlockDate) {
            unlockAchievementAt((Achievement)index, unlockDate);
        }
    }
    MELAchievementSaveStatus();
}

static int updateFileSelect(void * _Nonnull userdata) {
    TitleScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();
#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    SampleLoadNextSample();

    const int8_t oldSelection = self->selectedSaveGame;
    int8_t selection = oldSelection;

    PDButtons pressed, pressing;
    playdate->system->getButtonState(&pressing, &pressed, NULL);
    if (pressed & kButtonA) {
        const MELBoolean isNewGame = selection == self->saveGames.count;
        if (isNewGame) {
            SaveGameCreate(&self->saveGames);
        }
        currentSaveGame = self->saveGames.memory[selection];
        setLanguage(currentSaveGame.language);

        // Menu
        MELGridViewSetSelection(self->menuGridSprite, MELIntPointZero);
        MELPoint target = self->menuGrid->super.frame.origin;
        self->menuGrid->super.frame.origin.y += 64;
        StrideSpriteTo(self->menuGridSprite, target, 0.1f, 0.4f);
        StrideSpriteFromAndTo(MELSceneFindSpriteByName(SpriteNameMenuCiel), (MELPoint) {
            .x = 800 + 200,
            .y = LCD_ROWS / 2
        }, (MELPoint) {
            .x = 800 + 200 + 160,
            .y = LCD_ROWS / 2
        }, 0.0f, 1.0f);

        // Retire l'option de suppression
        if (deleteSaveGameMenuItem) {
            playdate->system->removeMenuItem(deleteSaveGameMenuItem);
            deleteSaveGameMenuItem = NULL;
        }
        if (pickCardMenuItem) {
            playdate->system->removeMenuItem(pickCardMenuItem);
            pickCardMenuItem = NULL;
        }

        // Va vers le menu principal
        MELGridViewRepaint(self->menuGridSprite);
        translateWithDirection(self, MELDirectionRight);
        return true;
    } else if (pressed & kButtonB) {
        // Retire l'option de suppression
        if (deleteSaveGameMenuItem) {
            playdate->system->removeMenuItem(deleteSaveGameMenuItem);
            deleteSaveGameMenuItem = NULL;
        }
        if (pickCardMenuItem) {
            playdate->system->removeMenuItem(pickCardMenuItem);
            pickCardMenuItem = NULL;
        }
        // Retourne à l'écran de titre
        backFromFileSelectToTitle(self);
        return true;
    } else if (pressed & kButtonLeft) {
        selection = MELIntMax(selection - 1, 0);
    } else if (pressed & kButtonRight) {
        selection = MELIntMin(selection + 1, MELIntMin(self->saveGames.count, 4));
    }

    const MELBoolean selectionChanged = selection != oldSelection || self->saveGameTitle == NULL;

    if (deleteSaveGameMenuItem == NULL && selection < self->saveGames.count) {
        char message[] = "delete save 1";
        message[12] += selection;
        deleteSaveGameMenuItem = playdate->system->addMenuItem(message, deleteSaveGame, self);
        pickCardMenuItem = playdate->system->addMenuItem("change card", pickCardAgain, self);
    } else if (deleteSaveGameMenuItem != NULL && selectionChanged && selection < self->saveGames.count) {
        char message[] = "delete save 1";
        message[12] += selection;
        playdate->system->setMenuItemTitle(deleteSaveGameMenuItem, message);
    } else if (deleteSaveGameMenuItem != NULL && selectionChanged) {
        playdate->system->removeMenuItem(deleteSaveGameMenuItem);
        deleteSaveGameMenuItem = NULL;

        playdate->system->removeMenuItem(pickCardMenuItem);
        pickCardMenuItem = NULL;
    }

    if (selectionChanged) {
        const MELBoolean isNewGame = selection == self->saveGames.count;
        if (self->saveGameTitle == NULL) {
            self->saveGameTitle = TextConstructor(MELPointMake(410.0f, 30.0f), fontMagic, kDrawModeCopy, "", 0);
        }
        if (self->saveGameDescription == NULL) {
            self->saveGameDescription = TextConstructor(MELPointMake(410.0f, 60.0f), fontRoobert10Bold, kDrawModeCopy, "", 0);
        }
        if (!isNewGame) {
            const SaveGame *saveGame = self->saveGames.memory + selection;
           
            switch (saveGame->language) {
                case MELLanguageEnglish: {
                    char saveTitle[] = "SAVE 1";
                    saveTitle[5] = '1' + selection;
                    TextSet(self->saveGameTitle, fontMagic, kDrawModeCopy, saveTitle, 99);

                    char description[] = "Play time: 00:00:00\nWins: 0000\nLosses: 0000\nChips sent: 000000\nBest combo: 0000\nScore Attack: 00000000\nLanguage: English\n\nⒶ Select\nⓞ Options";
                    setPlayTime(saveGame->playtime, description + 11);
                    set4DigitNumber(saveGame->wins, description + 26);
                    set4DigitNumber(saveGame->loses, description + 39);
                    setNumber(saveGame->sentChips, 6, description + 56);
                    set4DigitNumber(saveGame->bestCombo, description + 75);
                    setNumber(saveGame->scoreAttackPersonalBest, 8, description + 94);
                    TextSetWithLeading(self->saveGameDescription, fontRoobert10Bold, kDrawModeCopy, 4, description, 200);
                } break;
                case MELLanguageFrench: {
                    char saveTitle[] = "SAUVEGARDE 1";
                    saveTitle[11] = '1' + selection;
                    TextSet(self->saveGameTitle, fontMagic, kDrawModeCopy, saveTitle, 99);

                    char description[] = "Temps de jeu : 00:00:00\nVictoires : 0000\nDéfaites : 0000\nJetons envoyés : 000000\nCombo max : 0000\nScore Attack: 00000000\nLangue : français\n\nⒶ Sélectionner\nⓞ Options";
                    setPlayTime(saveGame->playtime, description + 15);
                    set4DigitNumber(saveGame->wins, description + 36);
                    set4DigitNumber(saveGame->loses, description + 53);
                    setNumber(saveGame->sentChips, 6, description + 76);
                    set4DigitNumber(saveGame->bestCombo, description + 95);
                    setNumber(saveGame->scoreAttackPersonalBest, 8, description + 114);
                    TextSetWithLeading(self->saveGameDescription, fontRoobert10Bold, kDrawModeCopy, 4, description, 200);
                } break;
                case MELLanguageJapanese: {
                    char saveTitle[] = "セーブ 1";
                    saveTitle[10] = '1' + selection;
                    TextSet(self->saveGameTitle, fontMagic, kDrawModeCopy, saveTitle, 99);

                    char description[] = "プレイ時間：00:00:00\n勝利数：0000\n敗北数：0000\n送ったチップ：000000\n最大連鎖：0000\nスコアアタック：00000000\n言語：日本語\n\nⒶ 選択\nⓞ オプション";
                    setPlayTime(saveGame->playtime, description + 18);
                    set4DigitNumber(saveGame->wins, description + 39);
                    set4DigitNumber(saveGame->loses, description + 56);
                    setNumber(saveGame->sentChips, 6, description + 82);
                    set4DigitNumber(saveGame->bestCombo, description + 104);
                    setNumber(saveGame->scoreAttackPersonalBest, 8, description + 133);
                    TextSetWithLeading(self->saveGameDescription, fontJfDotK14Bold, kDrawModeCopy, 4, description, 200);
                } break;
                case MELLanguageBrazilianPortuguese: {
                    char saveTitle[] = "JOGO 1";
                    saveTitle[5] = '1' + selection;
                    TextSet(self->saveGameTitle, fontMagic, kDrawModeCopy, saveTitle, 99);

                    char description[] = "Tempo de jogo: 00:00:00\nVitórias: 0000\nDerrotas: 0000\nFichas enviadas: 000000\nMaior combo: 0000\nScore Attack: 00000000\nIdioma: Português (Brasil)\n\nⒶ Selecionar\nⓞ Opções";
                    setPlayTime(saveGame->playtime, description + 15);
                    set4DigitNumber(saveGame->wins, description + 35);
                    set4DigitNumber(saveGame->loses, description + 50);
                    setNumber(saveGame->sentChips, 6, description + 72);
                    set4DigitNumber(saveGame->bestCombo, description + 92);
                    setNumber(saveGame->scoreAttackPersonalBest, 8, description + 111);
                    TextSetWithLeading(self->saveGameDescription, fontRoobert10Bold, kDrawModeCopy, 4, description, 200);
                } break;
            }
            
        } else {
            switch (currentLanguage) {
                case MELLanguageEnglish: {
                    static const char newGame[] = "NEW GAME";
                    static const char description[] = "Press Ⓐ to pick a card\nand create a new game.\n\nLanguage: English\n\nⓞ Options";
                    TextSet(self->saveGameTitle, fontMagic, kDrawModeCopy, newGame, 99);
                    TextSet(self->saveGameDescription, fontRoobert10Bold, kDrawModeCopy, description, 99);
                } break;
                case MELLanguageFrench: {
                    static const char newGame[] = "NOUVELLE PARTIE";
                    static const char description[] = "Appuyez sur Ⓐ pour\nprendre une carte et\ncréer une nouvelle\nsauvegarde.\n\nLangue : français\n\nⓞ Options";
                    TextSet(self->saveGameTitle, fontMagic, kDrawModeCopy, newGame, 99);
                    TextSet(self->saveGameDescription, fontRoobert10Bold, kDrawModeCopy, description, 200);
                } break;
                case MELLanguageJapanese: {
                    static const char newGame[] = "NEW GAME";
                    static const char description[] = "Ⓐを押してカードを引き、\n新しいセーブを作成します。\n\n言語：日本語\n\nⓞ オプション";
                    TextSet(self->saveGameTitle, fontMagic, kDrawModeCopy, newGame, 99);
                    TextSetWithLeading(self->saveGameDescription, fontJfDotK14Bold, kDrawModeCopy, 4, description, 200);
                } break;
                case MELLanguageBrazilianPortuguese: {
                    static const char newGame[] = "NOVO JOGO";
                    static const char description[] = "Pressione Ⓐ para pegar\numa carta e criar um novo\njogo.\n\nIdioma: Português (Brasil)\n\nⓞ Opções";
                    TextSet(self->saveGameTitle, fontMagic, kDrawModeCopy, newGame, 99);
                    TextSet(self->saveGameDescription, fontRoobert10Bold, kDrawModeCopy, description, 200);
                } break;
            }
        }
    }
    self->selectedSaveGame = selection;

    return true;
}

#pragma mark - Menu

static int updateMenu(void * _Nonnull userdata) {
    TitleScene *self = userdata;
    self->menuGrid->disableInputs = false;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();
    playdate->sprite->updateAndDrawSprites();
#if SHOW_FRAME_RATE
    playdate->system->drawFPS(0, LCD_ROWS - 12);
#endif

    LCDSprite *sky = MELSceneFindSpriteByName(SpriteNameMenuCiel);
    if (StrideIsDone(sky)) {
        playdate->sprite->setUpdateFunction(sky, MELSpriteUpdate);
    }

    SampleLoadNextSample();
    PDButtons pressed, pressing;
    playdate->system->getButtonState(&pressing, &pressed, NULL);
    // Début du jeu
    int selection = self->menuGrid->selection.y;
    const int oldSelection = selection;
    if (pressed & kButtonLeft) {
        selection = MELIntMin(selection + 1, TitleMenuItemSchool);
    } else if (pressed & kButtonRight) {
        selection = MELIntMax(selection - 1, TitleMenuItemStory);
    }
    if (selection != oldSelection) {
        MELGridViewSetSelection(self->menuGridSprite, MELIntPointMake(0, selection));
    }
    if ((pressed & kButtonA) && self->menuGrid->selection.y == TitleMenuItemStory) {
        StorySelectScene *storySelectScene = StorySelectSceneAlloc();
        MELSceneMakeCurrent(&storySelectScene->super);
        return true;
    } else if ((pressed & kButtonA) && self->menuGrid->selection.y == TitleMenuItemScoreAttack) {
        ScoreAttackSelectScene *scoreAttackSelectScene = ScoreAttackSelectSceneAlloc();
        MELSceneMakeCurrent(&scoreAttackSelectScene->super);
        return true;
    } else if ((pressed & kButtonA) && self->menuGrid->selection.y == TitleMenuItemArcade) {
        RallyScoreScene *rallyScoreScene = RallyScoreSceneAlloc();
        MELSceneMakeCurrent(&rallyScoreScene->super);
        return true;
    } else if ((pressed & kButtonA) && self->menuGrid->selection.y == TitleMenuItemSchool) {
        LessonSelectScene *lessonScene = LessonSelectSceneAlloc();
        MELSceneMakeCurrent(&lessonScene->super);
        return true;
    } else if (pressed & kButtonB) {
        self->menuGrid->disableInputs = true;
        clearAndRecreateCards(self);

        LCDSprite *skySprite = MELSceneFindSpriteByName(SpriteNameMenuCiel);
        StrideSkip(skySprite);

        translateWithDirection(self, MELDirectionLeft);
    }

    return true;
}

static MELBoolean hasNewLesson(void) {
    const uint16_t victories = currentSaveGame.wins;
    for (unsigned int index = 0; index < kLessons.count; index++) {
        Lesson *lesson = kLessons.memory + index;
        const uint16_t victoriesToUnlock = lesson->objectives.victoriesToUnlock;
        if (victoriesToUnlock > 0 && victories >= victoriesToUnlock && !isLessonDone(index)) {
            return true;
        }
    }
    return false;
}

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected) {
    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->setDrawMode(kDrawModeCopy);
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);

    if (isSelected) {
        graphics->setDrawMode(kDrawModeInverted);

        loadSpriteMenuCursorPalette();
        const int top = (spriteMenuItems.size.height - spriteMenuCursor.size.height) / 2;
        graphics->drawBitmap(graphics->getTableBitmap(spriteMenuCursor.palette, 0), 0, top, kBitmapUnflipped);
    }
    loadSpriteMenuItemsPalette();
    const int left = spriteMenuCursor.size.width + 2;
    MELAnimationDefinition *animation = MELSpriteDefinitionGetAnimationDefinition(spriteMenuItems, y, (MELAnimationDirection) currentLanguage);
    if (animation != NULL) {
        LCDBitmap *label = graphics->getTableBitmap(spriteMenuItems.palette, animation->frames[0].atlasIndex);
        graphics->drawBitmap(label, left, 0, kBitmapUnflipped);
    }

    if (y == TitleMenuItemSchool && hasNewLesson()) {
        loadSpriteStoryClearPalette();
        MELAnimationDefinition *animation = MELSpriteDefinitionGetAnimationDefinition(spriteStoryClear, 1, MELAnimationDirectionRight);
        LCDBitmap *icon = graphics->getTableBitmap(spriteStoryClear.palette, animation->frames[0].atlasIndex);
        int x = gridView->cellSize.width - spriteStoryClear.size.width;
        switch (currentLanguage) {
            case MELLanguageEnglish:
            case MELLanguageBrazilianPortuguese:
                x -= 60;
                break;
            case MELLanguageJapanese:
                x -= 150;
                break;
            default:
                break;
        }
        graphics->drawBitmap(icon, x, (gridView->cellSize.height - spriteStoryClear.size.height) / 2, kBitmapUnflipped);
    }

    graphics->popContext();
}

#pragma mark - Changement de langue

static void onLanguageChanged(void * _Nonnull userdata, int language) {
    TitleScene *self = userdata;
    if (self->menuGridSprite) {
        MELGridViewRepaint(self->menuGridSprite);
    }
    if (self->saveGameTitle) {
        MELSprite *melSprite = playdate->sprite->getUserdata(self->saveGameTitle);
        melSprite->class->destroy(self->saveGameTitle);
        self->saveGameTitle = NULL;
    }
    const int selection = self->selectedSaveGame;
    const int saveGameCount = self->saveGames.count;
    if (getTitleScreen() == TitleScreenSaveSelect && saveGameCount > 0 && selection >= 0 && selection < saveGameCount) {
        self->saveGames.memory[selection].language = language;
        SaveGameToDisk(self->saveGames.memory + selection);
    }
}
