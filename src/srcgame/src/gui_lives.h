//
//  gui_lives.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 17/03/2023.
//

#ifndef gui_lives_h
#define gui_lives_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELSprite super;
    MELPoint topLeft;
    LCDBitmap * _Nullable lifeIcon;
    LCDBitmap * _Nullable maxIcon;
    const int8_t * _Nonnull lives;
    int8_t currentLives;
} GUILives;

LCDSprite * _Nonnull GUILivesConstructor(const int8_t * _Nonnull lives);
LCDSprite * _Nonnull GUILivesConstructorWithImagePathAndTopLeft(const int8_t * _Nonnull lives, const char * _Nonnull imagePath, MELPoint topLeft);

void GUILivesSetValue(LCDSprite * _Nullable sprite, int8_t lives);
void GUILivesUpdateTopLeftWithCurrentOrigin(LCDSprite * _Nullable sprite);

const MELSpriteClass * _Nonnull GUILivesGetClass(void);

#endif /* gui_lives_h */
