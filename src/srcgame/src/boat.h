//
//  boat.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/04/2023.
//

#ifndef boat_h
#define boat_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull BoatConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable BoatLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* boat_h */
