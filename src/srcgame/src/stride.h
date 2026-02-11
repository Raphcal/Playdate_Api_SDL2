//
//  stride.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 13/07/2023.
//

#ifndef stride_h
#define stride_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull StrideConstructor(MELSpriteDefinition * _Nonnull definition, MELPoint from, MELPoint to, float delay, float duration);
void StrideSpriteTo(LCDSprite * _Nonnull sprite, MELPoint to, float delay, float duration);
void StrideTogetherAlignedRight(LCDSprite * _Nullable sprite, LCDSprite * _Nullable spriteToFollow);
void StrideSkip(LCDSprite * _Nullable sprite);
void StrideResume(LCDSprite * _Nonnull sprite);
MELBoolean StrideIsDone(LCDSprite * _Nullable sprite);

#endif /* stride_h */
