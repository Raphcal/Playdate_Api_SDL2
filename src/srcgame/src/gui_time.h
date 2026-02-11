//
//  gui_time.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 17/03/2023.
//

#ifndef gui_time_h
#define gui_time_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull GUITimeConstructor(const MELTimeInterval * _Nonnull elapsed);

const MELSpriteClass * _Nonnull GUITimeGetClass(void);

#endif /* gui_time_h */
