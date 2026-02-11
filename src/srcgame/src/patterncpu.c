//
//  patterncpu.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 22/05/2024.
//

#include "patterncpu.h"

#include "gameflow.h"
#include "cardsprite.h"
#include "instantcpu.h"

MELListImplement(PatternMatch);

typedef enum {
    PatternCpuScoreNotFound,
    PatternCpuScoreReduceCombo,
    PatternCpuScoreOutsidePattern,
    PatternCpuScoreReduceTrigger,
    PatternCpuScoreLandOnEmptySpace,
    PatternCpuScoreMatch,
    PatternCpuScoreTriggerCombo,
} PatternCpuScore;

typedef struct {
    MELIntPoint main;
    MELIntPoint extra;
    PatternCpuScore score;
    uint8_t comboCount;
} CardPairLocation;

static MELBoolean getRank(LCDSprite * _Nullable sprite, Rank * _Nonnull rank) {
    if (sprite == NULL) {
        return false;
    }
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    if (melSprite->definition.name == SpriteNameCards18) {
        CardSprite *cardSprite = (CardSprite *)melSprite;
        *rank = cardSprite->card.rank;
        return true;
    }
    return false;
}

static MELBoolean patternMatchesAt(const Pattern * _Nonnull pattern, Board * _Nonnull board, int x, int y, PatternMatch * _Nonnull outMatch) {
    PatternMatch match = (PatternMatch) {
        .pattern = pattern,
        .topLeft = {
            .x = x,
            .y = y
        },
    };
    const MELIntSize size = pattern->size;
    const char *patternPattern = pattern->pattern;

    match.comboCount = pattern->variableCount;

    int triggerCount = 0;
    const int patternSize = size.width * size.height;
    for (unsigned int index = 0; index < patternSize; index++) {
        const int patternX = index % size.width;
        const int patternY = index / size.width;
        const char entry = patternPattern[index];
        const MELIntPoint point = (MELIntPoint) {
            .x = x + patternX,
            .y = y + patternY
        };

        LCDSprite *sprite = BoardSpriteAtLocation(board, point);
        if (entry == ' ') {
            continue;
        } else if (entry == '*') {
            triggerCount++;
            if (sprite == NULL) {
                match.usableTrigger++;
                MELIntPointListPush(&match.triggers, point);
            }
            continue;
        }

#if CHECK_IF_PATTERN_IS_VALID
        if (entry < 'A' || entry > 'H') {
            playdate->system->error("Unexpected entry in pattern: %c", entry);
            return false;
        }
#endif

        const uint8_t variable = entry - 'A';
        if (sprite != NULL) {
            Rank rank;
            const MELBoolean thereIsACard = getRank(sprite, &rank);
            const int flag = 1 << variable;
            if (thereIsACard && !(match.mapped & flag)) {
                match.mapping[variable] = rank;
                match.mapped |= flag;
            } else if (!thereIsACard || match.mapping[variable] != rank) {
                if (variable > 0) {
                    match.comboCount = MELIntMin(match.comboCount, variable);
                } else {
                    return  false;
                }
            }
        }
        MELIntPointListPush(&match.locations[variable], point);
    }
    *outMatch = match;

#if LOG_PATTERN_CPU
    if (triggerCount == 0 || match.usableTrigger > 0) {
        playdate->system->logToConsole("New match found for pattern '%s', topLeft: %dx%d, A: %d, B: %d, C: %d, D: %d, E: %d, F: %d", match.pattern->name, match.topLeft.x, match.topLeft.y, match.mapping[0], match.mapping[1], match.mapping[2], match.mapping[3], match.mapping[4], match.mapping[5], match.mapping[6], match.mapping[7]);
    }
#endif

    return triggerCount == 0 || match.usableTrigger > 0;
}

