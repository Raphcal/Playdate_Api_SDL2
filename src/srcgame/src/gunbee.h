//
//  gunbee.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 20/05/2023.
//

#ifndef gunbee_h
#define gunbee_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull GunbeeConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable GunbeeLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);


#endif /* gunbee_h */
