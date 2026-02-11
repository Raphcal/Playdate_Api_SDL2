//
//  gameover.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 21/07/2023.
//

#ifndef gameover_h
#define gameover_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull GameoverConstructorNoArgs(void);
LCDSprite * _Nonnull GameoverConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

MELSprite * _Nullable GameoverLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);


#endif /* gameover_h */