static MELBoolean patternMatches(const Pattern * _Nonnull pattern, Board * _Nonnull board, PatternMatchList * _Nonnull matches) {
    const int columns = BOARD_COLUMNS - pattern->size.width;
    const int rows = BOARD_ROWS + BOARD_HIDDEN_ROWS - pattern->size.height;
    PatternMatch match = (PatternMatch) {};
    for (int y = rows; y >= 0; y--) {
        for (unsigned int x = 0; x <= columns; x++) {
            if (patternMatchesAt(pattern, board, x, y, &match)) {
                PatternMatchListPush(matches, match);
                return true;
            }
        }
    }
    return false;
}

/**
 * Vérifie que le motif est toujours en cours de construction.
 */
static MELBoolean patternStillMatches(PatternMatch * _Nonnull self, Board * _Nonnull board) {
    MELIntPointList points;

    int totalMatchCount = 0;
    int maxCombo = self->comboCount;
    int mapped = self->mapped;
    uint8_t state = 0;
    for (unsigned int variable = 0; variable < maxCombo; variable++) {
        int matchCount = 0;
        points = self->locations[variable];
        const int flag = 1 << variable;
        MELBoolean isMapped = (mapped & flag) != 0;
        Rank expectedRank = self->mapping[variable];
        for (unsigned int index = 0; index < points.count; index++) {
            const MELIntPoint point = points.memory[index];
            Rank rank;
            LCDSprite *sprite = BoardSpriteAtLocation(board, point);
            const MELBoolean thereIsACard = getRank(sprite, &rank);
            if (thereIsACard && !isMapped) {
                isMapped = true;
                mapped |= flag;
                expectedRank = rank;
                self->mapping[variable] = rank;
                matchCount++;
#if LOG_PATTERN_CPU
                playdate->system->logToConsole("Updated mapping for pattern '%s', variable %d = %d", self->pattern->name, variable, rank);
#endif
            } else if ((thereIsACard && rank != expectedRank) || (!thereIsACard && sprite != NULL)) {
                if (variable > 0) {
                    self->comboCount = MELIntMin(self->comboCount, variable);
                } else {
                    return false;
                }
            } else if (thereIsACard) {
                matchCount++;
            }
        }
        if ((variable == 0 && matchCount >= 2) || (variable > 0 && matchCount >= 3)) {
            state |= 1 << variable;
        }
        totalMatchCount += matchCount;
        if (matchCount == 0 && isMapped) {
            // Unlève le mapping
            mapped = (mapped & ~flag);
#if LOG_PATTERN_CPU
                playdate->system->logToConsole("Unsetting mapping for pattern '%s', variable %d because no match was found", self->pattern->name, variable);
#endif
        }
    }
    self->mapped = mapped;
    const int expectedState = 3;
    const MELBoolean isReadyForTrigger = (state & expectedState) == expectedState;
#if LOG_PATTERN_CPU
    if (!self->isReadyForTrigger && isReadyForTrigger) {
        playdate->system->logToConsole("Pattern '%s' is ready for trigger, state: %d, expected: %d (combo count: %d)", self->pattern->name, state, expectedState, self->comboCount);
    }
#endif
    self->isReadyForTrigger = isReadyForTrigger;

    if (mapped != 0 && totalMatchCount == 0) {
#if LOG_PATTERN_CPU
        playdate->system->logToConsole("Pattern '%s' has been cleared!", self->pattern->name);
#endif
        return false;
    }

    int usableTrigger = 0;
    points = self->triggers;
    for (unsigned int index = 0; index < points.count; index++) {
        const MELIntPoint trigger = points.memory[index];
        LCDSprite *sprite = BoardSpriteAtLocation(board, trigger);
        if (sprite == NULL) {
            usableTrigger++;
        }
    }
    self->usableTrigger = usableTrigger;
    return points.count == 0 || usableTrigger > 0;
}

static MELBoolean isEmptyAndAboveGround(MELIntPoint point, Board * _Nonnull board) {
    // L'emplacement donné est vide.
    return BoardSpriteAtLocation(board, point) == NULL
    // Et il y a quelque chose dessous.
    && (point.y == BOARD_ROWS + BOARD_HIDDEN_ROWS - 1 || BoardSpriteAtLocation(board, (MELIntPoint) { .x = point.x, .y = point.y + 1 }) != NULL);
}

