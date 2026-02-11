//
//  handidentifier.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 26/12/2023.
//

#include "handidentifier.h"

#include "board.h"
#include "boardlocation.h"

typedef struct matcher Matcher;

typedef struct matcher {
    Suit suit;
    Rank rank;
    MELBoolean (* _Nullable matches)(Matcher * _Nonnull self, Card card);
} Matcher;

MELListImplement(HandAndLocation);

static const int kFromNowhere = -1;
static const MELDirection kDirections[] = {MELDirectionLeft, MELDirectionUp, MELDirectionRight, MELDirectionDown};
static const int kDirectionCount = 4;

static MELIntPointList sameKindsAsCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore);
static MELIntPointList sameSuitAsCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore);
static MELIntPointList straightIncludingCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore);
static MELBoolean isFlush(HandIdentifier * _Nonnull self, MELIntPointList locations);
static void findCardsMatching(HandIdentifier * _Nonnull self, Matcher matcher, MELIntPoint origin, int from);
static MELIntPointList findStraightFromRank(HandIdentifier * _Nonnull self, Rank rank, Suit suit, int next, MELIntPoint origin, int from);
static void goTo(HandIdentifier * _Nonnull self, MELDirection to, int from, MELIntPoint origin, Matcher matcher);
static MELBoolean canGoTo(HandIdentifier * _Nonnull self, MELDirection to, MELIntPoint origin);
static Rank highestRank(Board * _Nonnull board, MELIntPointList locations);
static void ignoreLocations(HandIdentifier * _Nonnull self, MELIntPointList locations);
static MELIntPointList result(HandIdentifier * _Nonnull self);
static void reset(HandIdentifier * _Nonnull self);

#if ENABLE_TWO_PAIRS
static MELIntPointList pairsAroundLocations(HandIdentifier * _Nonnull self, MELIntPointList locations, MELIntPointList ignore);
static void findPairsAround(HandIdentifier * _Nonnull self, MELIntPoint location, MELIntPoint notInLocation);
#endif

HandIdentifier * _Nonnull HandIdentifierMake(Board * _Nonnull board) {
    HandIdentifier *self = playdate->system->realloc(NULL, sizeof(HandIdentifier));
    *self = (HandIdentifier) {
        .board = board,
    };
    return self;
}

void HandIdentifierDealloc(HandIdentifier * _Nonnull self) {
    MELIntPointListDeinit(&self->locations);
    playdate->system->realloc(self, 0);
}

HandAndLocationList HandIdentifierGetHandsForCardAtLocation(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList * _Nullable givenLocations) {
    HandAndLocationList hands = HandAndLocationListEmpty;

    MELIntPointList locations = givenLocations
        ? *givenLocations
        : MELIntPointListEmpty;

    // Vérification des suites.
    MELIntPointList straight = straightIncludingCard(self, card, location, locations);
    if (straight.count >= 5) {
        HandAndLocationListPush(&hands, (HandAndLocation) {
            .hand = {
                .type = HandTypeStraight,
                .isFlush = isFlush(self, straight),
                .count = straight.count,
                .rank = highestRank(self->board, straight),
            },
            .location = MELIntPointListCenter(straight),
        });
        MELIntPointListAddAll(&locations, straight);
    }
    MELIntPointListDeinit(&straight);

    // Vérification des brelans / carrés / etc.
    MELIntPointList sameKinds = sameKindsAsCard(self, card, location, locations);
    if (sameKinds.count >= 3) {
         HandAndLocationListPush(&hands, (HandAndLocation) {
             .hand = {
                 .type = HandTypeSameKind,
                 .rank = card.rank,
                 .count = sameKinds.count,
                 .isFlush = isFlush(self, sameKinds),
             },
             .location = MELIntPointListCenter(sameKinds),
         });
         MELIntPointListAddAll(&locations, sameKinds);
    }
    #if ENABLE_TWO_PAIRS
    // Vérification des doubles pairs.
    if (sameKinds.count == 2) {
        MELIntPointList pairs = pairsAroundLocations(self, sameKinds, locations);
        // TODO: Ajouter la main !

        if (pairs.count > 0) {
            MELIntPointListAddAll(&locations, sameKinds);
            MELIntPointListAddAll(&locations, pairs);
        }
        MELIntPointListDeinit(&pairs);
    }
    #endif
    MELIntPointListDeinit(&sameKinds);

    // Vérification des couleurs.
    MELIntPointList sameSuit = sameSuitAsCard(self, card, location, locations);
    if (sameSuit.count >= 5) {
        HandAndLocationListPush(&hands, (HandAndLocation) {
            .hand = {
                .type = HandTypeFlush,
                .suit = card.suit,
                .count = sameSuit.count,
                .isFlush = true,
            },
            .location = MELIntPointListCenter(sameSuit),
        });
        MELIntPointListAddAll(&locations, sameSuit);
    }
    MELIntPointListDeinit(&sameSuit);

    if (givenLocations) {
        *givenLocations = locations;
    } else {
        MELIntPointListDeinit(&locations);
    }
    return hands;
}

