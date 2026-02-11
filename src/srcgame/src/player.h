//
//  player.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 24/01/2023.
//

#ifndef player_h
#define player_h

#include "common.h"
#include "../lib/melice.h"

typedef enum {
    PlayerCharacterKatsuo,
    PlayerCharacterSaki
} PlayerCharacter;
#define kPlayerCharacterCount 2

typedef enum {
    PlayerStateNormal,
    PlayerStateDestroyed,
    PlayerStateClear,
    PlayerStateNoop,
} PlayerState;

typedef struct {
    MELSprite super;
    PlayerState state;
    MELBoolean isDead;
    float speed;
    float maximumSpeed;
    float acceleration;
    LCDSprite * _Nullable explosion;
    MELSubSprite * _Nullable hands;
    MELSubSprite * _Nullable leftHand;
    MELBoolean isCatching;
    PlayerCharacter character;
} Player;

LCDSprite * _Nonnull PlayerConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);
MELSprite * _Nullable PlayerLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);
const MELSpriteClass * _Nonnull PlayerGetClass(void);

void PlayerReset(Player * _Nonnull self, LCDSprite * _Nonnull sprite);
void PlayerSetCharacter(Player * _Nonnull self, LCDSprite * _Nonnull sprite, PlayerCharacter character);

void PlayerSetClearState(Player * _Nonnull self, LCDSprite * _Nonnull sprite, MELBoolean isAlive);

#endif /* player_h */
