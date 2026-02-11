//
//  story.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#ifndef story_h
#define story_h

#include "common.h"
#include "../lib/melice.h"

#include "character.h"
#include "actor.h"
#include "side.h"
#include "background.h"
#include "phrase.h"

#define kStoryCount 6
#define kStoryUnlockConditionCount 3

typedef struct {
    Character opponent;
    Actor * _Nullable actor;
    float gameSpeed;
    Background background;
    MELPoint pin;
    PhraseList dialog;
} Encounter;
MELListDefine(Encounter);

typedef enum {
    StoryNameDove,
    StoryNameCaroline,
    StoryNameClovis,
    StoryNameBelleplume,
    StoryNameCrocoford,
    StoryNameMelusine,
} StoryName;

typedef struct {
    Character character;
    EncounterList encounters;
} Story;

extern const Character kStoryCharacters[kStoryCount];
extern const Story * _Nonnull kStories[kStoryCount];

const Story * _Nullable StoryGetByCharacter(Character character);
StoryName StoryNameByCharacter(Character character);
MELBoolean StoryIsUnlocked(StoryName storyName);

#endif /* story_h */