MELBoolean isSameSuit(Matcher * _Nonnull self, Card card) {
    return self->suit == card.suit;
}
MELBoolean isSameRank(Matcher * _Nonnull self, Card card) {
    return self->rank == card.rank;
}

static Matcher matcherForSuit(Suit suit) {
    return (Matcher) {
        .suit = suit,
        .matches = isSameSuit,
    };
}
static Matcher matcherForRank(Rank rank) {
    return (Matcher) {
        .rank = rank,
        .matches = isSameRank,
    };
}

MELIntPointList HandIdentifierSameKindsAsCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore) {
    return sameKindsAsCard(self, card, location, ignore);
}

static MELIntPointList sameKindsAsCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore) {
    ignoreLocations(self, ignore);
    findCardsMatching(self, matcherForRank(card.rank), location, kFromNowhere);
    return result(self);
}

#if ENABLE_TWO_PAIRS
static MELIntPointList pairsAroundLocations(HandIdentifier * _Nonnull self, MELIntPointList locations, MELIntPointList ignore) {
    ignoreLocations(self, ignore);
    findPairsAround(self, locations.memory[0], locations.memory[1]);
    findPairsAround(self, locations.memory[1], locations.memory[0]);
    return result(self);
}
#endif

MELIntPointList HandIdentifierSameSuitAsCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore) {
    return sameSuitAsCard(self, card, location, ignore);
}

static MELIntPointList sameSuitAsCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore) {
    ignoreLocations(self, ignore);
    findCardsMatching(self, matcherForSuit(card.suit), location, kFromNowhere);
    return result(self);
}

MELIntPointList HandIdentifierStraightIncludingCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore) {
    return straightIncludingCard(self, card, location, ignore);
}

static MELIntPointList straightIncludingCard(HandIdentifier * _Nonnull self, Card card, MELIntPoint location, MELIntPointList ignore) {
    ignoreLocations(self, ignore);

    MELIntPointList lower = findStraightFromRank(self, card.rank, card.suit, -1, location, kFromNowhere);
    MELIntPointListAddAll(&self->locations, lower);
    MELIntPointListDeinit(&lower);

    MELIntPointListPush(&self->locations, location);

    MELIntPointList upper = findStraightFromRank(self, card.rank, card.suit, 1, location, kFromNowhere);
    MELIntPointListAddAll(&self->locations, upper);
    MELIntPointListDeinit(&upper);

    return result(self);
}

static MELBoolean isFlush(HandIdentifier * _Nonnull self, MELIntPointList locations) {
    Card reference;
    MELBoolean hasReference = false;
    Board *board = self->board;

    for (unsigned int index = 0; index < locations.count; index++) {
        MELIntPoint location = locations.memory[index];
        Card card;
        if (BoardCardAtLocation(board, location, &card)) {
            if (hasReference && card.suit != reference.suit) {
                return false;
            } else if (!hasReference) {
                reference = card;
                hasReference = true;
            }
        }
    }
    return true;
}

#if ENABLE_TWO_PAIRS
static void findPairsAround(HandIdentifier * _Nonnull self, MELIntPoint location, MELIntPoint notInLocation) {
    MELIntPointList oldLocations = MELIntPointListMakeWithList(self->locations);

    self->status[MELIntPointBoardIndex(location)] = true;

    Board *board = self->board;
    for (unsigned int index = 0; index < kDirectionCount; index++) {
        const MELDirection direction = kDirections[index];
        const MELIntPoint nextLocation = MELIntPointAdd(location, MELDirectionIntPoint[direction]);

        Card card;
        if (!MELIntPointEquals(nextLocation, notInLocation) && canGoTo(self, direction, location) && BoardCardAtLocation(board, nextLocation, &card)) {
            findCardsMatching(self, matcherForRank(card.rank), nextLocation, MELDirectionReverses[direction]);
        }

        if (self->locations.count == oldLocations.count + 1) {
            self->locations.count = 0;
            MELIntPointListAddAll(&self->locations, oldLocations);
        } else {
            oldLocations.count = 0;
            MELIntPointListAddAll(&oldLocations, self->locations);
        }
    }
    MELIntPointListDeinit(&oldLocations);
}
#endif

