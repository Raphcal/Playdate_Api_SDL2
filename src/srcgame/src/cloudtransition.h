//
//  cloudtransition.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/08/2023.
//

#ifndef cloudtransition_h
#define cloudtransition_h

#include "common.h"
#include "gamescene.h"
#include "../lib/melice.h"
#include "../gen/maps.h"

LCDSprite * _Nonnull CloudTransitionConstructor(MapName nextMapName, GameScene * _Nonnull gameScene);
MELSprite * _Nullable CloudTransitionLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);
const MELSpriteClass * _Nonnull CloudTransitionGetClass(void);

#endif /* cloudtransition_h */
