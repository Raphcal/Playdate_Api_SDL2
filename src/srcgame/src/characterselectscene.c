//
//  characterselectscene.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/08/2023.
//

#include "characterselectscene.h"

#include "character_select_character.h"
#include "character_select_grid.h"
#include "character_select_name.h"
#include "character_select_title.h"
#include "character_select_robot.h"
#include "character_select_controls.h"
#include "storyscene.h"
#include "titlescene.h"
#include "samples.h"

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

CharacterSelectScene * _Nonnull CharacterSelectSceneAlloc(void) {
    CharacterSelectScene *self = playdate->system->realloc(NULL, sizeof(CharacterSelectScene));

    *self = (CharacterSelectScene) {
        .super = {
            .type = SceneTypeCharacterSelect,
            .init = init,
            .dealloc = dealloc,
            .update = update,
        },
        .filePlayer = playdate->sound->fileplayer->newPlayer(),
        .selection = PlayerCharacterSaki,
    };
    return self;
}

static void init(MELScene * _Nonnull scene) {
    CharacterSelectScene *self = (CharacterSelectScene *)scene;

    const struct playdate_sound_fileplayer *filePlayerAPI = playdate->sound->fileplayer;
    filePlayerAPI->loadIntoPlayer(self->filePlayer, "audio/music-ready");
    filePlayerAPI->play(self->filePlayer, 0);

    LCDBitmapTable *artworks = self->artworks = LCDBitmapTableLoadOrError("sprites/sprite-character_select_character");
    CharacterSelectCharacterConstructor(PlayerCharacterKatsuo, &self->selection, artworks);
    CharacterSelectCharacterConstructor(PlayerCharacterSaki, &self->selection, artworks);

    CharacterSelectGridConstructorWithSelection(&self->selection);

    LCDBitmapTable *name = self->names = LCDBitmapTableLoadOrError("sprites/sprite-character_name");
    CharacterSelectNameConstructor(PlayerCharacterKatsuo, &self->selection, playdate->graphics->getTableBitmap(name, PlayerCharacterKatsuo));
    CharacterSelectNameConstructor(PlayerCharacterSaki, &self->selection, playdate->graphics->getTableBitmap(name, PlayerCharacterSaki));

    LCDBitmapTable *controls = self->controls = LCDBitmapTableLoadOrError("sprites/sprite-character_controls");
    CharacterSelectControlsConstructor(PlayerCharacterKatsuo, &self->selection, playdate->graphics->getTableBitmap(controls, PlayerCharacterKatsuo));
    CharacterSelectControlsConstructor(PlayerCharacterSaki, &self->selection, playdate->graphics->getTableBitmap(controls, PlayerCharacterSaki));

    CharacterSelectRobotConstructor(&self->selection);

    CharacterSelectTitleConstructor(&self->selection);
}

static void dealloc(MELScene * _Nonnull scene) {
    CharacterSelectScene *self = (CharacterSelectScene *)scene;
    LCDSpriteRefListDeallocReverse(&self->super.sprites);
    playdate->sound->fileplayer->stop(self->filePlayer);
    playdate->sound->fileplayer->freePlayer(self->filePlayer);
    playdate->graphics->freeBitmapTable(self->artworks);
    playdate->graphics->freeBitmapTable(self->names);
    playdate->graphics->freeBitmapTable(self->controls);
    playdate->system->realloc(self, 0);
}

static int update(void * _Nonnull userdata) {
    CharacterSelectScene *self = userdata;
    playdate->sprite->updateAndDrawSprites();

    PDButtons pressed;
    playdate->system->getButtonState(NULL, &pressed, NULL);
    if (pressed & kButtonLeft && self->selection == PlayerCharacterSaki) {
        self->selection = PlayerCharacterKatsuo;
        SamplePlay(SampleSelect);
    } else if (pressed & kButtonRight && self->selection == PlayerCharacterKatsuo) {
        self->selection = PlayerCharacterSaki;
        SamplePlay(SampleSelect);
    }
    if (pressed & kButtonA) {
        StoryScene *storyScene = StorySceneAlloc(self->selection);
        MELSceneMakeCurrent(&storyScene->super);
    }
    else if (pressed & kButtonB) {
        TitleScene *titleScene = TitleSceneAlloc();
        MELSceneMakeCurrent(&titleScene->super);
    }
    return true;
}
