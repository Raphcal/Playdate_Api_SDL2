//
//  scoreentryscene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 04/07/2023.
//

#include "scoreentryscene.h"

#include "camera.h"
#include "rankingboard.h"
#include "text.h"
#include "image.h"
#include "titlescene.h"
#include "transitionout.h"
#include "savegame.h"

#include "../gen/spritevictorytransition.h"

#define kLastNameFile "username.data"

static const int screenPadding = 2;
static const int rankingItemFinalWidth = kRankingItemWidth - 16;

static const char * _Nonnull rankLabels[] = {
    "1st place !",
    "2nd place !",
    "3rd place !",
    "4th place",
    "5th place",
    "6th place",
    "7th place",
    "8th place",
    "9th place",
    "10th place",
    "11th place",
    "12th place",
    "13th place",
    "14th place",
    "15th place",
    "16th place",
    "17th place",
    "18th place",
    "19th place",
    "20th place",
};

static void onKeyboardAnimating(void * _Nonnull userdata);
static void onKeyboardWillHide(MELBoolean didConfirm, void * _Nonnull userdata);
static void onTextChanged(void * _Nonnull userdata);

static void init(MELScene * _Nonnull scene) {
    ScoreEntryScene *self = (ScoreEntryScene *)scene;

    playdate->sound->fileplayer->loadIntoPlayer(self->filePlayer, "audio/music-score");
    playdate->sound->fileplayer->play(self->filePlayer, 0);

    LCDBitmapTable *faces;
    self->faces = faces = SpriteNameLoadBitmapTable(SpriteNameRank48);

    // Artwork
    ImageConstructorWithPath((MELPoint) {
        .x = LCD_COLUMNS - kKeyboardWidth / 2,
        .y = LCD_ROWS / 2
    }, self->score.character == PlayerCharacterKatsuo ? "gui-win-katsuo" : "gui-win-saki");

    // Texte "back"
    LCDSprite *backSprite = TextConstructor(MELPointZero, romajiFont, kDrawModeCopy, "\b back", 99);
    MELSprite *back = playdate->sprite->getUserdata(backSprite);
    MELRectangle backFrame = back->frame;
    MELRectangleOriginIsCenterSetRight(&backFrame, LCD_COLUMNS - 8);
    MELRectangleOriginIsCenterSetBottom(&backFrame, LCD_ROWS - 8);
    back->frame = backFrame;

    // Item éditable
    const int rank = self->score.super.rank - 1;
    const int rankY = kRankingItemMargin + kRankingItemHeight / 2 + (kRankingItemMargin + kRankingItemHeight) * (rank + 1);
    const int screenCenter = LCD_ROWS / 2;
    const int itemY = MELIntMin(rankY, screenCenter);

    LCDSprite *sprite = RankingItemConstructor(itemY, self->score, 10, NULL);
    RankingItem *item = playdate->sprite->getUserdata(sprite);
    item->isEditable = true;
    item->isDirty = true;
    self->rankingItem = item;

    // Message "vous êtes 2ème !"
    const MELPoint textCenter = (MELPoint) {
        .x = (LCD_COLUMNS - kKeyboardWidth) / 2,
        .y = itemY - kRankingItemHeight / 2 - kRankingItemMargin - 8
    };
    LCDSprite *rankLabel = TextConstructor(textCenter, romajiFont, kDrawModeCopy, rankLabels[rank], 99);
    self->rankLabel = playdate->sprite->getUserdata(rankLabel);

    // Scores au dessus
    for (int y = itemY - 64 - kRankingItemHeight / 2, topRank = rank - 1; topRank >= 0 && y > -kRankingItemHeight / 2; y -= kRankingItemHeight + kRankingItemMargin, topRank--) {
        Score score = localScores.memory[topRank];
        RankingItemConstructor(y, score, 10, faces);
    }

    // Scores en dessous
    for (int y = itemY + 64 + kRankingItemHeight / 2, bottomRank = rank; bottomRank < localScores.count && y < LCD_ROWS + kRankingItemHeight / 2; y += kRankingItemHeight + kRankingItemMargin, bottomRank++) {
        Score score = localScores.memory[bottomRank];
        score.super.rank++;
        RankingItemConstructor(y, score, 10, faces);
    }

    camera.frame.origin = MELPointZero;

    MELKeyboardSetAnimatingCallback(self->keyboard, onKeyboardAnimating, self);
    MELKeyboardSetWillHideCallback(self->keyboard, onKeyboardWillHide, self);
    MELKeyboardSetTextChangedCallback(self->keyboard, onTextChanged, self);
    
    if (self->showTransition) {
        loadSpriteVictoryTransitionPalette();
        TransitionOutConstructor(&spriteVictoryTransition);
    } else {
        MELKeyboardShow(self->keyboard, self->score.super.player, (unsigned int) strlen(self->score.super.player));
    }
}

