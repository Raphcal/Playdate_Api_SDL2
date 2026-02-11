//
//  actor.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/12/2023.
//

#include "actor.h"

void ActorUpdate(Actor * _Nonnull self) {
    self->update(self);
}

Actor * _Nonnull ActorAlloc(Actor * _Nonnull self) {
    Actor *copy = playdate->system->realloc(NULL, sizeof(Actor));
    *copy = *self;
    return copy;
}

void ActorDealloc(Actor * _Nonnull self) {
    playdate->system->realloc(self, 0);
}

#pragma mark - Noop

Actor * _Nonnull ActorMakeNoop(void) {
    Actor *self = playdate->system->realloc(NULL, sizeof(Actor));
    *self = (Actor) {
        .dealloc = ActorDealloc,
        .update = ActorUpdateNoop,
        .handChanged = ActorHandChangedNoop,
    };
    return self;
}

void ActorUpdateNoop(Actor * _Nonnull self) {
    // Pas de mise à jour.
}

void ActorHandChangedNoop(Actor * _Nonnull self, Card * _Nonnull hand, Card * _Nonnull nextHand) {
    // Pas d'action.
}

#pragma mark - Player

Actor * _Nonnull ActorMakePlayer(void) {
    Actor *self = playdate->system->realloc(NULL, sizeof(Actor));
    *self = (Actor) {
        .dealloc = ActorDealloc,
        .update = ActorUpdatePlayer,
        .handChanged = ActorHandChangedNoop,
    };
    return self;
}

void ActorUpdatePlayer(Actor * _Nonnull self) {
    self->controller = MELControllerMake();
}
