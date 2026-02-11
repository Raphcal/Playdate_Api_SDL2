//
//  profile.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 20/01/2024.
//

#ifndef profile_h
#define profile_h

#include "common.h"

#include "story.h"

typedef struct {
    /** Nom  */
    char name[10];
    /** Dernière histoire jouée. */
    Story * _Nullable story;
    /** Nombre de scénarios terminés. */
    int completion;
    int storiesState[kStoryCount];
} Profile;

Profile * _Nullable currentProfile;

#endif /* profile_h */