static char variableInPatternAtPoint(PatternMatch * _Nonnull self, MELIntPoint point) {
    MELIntPoint pointInsidePattern = MELIntPointSubstract(point, self->topLeft);
    const Pattern *pattern = self->pattern;
    MELIntSize size = pattern->size;
    if (pointInsidePattern.x >= 0 && pointInsidePattern.x < size.width && pointInsidePattern.y >= 0 && pointInsidePattern.y < size.height) {
        return pattern->pattern[size.width * pointInsidePattern.y + pointInsidePattern.x];
    } else {
        return ' ';
    }
}

static int findFirstUnsetFlag(int flags) {
    for (int index = 0; index < RankCount; index++) {
        if (!(flags & (1 << index))) {
            return index;
        }
    }
    playdate->system->logToConsole("Flag overflow");
    return RankCount - 1;
}

static void findBestLocationForMapping(PatternMatch * _Nonnull self, Board * _Nonnull board, int mapping, int otherMapping, CardPairLocation * _Nonnull pairLocation, MELIntPoint * _Nonnull location, MELIntPoint * _Nonnull otherLocation) {
    if (pairLocation->score == PatternCpuScoreMatch) {
        return;
    }
    const MELBoolean isReadyForTrigger = self->isReadyForTrigger;
    const MELBoolean mainIsTrigger = isReadyForTrigger && mapping == self->pattern->triggerVariable;
    if (mainIsTrigger) {
        MELIntPointList *triggers = &self->triggers;
        for (unsigned int index = 0; index < triggers->count; index++) {
            const MELIntPoint point = triggers->memory[index];
            if (isEmptyAndAboveGround(point, board)) {
                pairLocation->score = PatternCpuScoreTriggerCombo;
                pairLocation->comboCount = self->comboCount;
                *location = point;
                *otherLocation = (MELIntPoint) {
                    .x = point.x,
                    .y = point.y - 1
                };
                return;
            }
        }
        return;
    }

    const MELBoolean otherIsTrigger = isReadyForTrigger && otherMapping == self->pattern->triggerVariable;
    MELIntPointList *locations = self->locations + mapping;
    for (unsigned int index = 0; index < locations->count; index++) {
        const MELIntPoint point = locations->memory[index];
        if (!isEmptyAndAboveGround(point, board)) {
            continue;
        }
        const char expectedVariable = otherIsTrigger ? '*' : ('A' + otherMapping);
        // Colonne à gauche
        const int leftColumn = point.x - 1;
        if (leftColumn >= 0) {
            MELIntPoint leftTop = BoardTopOfColumn(board, leftColumn);
            const char patternVariable = variableInPatternAtPoint(self, leftTop);
            if (patternVariable == expectedVariable) {
                // Top score !
                pairLocation->score = otherIsTrigger ? PatternCpuScoreTriggerCombo : PatternCpuScoreMatch;
                pairLocation->comboCount = self->comboCount;
                *location = point;
                *otherLocation = leftTop;
                return;
            }
            else if (patternVariable == ' ' && pairLocation->score < PatternCpuScoreLandOnEmptySpace) {
                pairLocation->score = PatternCpuScoreLandOnEmptySpace;
                pairLocation->comboCount = self->comboCount;
                *location = point;
                *otherLocation = leftTop;
            }
            else if (patternVariable == '*'
                     && self->usableTrigger > 1
                     && pairLocation->score < PatternCpuScoreReduceTrigger) {
                pairLocation->score = PatternCpuScoreReduceTrigger;
                *location = point;
                *otherLocation = leftTop;
            }
            else if (patternVariable >= 'B' && patternVariable <= 'H'
                     && pairLocation->score <= PatternCpuScoreReduceCombo
                     && pairLocation->comboCount < (patternVariable - 'A')) {
                pairLocation->score = PatternCpuScoreReduceCombo;
                pairLocation->comboCount = patternVariable - 'A';
                *location = point;
                *otherLocation = leftTop;
            }
        }
        // Colonne à droite
        const int rightColumn = point.x + 1;
        if (rightColumn < BOARD_COLUMNS) {
            MELIntPoint rightTop = BoardTopOfColumn(board, rightColumn);
            const char patternVariable = variableInPatternAtPoint(self, rightTop);
            if (patternVariable == expectedVariable) {
                // Top score !
                pairLocation->score = otherIsTrigger ? PatternCpuScoreTriggerCombo : PatternCpuScoreMatch;
                *location = point;
                *otherLocation = rightTop;
                return;
            }
            else if (patternVariable == ' ' && pairLocation->score < PatternCpuScoreLandOnEmptySpace) {
                pairLocation->score = PatternCpuScoreLandOnEmptySpace;
                *location = point;
                *otherLocation = rightTop;
            }
            else if (patternVariable >= '*'
                     && self->usableTrigger > 1
                     && pairLocation->score < PatternCpuScoreReduceTrigger) {
                pairLocation->score = PatternCpuScoreReduceTrigger;
                *location = point;
                *otherLocation = rightTop;
            }
            else if (patternVariable >= 'B' && patternVariable <= 'H'
                     && pairLocation->score <= PatternCpuScoreReduceCombo
                     && pairLocation->comboCount < (patternVariable - 'A')) {
                pairLocation->score = PatternCpuScoreReduceCombo;
                pairLocation->comboCount = patternVariable - 'A';
                *location = point;
                *otherLocation = rightTop;
            }
        }
        // Au dessus (le cas "en dessous" est traité avec la gestion de la carte 2).
        if (point.y > 0) {
            const MELIntPoint top = (MELIntPoint) {
                .x = point.x,
                .y = point.y - 1
            };
            const char patternVariable = variableInPatternAtPoint(self, top);
            if (patternVariable == expectedVariable) {
                // Top score !
                pairLocation->score = otherIsTrigger ? PatternCpuScoreTriggerCombo : PatternCpuScoreMatch;
                *location = point;
                *otherLocation = top;
                return;
            }
            else if (patternVariable == ' ' && pairLocation->score < PatternCpuScoreLandOnEmptySpace) {
                pairLocation->score = PatternCpuScoreLandOnEmptySpace;
                *location = point;
                *otherLocation = top;
            }
            else if (patternVariable >= '*'
                     && self->usableTrigger > 1
                     && pairLocation->score < PatternCpuScoreReduceTrigger) {
                pairLocation->score = PatternCpuScoreReduceTrigger;
                *location = point;
                *otherLocation = top;
            }
            else if (patternVariable >= 'B' && patternVariable <= 'H'
                     && pairLocation->score <= PatternCpuScoreReduceCombo
                     && pairLocation->comboCount < (patternVariable - 'A')) {
                pairLocation->score = PatternCpuScoreReduceCombo;
                pairLocation->comboCount = patternVariable - 'A';
                *location = point;
                *otherLocation = top;
            }
        }
    }
}

