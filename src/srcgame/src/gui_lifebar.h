//
//  gui_lifebar.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/03/2023.
//

#ifndef gui_lifebar_h
#define gui_lifebar_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull GUILifebarConstructor(const int maxHitPoints, const int * _Nonnull hitPoints);

void GUILifebarSave(LCDSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
LCDSprite * _Nonnull GUILifebarLoad(MELInputStream * _Nonnull inputStream, const int maxHitPoints, const int * _Nonnull hitPoints);

extern const LCDColor kDiagonalsPattern;

#endif /* gui_lifebar_h */
