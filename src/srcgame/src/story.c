//
//  story.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#include "story.h"

#include "savegame.h"

#include "story_dove.h"
#include "story_caroline.h"
#include "story_clovis.h"
#include "story_belleplume.h"
#include "story_crocoford.h"
#include "story_melusine.h"

MELListImplement(Encounter);

const Character kStoryCharacters[kStoryCount] = {
    CharacterCeres,
    CharacterCaroline,
    CharacterClovis,
    CharacterBelleplume,
    CharacterCrocoford,
    CharacterMelusine,
};

const Story * _Nonnull kStories[kStoryCount] = {
    &kDoveStory,
    &kCarolineStory,
    &kClovisStory,
    &kBelleplumeStory,
    &kCrocofordStory,
    &kMelusineStory,
};

const Story * _Nullable StoryGetByCharacter(Character character) {
    switch (character) {
        case CharacterCeres:
            return &kDoveStory;
        case CharacterCaroline:
            return &kCarolineStory;
        case CharacterClovis:
            return &kClovisStory;
        case CharacterBelleplume:
            return &kBelleplumeStory;
        case CharacterCrocoford:
            return &kCrocofordStory;
        case CharacterMelusine:
            return &kMelusineStory;
        default:
            return NULL;
    }
}

StoryName StoryNameByCharacter(Character character) {
    switch (character) {
        case CharacterCeres:
            return StoryNameDove;
        case StoryNameCaroline:
            return StoryNameCaroline;
        case CharacterClovis:
            return StoryNameClovis;
        case CharacterBelleplume:
            return StoryNameBelleplume;
        case CharacterCrocoford:
            return StoryNameCrocoford;
        case CharacterMelusine:
            return StoryNameMelusine;
        default:
            return 0;
    }
}

MELBoolean StoryIsUnlocked(StoryName storyName) {
    switch (storyName) {
        case StoryNameDove:
            return true;
        case StoryNameCaroline:
            return true;
        case StoryNameClovis:
            return true;
        case StoryNameBelleplume:
            return currentSaveGame.completion[StoryNameCaroline] && currentSaveGame.completion[StoryNameClovis];
        case StoryNameCrocoford:
            return currentSaveGame.completion[StoryNameBelleplume];
        case StoryNameMelusine:
            return currentSaveGame.completion[StoryNameCrocoford];
        default:
            return false;
    }
}
