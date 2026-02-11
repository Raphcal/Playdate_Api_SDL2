//
//  storyscene.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/08/2023.
//

#ifndef storyscene_h
#define storyscene_h

#include "common.h"
#include "player.h"
#include "rankingboard.h"
#include "../lib/melice.h"
#include "../gen/maps.h"

typedef struct animation_link AnimationLink;

typedef struct story_scene {
    MELScene super;
    void (* _Nonnull initSprite)(struct story_scene * _Nonnull self, SpriteName spriteName, LCDSprite * _Nonnull sprite, MELSprite * _Nonnull melSprite);
    MapName mapName;
    MELMap * _Nullable map;

    PlayerCharacter character;
    unsigned int score;
    Ranks ranks;
    float scrollSpeed;

    uint8_t linkCount;
    uint8_t link;
} StoryScene;

StoryScene * _Nonnull StorySceneAlloc(PlayerCharacter character);
StoryScene * _Nonnull StorySceneAllocWithMapNameAndLinkCount(PlayerCharacter character, MapName mapName, uint8_t linkCount);
void StorySceneDealloc(MELScene * _Nonnull self, AnimationLink * _Nonnull chain);

void StorySceneUpdateAnimationChain(StoryScene * _Nonnull self, AnimationLink * _Nonnull animationChain);


#endif /* storyscene_h */
