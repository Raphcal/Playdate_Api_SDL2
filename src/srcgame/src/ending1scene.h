//
//  ending1scene.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 12/10/2023.
//

#ifndef ending1scene_h
#define ending1scene_h

#include "common.h"
#include "storyscene.h"
#include "player.h"
#include "../lib/melice.h"

StoryScene * _Nonnull Ending1SceneSceneAlloc(PlayerCharacter character, unsigned int score, Ranks ranks);

#endif /* ending1scene_h */
