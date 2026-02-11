//
//  instantcpu.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 22/05/2024.
//

#include "instantcpu.h"

#include "gameflow.h"
#include "cardsprite.h"
#include "randomcpu.h"

typedef struct {
    MELIntPoint location;
    int score;
} InstanceCpuBestLocation;

static const float kPreferSideScores[BOARD_COLUMNS] = {1.5f, 1.0f, 0.5f, 0.75f, 1.0f, 1.5f};

static InstanceCpuBestLocation instantCpuBestLocationForCard(InstantCpu * _Nonnull self, Card card);

Actor * _Nonnull InstantCpuMake(MELBoolean isAlwaysFast, MELBoolean isFastWhenGoodHandIsFound, int sameKindScore, int sameSuitScore, int straightScore, int rowScore, MELBoolean preferSides) {
    InstantCpu *self = playdate->system->realloc(NULL, sizeof(InstantCpu));
    *self = (InstantCpu) {
        .super = {
            .super = {
                .dealloc = InstantCpuDealloc,
                .update = CpuUpdate,
                .handChanged = InstantCpuHandChanged,
            }
        },
        .isAlwaysFast = isAlwaysFast,
        .isFastWhenGoodHandIsFound = isFastWhenGoodHandIsFound,
        .sameKindScore = sameKindScore,
        .sameSuitScore = sameSuitScore,
        .straightScore = straightScore,
        .rowScore = rowScore,
        .preferSides = preferSides,
    };
    return &self->super.super;
}

Actor * _Nonnull InstantCpuMakeDefault(void) {
    return InstantCpuMake(false, true, 0, 0, 1, 0, false);
}

Actor * _Nonnull InstantCpuAlloc(Actor * _Nonnull actor) {
    InstantCpu *self = (InstantCpu *)actor;
    InstantCpu *copy = playdate->system->realloc(NULL, sizeof(InstantCpu));
    *copy = *self;
    return &copy->super.super;
}

void InstantCpuDealloc(Actor * _Nonnull actor) {
    InstantCpu *self = (InstantCpu *)actor;
    if (self->identifier) {
        HandIdentifierDealloc(self->identifier);
        self->identifier = NULL;
    }
    ActorDealloc(actor);
}

void InstantCpuHandChanged(Actor * _Nonnull actor, Card * _Nonnull hand, Card * _Nonnull nextHand) {
    InstantCpu *self = (InstantCpu *)actor;
    self->super.isPressingDown = self->isAlwaysFast;

    if (self->miss && MELRandomInt(100) < self->miss) {
#if LOG_INSTANT_CPU_MISS
        playdate->system->logToConsole("Miss!");
#endif
        RandomCpuHandChanged(actor, hand, nextHand);
    } else {
        const InstanceCpuBestLocation main = instantCpuBestLocationForCard(self, hand[0]);
        const InstanceCpuBestLocation extra = instantCpuBestLocationForCard(self, hand[1]);
        
        if (abs(main.location.x - extra.location.x) == 1) {
            // Les 2 meilleures cases sont voisines.
            self->super.target = main.location;
            if (main.location.x < extra.location.x) {
                self->super.targetDirection = MELDirectionRight;
            } else {
                self->super.targetDirection = MELDirectionLeft;
            }
        } else if (main.score > extra.score) {
            self->super.target = main.location;
            self->super.targetDirection = MELDirectionUp;
        } else {
            self->super.target = extra.location;
            self->super.targetDirection = MELDirectionDown;
        }
    }

    if (self->randomFast && MELRandomInt(100) < self->randomFast) {
        self->super.isPressingDown = false;
        self->super.willPressDownWhenOverTarget = false;
    }
}

static InstanceCpuBestLocation instantCpuBestLocationForCard(InstantCpu * _Nonnull self, Card card) {
    // TODO: Éviter de faire une boucle par carte.
    GameFlow *gameFlow = self->super.super.gameFlow;
    Board *board = gameFlow->board;

    self->super.willPressDownWhenOverTarget = self->isFastWhenAboveTarget;

    int bestColumn = self->preferSides
        ? MELRandomInt(2) * (BOARD_COLUMNS - 1)
        : MELRandomInt(BOARD_COLUMNS);
    int bestScore = 0;

    HandIdentifier *identifier = self->identifier;
    if (identifier == NULL) {
        self->identifier = identifier = HandIdentifierMake(board);
    }

    for (uint8_t column = 0; column < BOARD_COLUMNS; column++) {
        MELIntPoint top = BoardTopOfColumn(board, column);
        if (top.y > 0) {
            const int row = top.y - BOARD_HIDDEN_ROWS;

            int score = row * self->rowScore;
            if (self->sameKindScore) {
                MELIntPointList sameKind = HandIdentifierSameKindsAsCard(identifier, card, top, MELIntPointListEmpty);
                score += (sameKind.count - 1) * self->sameKindScore;
                MELIntPointListDeinit(&sameKind);
            }
            if (self->sameSuitScore) {
                MELIntPointList sameSuit = HandIdentifierSameSuitAsCard(identifier, card, top, MELIntPointListEmpty);
                score += (sameSuit.count - 1) * self->sameSuitScore;
                MELIntPointListDeinit(&sameSuit);
            }
            if (self->straightScore) {
                MELIntPointList straight = HandIdentifierStraightIncludingCard(identifier, card, top, MELIntPointListEmpty);
                score += (straight.count - 1) * self->straightScore;
                MELIntPointListDeinit(&straight);
            }
            if (column == 2 && self->avoidCenter) {
                // Réduction du score pour éviter de tout mettre au centre.
                score /= 2;
            }
            if (self->preferSides) {
                score = (int) (score * kPreferSideScores[column]);
            }

            if (score > bestScore) {
                bestScore = score;
                bestColumn = column;
                self->super.willPressDownWhenOverTarget = self->super.willPressDownWhenOverTarget || self->isFastWhenGoodHandIsFound;
            }
        }
    }

    return (InstanceCpuBestLocation) {
        .location = (MELIntPoint) {
            .x = bestColumn,
            .y = 0
        },
        .score = bestScore
    };
}
