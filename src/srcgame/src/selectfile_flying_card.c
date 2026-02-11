//
//  selectfile_flying_card.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 07/03/2024.
//

#include "selectfile_flying_card.h"

#include "titlescene.h"
#include "../gen/spritefileselectcard.h"

typedef struct {
    MELSprite super;
    float time;
    SaveGameList * _Nonnull saveGames;
    int8_t * _Nonnull selectedSaveGame;
} SelectFileFlyingCard;

static void destroy(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);

static MELPoint cardCenter(int8_t selected, SaveGameList * _Nonnull saveGames);

static const MELSpriteClass SelectFileFlyingCardClass = (MELSpriteClass) {
    .name = SpriteClassNameSelectFileFlyingCard,
    .destroy = destroy,
};

LCDSprite * _Nonnull SelectFileFlyingCardConstructor(SaveGameList * _Nonnull saveGames, int8_t * _Nonnull selectedSaveGame) {
    SelectFileFlyingCard *self = new(SelectFileFlyingCard);
    *self = (SelectFileFlyingCard) {};
    const int8_t selected = *selectedSaveGame;
    loadSpriteFileSelectCardPalette();
    LCDSprite *sprite = MELSpriteInitWithCenter(&self->super, &spriteFileSelectCard, cardCenter(selected, saveGames));
    self->super.class = &SelectFileFlyingCardClass;
    self->saveGames = saveGames;
    self->selectedSaveGame = selectedSaveGame;

    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setZIndex(sprite, 3);
    if (selected < 0) {
        playdate->sprite->setVisible(sprite, false);
    }
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

void SelectFileFlyingCardSetSaveGamesAndSelectedGame(LCDSprite * _Nonnull sprite, SaveGameList * _Nonnull saveGames, int8_t * _Nonnull selectedSaveGame) {
    SelectFileFlyingCard *self = playdate->sprite->getUserdata(sprite);
    self->saveGames = saveGames;
    self->selectedSaveGame = selectedSaveGame;
}

static void destroy(LCDSprite * _Nonnull sprite) {
    playdate->graphics->freeBitmapTable(spriteFileSelectCard.palette);
    spriteFileSelectCard.palette = NULL;
    MELSpriteDealloc(sprite);
}

static void update(LCDSprite * _Nonnull sprite) {
    SelectFileFlyingCard *self = playdate->sprite->getUserdata(sprite);
    const float time = self->time += DELTA;

    const int8_t selection = *self->selectedSaveGame;
    const MELBoolean shouldBeVisible = selection >= 0;
    const MELBoolean isVisible = playdate->sprite->isVisible(sprite) == true;
    if (shouldBeVisible != isVisible) {
        playdate->sprite->setVisible(sprite, shouldBeVisible);
    }
    if (shouldBeVisible) {
        // TODO: Choisir la face de la carte
        const MELBoolean isNewGame = selection == self->saveGames->count;
        if (isNewGame) {
            MELSpriteSetAnimation(&self->super, AnimationNameStand);
        } else {
            SaveGame saveGame = self->saveGames->memory[selection];
            MELSpriteSetAnimation(&self->super, saveGame.card.suit + 1);
            MELAnimation *animation = self->super.animation;
            MELSingleFrameAnimationReuse(animation);
            MELAnimationSetFrameIndex(animation, saveGame.card.rank);
        }

        MELPoint center = cardCenter(selection, self->saveGames);
        self->super.frame.origin = (MELPoint) {
            .x = center.x + sinf(time * 1.5f * MEL_PI) * 2.0f,
            .y = center.y + sinf(time * 2.0f * MEL_PI) * 2.0f - 20.0f,
        };
    }
    MELSpriteUpdate(sprite);
}

static MELPoint cardCenter(int8_t selected, SaveGameList * _Nonnull saveGames) {
    const int selectedAsInt = MELIntBound(0, selected, 5);
    const int saveCount = MELIntBound(0, saveGames->count + 1, 5);
    const float cardSize = 0.5f;
    const float angle = (MEL_PI / (saveCount + 2)) * (saveCount - selectedAsInt) + cardSize / 4.0f;
    const float cosAngle = cosf(angle + cardSize / 2.0f);
    const float sinAngle = sinf(angle + cardSize / 2.0f);
    return (MELPoint) {
        .x = 600.0f + kCarolineLeftPadding + 75.0f * cosAngle,
        .y = 150.0f + 15.0f * sinAngle
    };
}
