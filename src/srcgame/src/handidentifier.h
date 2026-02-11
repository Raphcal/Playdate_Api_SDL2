//
//  handidentifier.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/12/2023.
//

#ifndef handidentifier_h
#define handidentifier_h

#include "common.h"
#include "../lib/melice.h"

#include "card.h"
#include "hand.h"
#include "references.h"

typedef struct board Board;

typedef struct {
    Board * _Nullable board;
    MELIntPointList locations;
    MELBoolean status[BOARD_GRID_SIZE];
} HandIdentifier;

typedef struct {
    Hand hand;
    MELIntPoint location;
} HandAndLocation;

MELListDefine(HandAndLocation);

HandIdentifier * _Nonnull HandIdentifierMake(Board * _Nonnull board);
void HandIdentifierDealloc(HandIdentifier * _Nonnull self);

HandAndLocationList HandIdentifierGetHandsForCardAtLocation(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList * _Nullable locations);

MELIntPointList HandIdentifierSameKindsAsCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore);
MELIntPointList HandIdentifierSameSuitAsCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore);
MELIntPointList HandIdentifierStraightIncludingCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore);

#endif /* handidentifier_h */
