//
//  gui_combo.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/10/2023.
//

#ifndef gui_combo_h
#define gui_combo_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    LCDSprite * _Nullable hitLabel;
    LCDSprite * _Nullable progressBar;
    LCDSprite * _Nullable counter;
    uint16_t maxCombo;
    uint16_t combo;
    float timer;
} GUICombo;

void GUIComboInit(GUICombo * _Nonnull self);
void GUIComboUpdate(GUICombo * _Nonnull self, int combo, float timer);

void GUIComboAnimateAppear(GUICombo self);
void GUIComboAnimateDisappear(GUICombo self);

#endif /* gui_combo_h */
