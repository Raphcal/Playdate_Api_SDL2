//
//  new_best.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 05/12/2023.
//

#ifndef new_best_h
#define new_best_h

#include "common.h"
#include "../lib/melice.h"

typedef enum {
    NewBestMessagePersonalBest = AnimationNameStand,
    NewBestMessageTop10Reached = AnimationNameWalk,
    NewBestMessage3rdPlace = AnimationNameRun,
    NewBestMessage2ndPlace = AnimationNameSkid,
    NewBestMessageWorldRecord = AnimationNameJump,
    NewBestMessageTop20Reached = AnimationNameFall,
    NewBestMessageYouAreFirst = AnimationNameShaky,
} NewBestMessage;

LCDSprite * _Nonnull NewBestConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);
LCDSprite * _Nonnull NewBestConstructorWithMessage(NewBestMessage message);
MELSprite * _Nullable NewBestLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* new_best_h */
