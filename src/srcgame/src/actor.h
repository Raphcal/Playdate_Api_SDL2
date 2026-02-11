//
//  actor.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/12/2023.
//

#ifndef actor_h
#define actor_h

#include "common.h"
#include "../lib/melice.h"

#include "card.h"

typedef struct gameflow GameFlow;
typedef struct actor Actor;

typedef struct actor {
    MELController controller;
    GameFlow * _Nullable gameFlow;
    Actor * _Nonnull (* _Nonnull alloc)(Actor * _Nonnull self);
    void (* _Nonnull dealloc)(Actor * _Nonnull self);
    void (* _Nonnull update)(Actor * _Nonnull self);
    void (* _Nonnull handChanged)(Actor * _Nonnull self, Card * _Nonnull hand, Card * _Nonnull nextHand);
} Actor;

Actor * _Nonnull ActorMakeNoop(void);
Actor * _Nonnull ActorMakePlayer(void);

void ActorHandChangedNoop(Actor * _Nonnull self, Card * _Nonnull hand, Card * _Nonnull nextHand);
void ActorUpdateNoop(Actor * _Nonnull self);
void ActorUpdatePlayer(Actor * _Nonnull self);

Actor * _Nonnull ActorAlloc(Actor * _Nonnull self);
void ActorDealloc(Actor * _Nonnull self);
void ActorUpdate(Actor * _Nonnull self);

#endif /* actor_h */
