//
//  ending2scene.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 12/10/2023.
//

#ifndef ending2scene_h
#define ending2scene_h

#include "common.h"
#include "storyscene.h"
#include "player.h"
#include "../lib/melice.h"

StoryScene * _Nonnull Ending2SceneSceneAlloc(PlayerCharacter character, unsigned int score, Ranks ranks);

#endif /* ending2scene_h */
