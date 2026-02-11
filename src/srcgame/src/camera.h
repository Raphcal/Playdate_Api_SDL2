//
//  camera.h
//  Roll
//
//  Created by Raphaël Calabro on 03/06/2022.
//

#ifndef camera_h
#define camera_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    /**
     * Rectangle visible à l'écran. L'origine est en haut à gauche.
     */
    MELRectangle frame;
} Camera;

extern Camera camera;

LCDSprite * _Nonnull CameraConstructor(void);

MELBoolean CameraIsVisible(Camera camera, MELSprite * _Nonnull sprite);

/**
 * Effet de tremblement de terre.
 *
 * @param duration Durée en seconde du tremblement.
 * @param intensity Intensité du tremblement.
 */
LCDSprite * _Nonnull CameraShakeConstructor(float duration, float intensity);

const MELSpriteClass * _Nonnull CameraShakeGetClass(void);

#endif /* camera_h */