static void findCardsMatching(HandIdentifier * _Nonnull self, Matcher matcher, MELIntPoint origin, int from) {
    MELIntPointListPush(&self->locations, origin);
    self->status[MELIntPointBoardIndex(origin)] = true;

    goTo(self, MELDirectionUp, from, origin, matcher);
    goTo(self, MELDirectionLeft, from, origin, matcher);
    goTo(self, MELDirectionRight, from, origin, matcher);
    goTo(self, MELDirectionDown, from, origin, matcher);
}

static MELIntPointList findStraightFromRank(HandIdentifier * _Nonnull self, Rank rank, Suit suit, int next, MELIntPoint origin, int from) {
    MELIntPointList locations = MELIntPointListEmpty;

    if (from != kFromNowhere) {
        MELIntPointListPush(&locations, origin);
    }

    const int nextRank = rank + next;
    if (nextRank >= 0 && nextRank < RankCount) {
        int maximumSameSuitCount = 0;
        MELIntPointList maximum = MELIntPointListEmpty;

        Board *board = self->board;
        for (unsigned int index = 0; index < kDirectionCount; index++) {
            const MELDirection to = kDirections[index];
            const MELIntPoint nextLocation = MELIntPointAdd(origin, MELDirectionIntPoint[to]);
            Card other;
            if (from != to && canGoTo(self, to, origin) && BoardCardAtLocation(board, nextLocation, &other) && other.rank == nextRank) {
                MELIntPointList result = findStraightFromRank(self, nextRank, suit, next, nextLocation, MELDirectionReverses[to]);

                unsigned int sameSuitCount = 0;
                for (unsigned int resultIndex = 0; resultIndex < result.count; resultIndex++) {
                    Card card;
                    if (BoardCardAtLocation(board, result.memory[resultIndex], &card) && card.suit == suit) {
                        sameSuitCount++;
                    }
                }

                if (result.count > maximum.count || (result.count == maximum.count && sameSuitCount > maximumSameSuitCount)) {
                    MELIntPointListDeinit(&maximum);
                    maximum = result;
                    maximumSameSuitCount = sameSuitCount;
                } else {
                    MELIntPointListDeinit(&result);
                }
            }
        }
        MELIntPointListAddAll(&locations, maximum);
        MELIntPointListDeinit(&maximum);
    }
    return locations;
}

static void goTo(HandIdentifier * _Nonnull self, MELDirection to, int from, MELIntPoint origin, Matcher matcher) {
    Card other;
    const MELIntPoint nextLocation = MELIntPointAdd(origin, MELDirectionIntPoint[to]);
    if (from != to && canGoTo(self, to, origin) && BoardCardAtLocation(self->board, nextLocation, &other) && matcher.matches(&matcher, other)) {
        findCardsMatching(self, matcher, nextLocation, MELDirectionReverses[to]);
    }
}

static MELBoolean canGoTo(HandIdentifier * _Nonnull self, MELDirection to, MELIntPoint origin) {
    const int nextIndex = MELIntPointBoardIndex(MELIntPointAdd(origin, MELDirectionIntPoint[to]));
    switch (to) {
        case MELDirectionLeft:
            return origin.x > 0 && !self->status[nextIndex];
        case MELDirectionRight:
            return origin.x < BOARD_COLUMNS - 1 && !self->status[nextIndex];
        case MELDirectionUp:
            return origin.y > 0 && !self->status[nextIndex];
        case MELDirectionDown:
            return origin.y < BOARD_ROWS + BOARD_HIDDEN_ROWS - 1 && !self->status[nextIndex];
        default:
            return false;
    }
}

static Rank highestRank(Board * _Nonnull board, MELIntPointList locations) {
    Card card;
    Rank rank = RankSeven;
    for (unsigned int index = 0; index < locations.count; index++) {
        BoardCardAtLocation(board, locations.memory[index], &card);
        rank = MELIntMax(rank, card.rank);
    }
    return rank;
}

static void ignoreLocations(HandIdentifier * _Nonnull self, MELIntPointList locations) {
    for (unsigned int index = 0; index < locations.count; index++) {
        self->status[MELIntPointBoardIndex(locations.memory[index])] = true;
    }
}

static MELIntPointList result(HandIdentifier * _Nonnull self) {
    MELIntPointList result = MELIntPointListMakeWithList(self->locations);
    reset(self);
    return result;
}

static void reset(HandIdentifier * _Nonnull self) {
    memset(self->status, false, sizeof(MELBoolean) * BOARD_GRID_SIZE);
    self->locations.count = 0;
}
