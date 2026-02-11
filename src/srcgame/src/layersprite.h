//
//  layersprite.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/07/2023.
//

#ifndef layersprite_h
#define layersprite_h

#include "common.h"
#include "../lib/melice.h"
#include "../gen/maps.h"

LCDSprite * _Nonnull LayerSpriteConstructor(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, MELBoolean isRepeat);

#endif /* layersprite_h */