static CardPairLocation findLocationInsidePattern(PatternMatch * _Nonnull self, Card * _Nonnull cards, Board * _Nonnull board) {
    Rank *mapping = self->mapping;
    int cardMapping[2] = {0, 0};

    // Recherche la correspondance entre les cartes et les variables du motif.
    int mapped = self->mapped;
    uint8_t found = 0;
    for (unsigned int index = 0; index < RankCount; index++) {
        if (!(mapped & (1 << index))) {
            continue;
        }
        if (mapping[index] == cards[0].rank) {
            cardMapping[0] = index;
            found |= 1;
        }
        if (mapping[index] == cards[1].rank) {
            cardMapping[1] = index;
            found |= 2;
        }
    }

    // Si non trouvé, map les cartes avec les variables inutilisées.
    if (!(found & 1)) {
        const int variable = findFirstUnsetFlag(mapped);
        if (variable >= RankCount) {
            playdate->system->error("Flag overflow: %d, mapped: %d", variable, mapped);
        }
        cardMapping[0] = variable;
        mapping[variable] = cards[0].rank;
        mapped |= 1 << variable;

        if (cards[0].rank == cards[1].rank) {
            cardMapping[1] = variable;
            found |= 2;
        }
    }
    if (!(found & 2)) {
        const int variable = findFirstUnsetFlag(mapped);
        if (variable >= RankCount) {
            playdate->system->error("Flag overflow: %d, mapped: %d", variable, mapped);
        }
        cardMapping[1] = variable;
        mapping[variable] = cards[1].rank;
        mapped |= 1 << variable;
    }

    self->mapped = mapped;

    // Recherche du meilleur emplacement pour les 2 cartes.
#if LOG_PATTERN_CPU
    playdate->system->logToConsole("Searching in pattern: '%s' best location for main: rank=%d var=%d/%d, extra: rank=%d var=%d/%d", self->pattern->name, cards[0].rank, cardMapping[0], mapping[cardMapping[0]], cards[1].rank, cardMapping[1], mapping[cardMapping[1]]);
#endif

    CardPairLocation bestLocation = (CardPairLocation) {
        .score = PatternCpuScoreNotFound,
        .comboCount = 0,
    };
    if (self->topLeft.x > 0) {
        // Place à gauche du motif (avant topLeft.x)
        bestLocation.main.x = bestLocation.extra.x = MELRandomInt(self->topLeft.x);
        bestLocation.score = PatternCpuScoreOutsidePattern;
    } else if (self->pattern->size.width < BOARD_COLUMNS) {
        // Place à droite du motif
        bestLocation.main.x = bestLocation.extra.x = MELRandomInt(BOARD_COLUMNS - self->pattern->size.width) + self->pattern->size.width;
        bestLocation.score = PatternCpuScoreOutsidePattern;
    } else {
        // Place à droite.
        bestLocation.main.x = bestLocation.extra.x = BOARD_COLUMNS - 1;
    }

    findBestLocationForMapping(self, board, cardMapping[0], cardMapping[1], &bestLocation, &bestLocation.main, &bestLocation.extra);
    findBestLocationForMapping(self, board, cardMapping[1], cardMapping[0], &bestLocation, &bestLocation.extra, &bestLocation.main);

    if (bestLocation.score == PatternCpuScoreNotFound) {
        // Essaye d'écraser les variables les plus grandes.
        findBestLocationForMapping(self, board, self->comboCount - 1, self->comboCount - 1, &bestLocation, &bestLocation.main, &bestLocation.extra);
    }

#if LOG_PATTERN_CPU
    playdate->system->logToConsole("Score: %d, main: %dx%d, extra: %dx%d", bestLocation.score, bestLocation.main.x, bestLocation.main.y, bestLocation.extra.x, bestLocation.extra.y);
    if (bestLocation.comboCount > 0 && bestLocation.comboCount < self->comboCount) {
        playdate->system->logToConsole("Will reduce max combo to %d", bestLocation.comboCount);
    }
#endif
    return bestLocation;
}

