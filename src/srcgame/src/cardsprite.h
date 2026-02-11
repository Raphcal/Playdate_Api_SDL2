//
//  cardsprite.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 19/12/2023.
//

#ifndef cardsprite_h
#define cardsprite_h

#include "common.h"

#include "card.h"
#include "board.h"
#include "../lib/melice.h"

typedef enum {
    CardSpriteStateMain,
    CardSpriteStateExtra,
    CardSpriteStateMovingLaterally,
    CardSpriteStateRotating,
    CardSpriteStateFalling,
    CardSpriteStateLockDelay,
    CardSpriteStateBlinking,
    CardSpriteStateBouncing,
    CardSpriteStateNoop,
} CardSpriteState;

typedef struct cardsprite CardSprite;

typedef struct cardsprite {
    MELSprite super;
    Board * _Nonnull board;
    MELController * _Nullable controller;
    CardSprite * _Nullable linkedSprite;
    MELSprite * _Nullable cursor;
    CardSpriteState state;
    Card card;
    LCDSpriteRefList tail;
    MELPoint moveTarget;
    uint32_t lockDelay;
    float time;
    float moveTime;
    float distanceOrRotation;
    float mainStart;
    float extraStart;
    /// Nombre de rotations.
    int count;
    MELDirection direction;
    /// Indique si cette carte/jeton s'est déjà posé une première fois.
    /// Permet de mieux compter le remplissage du plateau.
    MELBoolean landed;
    uint8_t floorKickCount;
} CardSprite;

LCDSprite * _Nonnull CardSpriteMakeMain(Card card, Board * _Nonnull board);
LCDSprite * _Nonnull CardSpriteMakeExtra(Card card, Board * _Nonnull board);
LCDSprite * _Nonnull CardSpriteMakeChip(Board * _Nonnull board, uint8_t column, uint8_t tailIndex);

void CardSpriteSetController(LCDSprite * _Nonnull sprite, MELController * _Nonnull controller);
void CardSpriteSetLinkedSprite(LCDSprite * _Nonnull sprite, LCDSprite * _Nonnull linkedSprite);
void CardSpriteLockDelay(LCDSprite * _Nonnull sprite);
void CardSpriteStop(LCDSprite * _Nonnull sprite);
void CardSpriteFall(LCDSprite * _Nonnull sprite, LCDSpriteRefList tail, float initialSpeed, float waitDuration);
void CardSpriteBlink(LCDSprite * _Nonnull sprite);
void CardSpriteBounce(LCDSprite * _Nonnull sprite);
void CardSpriteChangeToChip(LCDSprite * _Nonnull sprite);

void CardSpriteRemoveMainAndExtra(void);

/**
 * Indique si le sprite donné est une carte ou un jeton.
 *
 * @returns `true` si le sprite donné est une carte, `false` s'il s'agit d'un jeton.
 */
MELBoolean CardSpriteIsCard(LCDSprite * _Nonnull sprite);

/**
 * Positionne la valeur `landed` à `true` et renvoie la valeur précédente.
 *
 * @param sprite Instance du sprite d'une carte ou d'un jeton.
 * @returns `false` si la carte/jeton ne s'était pas encore posé, `true` s'il s'était déjà posé avant.
 */
MELBoolean CardSpriteHasLanded(LCDSprite * _Nonnull sprite);

#endif /* cardsprite_h */
