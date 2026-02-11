//
//  classes.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 28/11/2023.
//

#include "classes.h"

#include "caught.h"
#include "thrown.h"
#include "player.h"
#include "bullet.h"
#include "bonus.h"
#include "camera.h"
#include "rocket.h"
#include "explosion.h"
#include "cloudtransition.h"
#include "boss_satellite_aux.h"
#include "fade.h"

const MELSpriteClass * _Nullable MELSpriteClassForName(SpriteClassName className) {
    switch (className) {
        case SpriteClassNameDefault:
            return &MELSpriteClassDefault;
        case SpriteClassNameBonus:
            return BonusGetClass();
        case SpriteClassNameBossSatelliteAux:
            return BossSatelliteAuxGetClass();
        case SpriteClassNameBullet:
            return BulletGetClass();
        case SpriteClassNameCameraShake:
            return CameraShakeGetClass();
        case SpriteClassNameCaught:
            return CaughtGetClass();
        case SpriteClassNameCloudTransition:
            return CloudTransitionGetClass();
        case SpriteClassNameExplosion:
            return ExplosionGetClass();
        case SpriteClassNameFade:
            return FadeGetClass();
        case SpriteClassNamePlayer:
            return PlayerGetClass();
        case SpriteClassNameRocket:
            return RocketGetClass();
        case SpriteClassNameThrown:
            return ThrownGetClass();
        default:
            playdate->system->logToConsole("Unsupported class name: %d", className);
            return NULL;
    }
}