// TODO: Mettre cette méthode en commun
static void deallocLCDSpriteRef(LCDSpriteRef * _Nonnull spriteRef) {
    LCDSprite *sprite = *spriteRef;
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    if (self->definition.palette) {
        playdate->graphics->freeBitmapTable(self->definition.palette);
    } else {
        LCDBitmap *bitmap = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(bitmap);
    }
    MELAnimationDealloc(self->animation);
    if (self->hitbox != NULL) {
        MELHitboxDeinit(self->hitbox);
        playdate->system->realloc(self->hitbox, 0);
        self->hitbox = NULL;
    }
    if (self->instance != NULL) {
        self->instance->sprite = NULL;
    }
    playdate->system->realloc(self, 0);
    playdate->sprite->removeSprite(sprite);
    playdate->sprite->freeSprite(sprite);
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeScoreEntry) {
        playdate->system->error("Given scene is not a ScoreEntryScene instance");
        return;
    }
    ScoreEntryScene *self = (ScoreEntryScene *)scene;
    LCDSpriteRefListDeinitWithDeinitFunction(&self->super.sprites, deallocLCDSpriteRef);
    MELKeyboardFree(self->keyboard);
    playdate->graphics->freeBitmapTable(self->faces);
    playdate->sound->fileplayer->freePlayer(self->filePlayer);
    if (self->elevenScore.super.player) {
        playdate->system->realloc(self->elevenScore.super.player, 0);
    }
    playdate->system->realloc(self, 0);
}

static int update(void * _Nonnull userdata) {
    ScoreEntryScene *self = userdata;
    playdate->sprite->updateAndDrawSprites();

    if (self->showTransition && MELSceneFindSpriteByName(SpriteNameVictoryTransition) != NULL) {
        // Transition en cours, pas de scrolling.
        return true;
    } else if (self->showTransition) {
        self->showTransition = false;
        MELKeyboardShow(self->keyboard, self->score.super.player, (unsigned int) strlen(self->score.super.player));
    }

    // TODO: Lancer des confettis autour du score.
    if (!self->keyboard->isVisible) {
        PDButtons pressed;
        playdate->system->getButtonState(NULL, &pressed, NULL);
        if (pressed & kButtonB) {
            TitleScene *titleScene = TitleSceneAlloc();
            titleScene->scrollToRankings = true;
            MELSceneMakeCurrent(&titleScene->super);
        }
    }
    return true;
}

static void onKeyboardAnimating(void * _Nonnull userdata) {
    ScoreEntryScene *self = userdata;
    const float keyboardWidth = self->keyboard->frame.size.width;
    const int rankingItemWidth = MELIntMin(LCD_COLUMNS - keyboardWidth + kRankingItemRankWidth - screenPadding, rankingItemFinalWidth);
    const int rankingItemLeft = (-kRankingItemRankWidth + screenPadding) * (keyboardWidth / kKeyboardWidth);

    LCDSpriteRefList sprites = self->super.sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *lcdSprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(lcdSprite);
        if (isRankingItem(melSprite)) {
            RankingItem *rankingItem = (RankingItem *)melSprite;
            MELRectangle frame = rankingItem->super.frame;
            frame.size.width = &self->rankingItem->super != melSprite ? rankingItemWidth : MELIntMin(rankingItemWidth, rankingItemFinalWidth - 64);
            MELRectangleOriginIsCenterSetLeft(&frame, rankingItemLeft);
            rankingItem->super.frame = frame;
            rankingItem->isDirty = true;
        }
    }
}

static void onKeyboardWillHide(MELBoolean didConfirm, void * _Nonnull userdata) {
    ScoreEntryScene *self = userdata;
    self->rankingItem->isEditable = false;
    self->rankingItem->isDirty = true;

    self->elevenScore = LocalScoresAdd(self->score);
}

static void onTextChanged(void * _Nonnull userdata) {
    ScoreEntryScene *self = userdata;

    int count = self->keyboard->text.count;
    if (count > 0 && self->keyboard->text.memory[count - 1] >= 'A' && self->keyboard->text.memory[count - 1] <= 'Z') {
        self->keyboard->text.memory[count - 1] += 'a' - 'A';
    }
    MELKeyboardGetText(self->keyboard, &self->score.super.player, NULL);
    self->rankingItem->score = self->score;
    self->rankingItem->isDirty = true;
}

ScoreEntryScene * _Nullable ScoreEntryAlloc(PlayerCharacter player, unsigned int score, Ranks ranks) {
    const int rank = ScoreListFindRank(localScores, score);
    if (rank >= localScores.count) {
        return NULL;
    }

    ScoreEntryScene *self = playdate->system->realloc(NULL, sizeof(ScoreEntryScene));
    *self = (ScoreEntryScene) {
        .super = {
            .type = SceneTypeScoreEntry,
            .init = init,
            .dealloc = dealloc,
            .update = update,
            .save = ScoreEntrySceneSaveState,
        },
        .keyboard = MELKeyboardNew(),
        .score = {
            .super = {
                .value = score,
                .player = MELStringCopy(""),
                .rank = rank + 1,
            },
            .character = player,
            .ranks = ranks,
        },
        .elevenScore = {},
        .filePlayer = playdate->sound->fileplayer->newPlayer(),
    };
    return self;
}