static void findBestTargetAndDirection(PatternCpu * _Nonnull self, Card * _Nonnull hand) {
    Board *board = self->super.super.super.gameFlow->board;
    PatternMatchList matches = self->matches;

    CardPairLocation location = {};
    for (unsigned int index = 0; index < matches.count; index++) {
        location = findLocationInsidePattern(matches.memory + index, hand, board);
        if (location.score == PatternCpuScoreMatch) {
            break;
        }
    }
    self->super.super.willPressDownWhenOverTarget = self->super.isFastWhenAboveTarget || (self->super.isFastWhenGoodHandIsFound && location.score);
    self->super.super.target.x = location.main.x;
    switch (location.main.x - location.extra.x) {
        case -1:
            // [Main][Extra]
            self->super.super.targetDirection = MELDirectionRight;
            break;
        case 1:
            // [Extra][Main]
            self->super.super.targetDirection = MELDirectionLeft;
            break;
        default:
            self->super.super.targetDirection = location.extra.y < location.main.y
                ? MELDirectionUp
                : MELDirectionDown;
            break;
    }
}

Actor * _Nonnull PatternCpuAlloc(Actor * _Nonnull actor) {
    PatternCpu *source = (PatternCpu *)actor;
    PatternCpu *self = playdate->system->realloc(NULL, sizeof(PatternCpu));
    *self = *source;
    self->index = self->patterns.count;
    return &self->super.super.super;
}

