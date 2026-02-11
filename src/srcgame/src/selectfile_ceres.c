//
//  selectfile_Ceres.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 20/03/2024.
//

#include "selectfile_ceres.h"

#include "stride.h"
#include "titlescene.h"
#include "../gen/spritemenudove.h"

typedef struct {
    int32_t oldSelection;
    int32_t * _Nullable selection;
    MELPoint from;
    MELPoint to;
    float time;
} SelectFileCeres;

static void update(LCDSprite * _Nonnull sprite);
static LCDSprite * _Nullable targetForMenuItem(TitleMenuItem item);
static void hide(SpriteName spriteName);
static void setOrigin(SpriteName spriteName, float x, float y);

static const float duration = 0.75f;

void SelectFileCeresConfigure(LCDSprite * _Nonnull sprite, int32_t * _Nullable selection) {
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    melSprite->class = &MELSpriteClassDefault;

    SelectFileCeres *self = playdate->system->realloc(NULL, sizeof(SelectFileCeres));
    *self = (SelectFileCeres) {
        .time = duration,
        .selection = selection,
        .oldSelection = *selection,
    };
    melSprite->userdata = self;
    melSprite->autoReleaseUserdata = true;
    playdate->sprite->setUpdateFunction(sprite, update);
}

void SelectFileCeresSetInitialPosition(MELSprite * _Nullable melSprite) {
    if (melSprite == NULL) {
        playdate->system->logToConsole("Ceres is NULL");
        return;
    }
    SelectFileCeres *self = melSprite->userdata;
    const int32_t selection = self->oldSelection;

    LCDSprite *targetSprite = targetForMenuItem(selection);
    if (targetSprite == NULL) {
        playdate->system->logToConsole("Unable to find Ceres target");
        return;
    }
    MELSprite *targetMelSprite = playdate->sprite->getUserdata(targetSprite);

    melSprite->frame.origin = MELPointAdd(targetMelSprite->frame.origin, (MELPoint) {
        .x = selection == 0 ? -30 : 30,
        .y = -44
    });
    melSprite->direction = self->to.x < self->from.x ? MELDirectionLeft : MELDirectionRight;

    hide(SpriteNameMenuDoor);
    hide(SpriteNameMenuChimney);

    const int skyMove = -192 / TitleMenuItemCount;
    setOrigin(SpriteNameMenuCiel, skyMove * selection, 0.0f);
    setOrigin(SpriteNameMenuSchool, 12 * selection, 0.0f);
    setOrigin(SpriteNameMenuBoard, 18 * selection, 0.0f);
    setOrigin(SpriteNameMenuCaroline, 24 * selection, 0.0f);
}

#pragma mark - Méthodes privées

static LCDSprite * _Nullable targetForMenuItem(TitleMenuItem item) {
    switch (item) {
        case TitleMenuItemStory:
            return MELSceneFindSpriteByName(SpriteNameMenuCaroline);
        case TitleMenuItemScoreAttack:
            return MELSceneFindSpriteByName(SpriteNameMenuBoard);
        case TitleMenuItemArcade:
            return MELSceneFindSpriteByName(SpriteNameMenuChimney);
        case TitleMenuItemSchool:
            return MELSceneFindSpriteByName(SpriteNameMenuDoor);
        default:
            return NULL;
    }
}

static void hide(SpriteName spriteName) {
    LCDSprite *sprite = MELSceneFindSpriteByName(spriteName);
    if (!sprite) {
        return;
    }
    playdate->sprite->setVisible(sprite, false);
}

static void setOrigin(SpriteName spriteName, float x, float y) {
    LCDSprite *sprite = MELSceneFindSpriteByName(spriteName);
    if (!sprite) {
        return;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->frame.origin = MELPointAdd(self->instance->center, MELPointMake(x, y));
}

static void strideRelativeToInstance(SpriteName spriteName, float x, float y) {
    LCDSprite *sprite = MELSceneFindSpriteByName(spriteName);
    if (!sprite) {
        return;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    const MELPoint center = MELPointAdd(self->instance->center, MELPointMake(x, y));
    StrideSpriteTo(sprite, center, 0.0f, 0.7f);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    if (camera.frame.origin.x < 600.0f) {
        MELPoint origin = melSprite->frame.origin;
        playdate->sprite->moveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
        return;
    }
    SelectFileCeres *self = melSprite->userdata;

    const int32_t selection = *self->selection;
    const int32_t oldSelection = self->oldSelection;

    float time = self->time;
    if (selection != oldSelection) {
        LCDSprite *targetSprite = targetForMenuItem(selection);
        MELSprite *targetMelSprite = playdate->sprite->getUserdata(targetSprite);

        self->oldSelection = selection;
        self->time = 0.0f;
        self->from = melSprite->frame.origin;
        self->to = MELPointAdd(targetMelSprite->frame.origin, (MELPoint) {
            .x = selection == 0 ? -30 : 30,
            .y = -44
        });
        melSprite->direction = self->to.x < self->from.x ? MELDirectionLeft : MELDirectionRight;

        const int skyMove = -192 / TitleMenuItemCount;
        strideRelativeToInstance(SpriteNameMenuCiel, skyMove * selection, 0.0f);
        strideRelativeToInstance(SpriteNameMenuSchool, 12 * selection, 0.0f);
        strideRelativeToInstance(SpriteNameMenuBoard, 18 * selection, 0.0f);
        strideRelativeToInstance(SpriteNameMenuCaroline, 24 * selection, 0.0f);
    } else if (time < duration) {
        self->time = time += DELTA;
        const float progress = MELEaseInOut(0, duration, time);
        MELPoint from = self->from;
        MELPoint to = self->to;

        melSprite->frame.origin = (MELPoint) {
            .x = from.x + (to.x - from.x) * progress,
            .y = from.y + (to.y - from.y) * progress,
        };
    }

    MELSpriteUpdate(sprite);
}
