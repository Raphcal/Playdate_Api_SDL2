//
//  board.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 19/12/2023.
//

#ifndef board_h
#define board_h

#include "common.h"

#include "references.h"
#include "side.h"
#include "card.h"
#include "handidentifier.h"

typedef struct board Board;

typedef struct shaker {
    float duration;
    float time;
    float intensity;
} Shaker;

typedef struct board {
    MELSprite super;
    LCDSprite * _Nullable sprite;
    LCDSpriteRefList grid;
    HandIdentifier * _Nullable identifier;
    Shaker shaker;
    MELPoint camera;
    MELIntPointList dirty;
    MELIntPointList marked;
    uint8_t count;
    uint8_t detached;
    Side side;
} Board;

typedef struct {
    LCDSprite * _Nullable main;
    LCDSprite * _Nullable extra;
} CardSprites;

Board * _Nonnull BoardAlloc(const Side side);
CardSprites BoardSpritesForMainCardAndExtraCard(Board * _Nonnull self, Card main, Card extra);
void BoardSpritesForChips(Board * _Nonnull self, uint32_t chips);
MELBoolean BoardIsSpriteOnSomething(Board * _Nonnull self, LCDSprite * _Nonnull sprite);
MELBoolean BoardIsSpriteAboveSomething(Board * _Nonnull self, LCDSprite * _Nonnull sprite);
MELBoolean BoardIsMELSpriteAboveSomething(Board * _Nonnull self, const MELSprite * _Nonnull melSprite);
MELBoolean BoardCanMoveToPoint(Board * _Nonnull self, MELPoint point);
MELIntPoint BoardTopOfColumn(Board * _Nonnull self, uint8_t column);
MELIntPoint BoardLocationForLCDSprite(Board * _Nonnull self, LCDSprite * _Nonnull sprite);
MELIntPoint BoardLocationForPoint(Board * _Nonnull self, MELPoint point);
MELIntPoint BoardLocationForXAndY(Board * _Nonnull self, float x, float y);
LCDSprite * _Nullable BoardSpriteAtLocation(Board * _Nonnull self, MELIntPoint point);
MELBoolean BoardLocationIsEmpty(Board * _Nonnull self, MELIntPoint location);
MELBoolean BoardCardAtLocation(Board * _Nonnull self, MELIntPoint location, Card * _Nonnull card);
MELBoolean BoardChipAtLocation(Board * _Nonnull self, MELIntPoint location);
void BoardAttachSprite(Board * _Nonnull self, LCDSprite * _Nonnull sprite, LCDSpriteRefList tail);
HandList BoardResolve(Board * _Nonnull self);
void BoardCommit(Board * _Nonnull self);
void BoardShake(Board * _Nonnull self, float duration, float intensity);

void BoardBounceCardsAtRow(Board * _Nonnull self, int row);
void BoardChangeCardsToChipAtRow(Board * _Nonnull self, int row);

#endif /* board_h */