void PatternCpuDealloc(Actor * _Nonnull actor) {
    PatternCpu *self = (PatternCpu *)actor;
    PatternMatchListDeinit(&self->matches);
    InstantCpuDealloc(actor);
}

void PatternCpuHandChanged(Actor * _Nonnull actor, Card * _Nonnull hand, Card * _Nonnull nextHand) {
    PatternCpu *self = (PatternCpu *)actor;
    self->super.super.isPressingDown = self->super.isAlwaysFast;

    Board *board = self->super.super.super.gameFlow->board;

    const int switchToInstantCpuLimit = self->switchToInstantCpuLimit == 0 ? BOARD_COLUMNS * BOARD_ROWS / 2 : self->switchToInstantCpuLimit;
    const int backToPatternCpuLimit = self->backToPatternCpuLimit == 0 ? BOARD_COLUMNS * 2 : self->backToPatternCpuLimit;
    
    const int boardCount = board->count;
#if LOG_PATTERN_CPU
    const MELBoolean wasInstantCpuMode = self->isInstantCpuMode;
#endif
    self->isInstantCpuMode = (!self->isInstantCpuMode && boardCount >= switchToInstantCpuLimit) || (self->isInstantCpuMode && boardCount >= backToPatternCpuLimit);

#if LOG_PATTERN_CPU
    if (self->isInstantCpuMode != wasInstantCpuMode) {
        playdate->system->logToConsole("InstantCpuMode changed, was: %s, changed to: %s", wasInstantCpuMode ? "true" : "false", self->isInstantCpuMode ? "true" : "false");
    }
#endif

    if (self->isInstantCpuMode) {
        self->matches.count = 0;
        InstantCpuHandChanged(actor, self->hand, self->hand);
        return;
    }

    PatternMatchList matches = self->matches;
    // TODO: Vérifier 1 still match par frame ?
    for (int index = matches.count - 1; index >= 0; index--) {
        if (!patternStillMatches(matches.memory + index, board)) {
            PatternMatchListRemoveSwap(&matches, index);
        }
    }
    self->matches = matches;

    if (matches.count == 0 || matches.count < self->matchLimit) {
        // Revérification des motifs.
        self->index = 0;
        self->hand[0] = hand[0];
        self->hand[1] = hand[1];
        self->super.super.target.x = 0;
        self->super.super.targetDirection = MELDirectionUp;
    } else {
        findBestTargetAndDirection(self, hand);
    }
}

void PatternCpuUpdate(Actor * _Nonnull actor) {
    PatternCpu *self = (PatternCpu *)actor;

    if (!self->isInstantCpuMode) {
        int index = self->index;
        PatternConstList patterns = self->patterns;
        if (index < patterns.count && self->super.super.super.gameFlow) {
            const Pattern *pattern = patterns.memory + index;
            Board *board = self->super.super.super.gameFlow->board;
            patternMatches(pattern, board, &self->matches);
            self->index = index + 1;
            
            if (self->index == patterns.count || self->matches.count == self->matchLimit) {
                self->index = patterns.count;
                findBestTargetAndDirection(self, self->hand);
            }
        }
    }

    CpuUpdate(actor);
}
