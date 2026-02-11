//
//  savegame.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/11/2023.
//

#ifndef savegame_h
#define savegame_h

#include "common.h"
#include "../lib/melice.h"

typedef struct melscene MELScene;

void SaveState(void);
MELBoolean LoadState(void);

void GameSceneSaveState(MELScene * _Nonnull self, MELOutputStream * _Nonnull outputStream);
void ScoreEntrySceneSaveState(MELScene * _Nonnull self, MELOutputStream * _Nonnull outputStream);
void StorySceneSaveState(MELScene * _Nonnull scene, MELOutputStream * _Nonnull outputStream);

#endif /* savegame_h */
