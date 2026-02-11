//
//  board.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 19/12/2023.
//

#include "board.h"

#include "boardlocation.h"
#include "cardsprite.h"
#include "explosion.h"
#include "savegame.h"

#include "../gen/spritechip18.h"

typedef enum {
    BoardCellContentEmpty,
    BoardCellContentCard,
    BoardCellContentChip
} BoardCellContent;

static void destroy(LCDSprite * _Nonnull sprite);
static void removalWarningForCardsAtLocations(Board * _Nonnull self, MELIntPointList locations);
static int removeCardsAtLocations(Board * _Nonnull self, MELIntPointList locations);
static BoardCellContent removeCardAtLocation(Board * _Nonnull self, MELIntPoint location);
static MELIntPointList chipLocationsAroundMarkedLocations(Board * _Nonnull self);

static void attachSpriteAtLocation(Board * _Nonnull self, LCDSprite * _Nonnull sprite, MELIntPoint location);
static void detachSpriteAtLocation(Board * _Nonnull self, MELIntPoint location);

static void updateShaking(LCDSprite * _Nonnull sprite);

static void setCount(Board * _Nonnull self, uint8_t count);

static const MELSpriteClass BoardClass = (MELSpriteClass) {
    .name = SpriteClassNameBoard,
    .destroy = destroy,
};

Board * _Nonnull BoardAlloc(const Side side) {
    Board *self = playdate->system->realloc(NULL, sizeof(Board));
    *self = (Board) {
        .super = {
            .class = &BoardClass,
            .frame = (MELRectangle) {
                .origin = {
                    .x = SideOriginLeft[side],
                    .y = TILE_SIZE,
                },
                .size = {
                    .width = BOARD_WIDTH,
                    .height = BOARD_HEIGHT,
                }
            },
        },
        .side = side,
        .grid = LCDSpriteRefListMakeWithInitialCapacity(BOARD_GRID_SIZE),
        .detached = 0,
        .dirty = MELIntPointListEmpty,
        .marked = MELIntPointListEmpty,
    };
    self->identifier = HandIdentifierMake(self);
    memset(self->grid.memory, 0, sizeof(LCDSpriteRef) * BOARD_GRID_SIZE);

    LCDSprite *sprite = playdate->sprite->newSprite();
    self->sprite = sprite;
    playdate->sprite->setImage(sprite, playdate->graphics->newBitmap(BOARD_WIDTH, BOARD_HEIGHT, kColorClear), kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_BOARD);
    playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    playdate->sprite->moveTo(sprite, SideOriginLeft[side] + BOARD_WIDTH / 2, TILE_SIZE + BOARD_HEIGHT / 2);
    playdate->sprite->addSprite(sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return self;
}

static void destroy(LCDSprite * _Nonnull sprite) {
    Board *self = playdate->sprite->getUserdata(sprite);
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);
    LCDSpriteRefListDeinit(&self->grid);
    HandIdentifierDealloc(self->identifier);
    MELIntPointListDeinit(&self->dirty);
    MELIntPointListDeinit(&self->marked);
    MELSpriteDealloc(sprite);
}

CardSprites BoardSpritesForMainCardAndExtraCard(Board * _Nonnull self, Card main, Card extra) {
    self->detached += 2;
#if LOG_NEW_HAND
    playdate->system->logToConsole("New hand, main: %c%c, extra: %c%c",
                                   SuitCharacter[main.suit], RankCharacter[main.rank],
                                   SuitCharacter[extra.suit], RankCharacter[extra.rank]);
#endif
    return (CardSprites) {
        .main = CardSpriteMakeMain(main, self),
        .extra = CardSpriteMakeExtra(extra, self)
    };
}

void BoardSpritesForChips(Board * _Nonnull self, uint32_t chips) {
    if (chips < BOARD_COLUMNS) {
        // Fait tomber quelques jetons.
        int columns[BOARD_COLUMNS] = {0, 1, 2, 3, 4, 5};
        int columnCount = BOARD_COLUMNS;
        for (uint32_t index = 0; index < chips; index++) {
            const int columnIndex = MELRandomInt(columnCount);
            const int column = columns[columnIndex];
            if (columnCount > 1) {
                columnCount--;
                columns[columnIndex] = columns[columnCount];
            }
            LCDSprite *sprite = CardSpriteMakeChip(self, column, 0);
            CardSpriteFall(sprite, LCDSpriteRefListEmpty, CHIP_FALL_INITIAL_SPEED, 0.0f);
        }
        self->detached += chips;
    } else {
        // Fait tomber des lignes complètes.
        const uint32_t count = chips / BOARD_COLUMNS;
        for (int column = 0; column < BOARD_COLUMNS; column++) {
            LCDSpriteRefList tail = LCDSpriteRefListMakeWithInitialCapacity(count);
            for (int index = 0; index < count; index++) {
                LCDSpriteRefListPush(&tail, CardSpriteMakeChip(self, column, index));
            }
            CardSpriteFall(tail.memory[0], tail, CHIP_FALL_INITIAL_SPEED, 0.0f);
        }
        self->detached += BOARD_COLUMNS;
    }
}

MELBoolean BoardIsSpriteOnSomething(Board * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    const MELPoint origin = melSprite->frame.origin;
    const MELIntPoint location = BoardLocationForXAndY(self, origin.x, origin.y);
    return location.y >= (BOARD_ROWS + BOARD_HIDDEN_ROWS) || self->grid.memory[MELIntPointBoardIndex(location)] != NULL;
}

MELBoolean BoardIsSpriteAboveSomething(Board * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    return BoardIsMELSpriteAboveSomething(self, melSprite);
}

MELBoolean BoardIsMELSpriteAboveSomething(Board * _Nonnull self, const MELSprite * _Nonnull melSprite) {
    const MELPoint origin = melSprite->frame.origin;
    const MELIntPoint location = BoardLocationForXAndY(self, origin.x, origin.y + TILE_SIZE / 2 + 1);
    return location.y >= (BOARD_ROWS + BOARD_HIDDEN_ROWS) || self->grid.memory[MELIntPointBoardIndex(location)] != NULL;
}

MELBoolean BoardCanMoveToPoint(Board * _Nonnull self, MELPoint point) {
    const MELIntPoint location = BoardLocationForPoint(self, point);
    if (location.x < 0 || location.x >= BOARD_COLUMNS || location.y < 0 || location.y >= BOARD_ROWS + BOARD_HIDDEN_ROWS) {
        return false;
    }
    return self->grid.memory[MELIntPointBoardIndex(location)] == NULL;
}

MELIntPoint BoardTopOfColumn(Board * _Nonnull self, uint8_t column) {
    MELIntPoint location = (MELIntPoint) {
        .x = column,
        .y = BOARD_ROWS + BOARD_HIDDEN_ROWS - 1,
    };

    while (location.y >= 0 && self->grid.memory[MELIntPointBoardIndex(location)] != NULL) {
        location.y--;
    }

    return location;
}

MELIntPoint BoardLocationForLCDSprite(Board * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    return BoardLocationForPoint(self, melSprite->frame.origin);
}

MELIntPoint BoardLocationForPoint(Board * _Nonnull self, MELPoint point) {
    return BoardLocationForXAndY(self, point.x, point.y);
}

MELIntPoint BoardLocationForXAndY(Board * _Nonnull self, float x, float y) {
    return (MELIntPoint) {
        .x = (int)((x - self->super.frame.origin.x + TILE_SIZE) / TILE_SIZE) - 1,
        .y = (int)((y - self->super.frame.origin.y + TILE_SIZE) / TILE_SIZE) + BOARD_HIDDEN_ROWS - 1,
    };
}

LCDSprite * _Nullable BoardSpriteAtLocation(Board * _Nonnull self, MELIntPoint location) {
    if (location.x >= 0 && location.x < BOARD_COLUMNS && location.y >= 0 && location.y < BOARD_ROWS + BOARD_HIDDEN_ROWS) {
        return self->grid.memory[MELIntPointBoardIndex(location)];
    } else {
        return NULL;
    }
}

MELBoolean BoardLocationIsEmpty(Board * _Nonnull self, MELIntPoint location) {
    return BoardSpriteAtLocation(self, location) == NULL;
}

MELBoolean BoardCardAtLocation(Board * _Nonnull self, MELIntPoint location, Card * _Nonnull card) {
    LCDSprite *sprite = BoardSpriteAtLocation(self, location);
    if (sprite != NULL) {
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.name == SpriteNameCards18) {
            CardSprite *cardSprite = (CardSprite *)melSprite;
            *card = cardSprite->card;
            return true;
        }
    }
    return false;
}

MELBoolean BoardChipAtLocation(Board * _Nonnull self, MELIntPoint location) {
    LCDSprite *sprite = BoardSpriteAtLocation(self, location);
    if (sprite != NULL) {
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.name == SpriteNameChip18) {
            return true;
        }
    }
    return false;
}

void BoardAttachSprite(Board * _Nonnull self, LCDSprite * _Nonnull sprite, LCDSpriteRefList tail) {
    self->detached--;

    LCDSpriteRefList sprites;
    if (tail.count == 0) {
        sprites = LCDSpriteRefListEmpty;
        sprites.memory = &sprite;
        sprites.count = 1;
    } else {
        sprites = tail;
    }

    MELIntPoint location = BoardLocationForLCDSprite(self, sprite);
    location.y = MELIntMin(location.y, BOARD_ROWS + BOARD_HIDDEN_ROWS - 1);

    while (location.y >= 0 && self->grid.memory[MELIntPointBoardIndex(location)] != NULL) {
        location.y--;
    }

    LCDBitmap *image = playdate->sprite->getImage(self->sprite);
    playdate->graphics->pushContext(image);

    const MELRectangle boardFrame = self->super.frame;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        CardSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (location.y >= 0) {
            MELIntPointListPush(&self->dirty, location);

            // Correction de la position du sprite.
            melSprite->super.frame.origin = (MELPoint) {
                .x = boardFrame.origin.x + TILE_SIZE * location.x + TILE_SIZE / 2,
                .y = boardFrame.origin.y + TILE_SIZE * (location.y - BOARD_HIDDEN_ROWS) + TILE_SIZE / 2,
            };

            // Placement dans la grille.
            self->grid.memory[MELIntPointBoardIndex(location)] = sprite;

            // Dessin et suppression du sprite
            attachSpriteAtLocation(self, sprite, location);
        } else {
            // Si l'écran est plein, désallocation du sprite.
            playdate->sprite->setUpdateFunction(sprite, melSprite->super.class->destroy);
        }
        location.y--;
    }

    playdate->graphics->popContext();
    playdate->sprite->markDirty(self->sprite);
}

HandList BoardResolve(Board * _Nonnull self) {
    HandList result = HandListEmpty;
    HandIdentifier *identifier = self->identifier;
    MELIntPointList dirty = self->dirty;

    for (unsigned int index = 0; index < dirty.count; index++) {
        MELIntPoint location = dirty.memory[index];
        Card card;
        if (BoardCardAtLocation(self, location, &card)) {
            HandAndLocationList hands = HandIdentifierGetHandsForCardAtLocation(identifier, card, location, &self->marked);

            for (unsigned int handIndex = 0; handIndex < hands.count; handIndex++) {
                HandAndLocation hand = hands.memory[handIndex];
                HandListPush(&result, hand.hand);
                // TODO: Afficher le nom de la main en sprite à l'emplacement de la main.
            }

            HandAndLocationListDeinit(&hands);
        }
    }
    MELIntPointList chipLocations = chipLocationsAroundMarkedLocations(self);
    MELIntPointListAddAll(&self->marked, chipLocations);
    MELIntPointListDeinit(&chipLocations);

    removalWarningForCardsAtLocations(self, self->marked);
    self->dirty.count = 0;

    return result;
}

void BoardCommit(Board * _Nonnull self) {
    const int removed = removeCardsAtLocations(self, self->marked);
    self->marked.count = 0;

    setCount(self, self->count - removed);
}

void BoardShake(Board * _Nonnull self, float duration, float intensity) {
    Shaker shaker = self->shaker;
    if (shaker.time < shaker.duration) {
        shaker.time = 0.0f;
        shaker.intensity = MELFloatMax(shaker.intensity, intensity);
        self->shaker = shaker;
        return;
    }
    self->shaker = (Shaker) {
        .duration = duration,
        .intensity = intensity,
    };
    playdate->sprite->setUpdateFunction(self->sprite, updateShaking);
}

void BoardBounceCardsAtRow(Board * _Nonnull self, int row) {
    LCDBitmap *image = playdate->sprite->getImage(self->sprite);
    playdate->graphics->pushContext(image);

    const int y = BOARD_ROWS + BOARD_HIDDEN_ROWS - row - 1;
    for (int x = 0; x < BOARD_COLUMNS; x++) {
        const MELIntPoint location = (MELIntPoint) {
            .x = x,
            .y = y
        };
        const int index = MELIntPointBoardIndex(location);
        LCDSprite *sprite = self->grid.memory[index];
        if (sprite) {
            detachSpriteAtLocation(self, location);
            CardSpriteBounce(sprite);
            self->grid.memory[index] = NULL;
        }
    }

    playdate->graphics->popContext();
    playdate->sprite->markDirty(self->sprite);
}

void BoardChangeCardsToChipAtRow(Board * _Nonnull self, int row) {
    loadSpriteChip18Palette();

    LCDBitmap *image = playdate->sprite->getImage(self->sprite);
    playdate->graphics->pushContext(image);

    LCDBitmap *chip = playdate->graphics->getTableBitmap(spriteChip18.palette, 0);

    const int y = BOARD_ROWS + BOARD_HIDDEN_ROWS - row - 1;
    for (int x = 0; x < BOARD_COLUMNS; x++) {
        const MELIntPoint location = (MELIntPoint) {
            .x = x,
            .y = y
        };
        const int index = MELIntPointBoardIndex(location);
        LCDSprite *sprite = self->grid.memory[index];
        if (sprite) {
            playdate->graphics->fillRect(TILE_SIZE * location.x, TILE_SIZE * (location.y - BOARD_HIDDEN_ROWS), TILE_SIZE, TILE_SIZE, kColorClear);
            playdate->graphics->drawBitmap(chip, TILE_SIZE * location.x, TILE_SIZE * (location.y - BOARD_HIDDEN_ROWS), kBitmapUnflipped);
        }
    }

    playdate->graphics->popContext();
    playdate->sprite->markDirty(self->sprite);
}

#pragma mark - Méthodes privées

static void attachSpriteAtLocation(Board * _Nonnull self, LCDSprite * _Nonnull sprite, MELIntPoint location) {
#if LOG_BOARD_ATTACH_DETACH
    CardSprite *cardSprite = playdate->sprite->getUserdata(sprite);
    playdate->system->logToConsole("attach %x at %dx%d, card: %c%c", sprite, location.x, location.y, SuitCharacter[cardSprite->card.suit], RankCharacter[cardSprite->card.rank]);
#endif
    LCDBitmap *card = playdate->sprite->getImage(sprite);
    playdate->graphics->drawBitmap(card, TILE_SIZE * location.x, TILE_SIZE * (location.y - BOARD_HIDDEN_ROWS), kBitmapUnflipped);
    playdate->sprite->removeSprite(sprite);

    if (!CardSpriteHasLanded(sprite)) {
        setCount(self, self->count + 1);
    }
}

static void detachSpriteAtLocation(Board * _Nonnull self, MELIntPoint location) {
    LCDSprite *sprite = BoardSpriteAtLocation(self, location);
    CardSprite *cardSprite = playdate->sprite->getUserdata(sprite);
    // self->grid.memory[MELIntPointBoardIndex(location)] = NULL;

#if LOG_BOARD_ATTACH_DETACH
    playdate->system->logToConsole("detach %x at %dx%d, card: %c%c", sprite, location.x, location.y, SuitCharacter[cardSprite->card.suit], RankCharacter[cardSprite->card.rank]);
#endif

    MELPoint boardOrigin = self->super.frame.origin;
    MELPoint cardOrigin = (MELPoint) {
        .x = boardOrigin.x + TILE_SIZE * location.x + TILE_SIZE / 2,
        .y = boardOrigin.y + TILE_SIZE * (location.y - BOARD_HIDDEN_ROWS) + TILE_SIZE / 2
    };
    cardSprite->super.frame.origin = cardOrigin;

    MELPoint camera = self->camera;
    playdate->sprite->moveTo(sprite, cardOrigin.x + camera.x, cardOrigin.y + camera.y);
    playdate->sprite->addSprite(sprite);

    playdate->graphics->fillRect(TILE_SIZE * location.x, TILE_SIZE * (location.y - BOARD_HIDDEN_ROWS), TILE_SIZE, TILE_SIZE, kColorClear);
}

static void removalWarningForCardsAtLocations(Board * _Nonnull self, MELIntPointList locations) {
    LCDBitmap *image = playdate->sprite->getImage(self->sprite);
    playdate->graphics->pushContext(image);

    for (unsigned int index = 0; index < locations.count; index++) {
        MELIntPoint location = locations.memory[index];
        LCDSprite *sprite = self->grid.memory[MELIntPointBoardIndex(location)];
        if (sprite != NULL) {
            detachSpriteAtLocation(self, location);
            CardSpriteBlink(sprite);
        } else {
            playdate->system->logToConsole("Trying to remove a card at %dx%d of side %d but no card was found.", location.x, location.y, self->side);
        }
    }
#if LOG_COUNT_CHANGES
    playdate->system->logToConsole("Side %d, remove %d cards", self->side, locations.count);
#endif

    playdate->graphics->popContext();
    playdate->sprite->markDirty(self->sprite);
}

static int compareLocations(const void * _Nonnull lhs, const void * _Nonnull rhs) {
    MELIntPoint left = *((MELIntPoint *)lhs);
    MELIntPoint right = *((MELIntPoint *)rhs);
    if (left.y == right.y) {
        return left.x - right.x;
    } else {
        return left.y - right.y;
    }
}

static int removeCardsAtLocations(Board * _Nonnull self, MELIntPointList locations) {
    uint8_t removedCards = 0;
    uint8_t removedChips = 0;

    // Tri les emplacements à supprimer du haut gauche au bas droite pour bien construire
    // les suites de cartes qui tombent. Il faut détacher celles qui sont les plus hautes
    // pour éviter d'avoir des problèmes à la suppression des plus basses.
    qsort(locations.memory, locations.count, sizeof(MELIntPoint), compareLocations);
    for (unsigned int index = 0; index < locations.count; index++) {
        BoardCellContent removedContent = removeCardAtLocation(self, locations.memory[index]);

        if (removedContent == BoardCellContentCard) {
            removedCards++;
        } else if (removedContent == BoardCellContentChip) {
            removedChips++;
        }
    }

    const Side side = self->side;
    const MELBoolean isLeftOrCenterSide = side == SideLeft || side == SideCenter;
    if (isLeftOrCenterSide && removedCards) {
        currentSaveGame.removedCards += removedCards;
        MELEventBusFireEvent(EventLeftSideRemovedCards, removedCards);
    }
    if (isLeftOrCenterSide && removedChips) {
        currentSaveGame.removedChips += removedChips;
        MELEventBusFireEvent(EventLeftSideRemovedChips, removedChips);
    }
    return removedCards + removedChips;
}

/**
 * @returns `true` si l'emplacement supprimé contenait une carte, `false` s'il contenait un jeton.
 */
static BoardCellContent removeCardAtLocation(Board * _Nonnull self, MELIntPoint location) {
    BoardCellContent content = BoardCellContentEmpty;

    int index = MELIntPointBoardIndex(location);
    LCDSpriteRefList grid = self->grid;
    LCDSprite *sprite = grid.memory[index];
    if (sprite) {
        LCDBitmap *image = playdate->sprite->getImage(self->sprite);
        playdate->graphics->pushContext(image);

        CardSprite *melSprite = playdate->sprite->getUserdata(sprite);
        content = melSprite->super.definition.name == SpriteNameCards18 ? BoardCellContentCard : BoardCellContentChip;
        ExplosionConstructor(melSprite->super.frame.origin, (AnimationName)melSprite->card.suit);
        melSprite->super.class->destroy(sprite);
        grid.memory[index] = NULL;

        LCDSpriteRefList tail = LCDSpriteRefListEmpty;
        location.y--;
        index = MELIntPointBoardIndex(location);
        while (location.y >= 0 && grid.memory[index] != NULL) {
            LCDSprite *tailSprite = grid.memory[index];
            LCDSpriteRefListPush(&tail, tailSprite);

            detachSpriteAtLocation(self, location);

            location.y--;
            grid.memory[index] = NULL;
            index = MELIntPointBoardIndex(location);
        }
        if (tail.count > 0) {
            self->detached++;
            CardSpriteFall(tail.memory[0], tail, CARD_FALL_INITIAL_SPEED, 0.25f);
        }

        playdate->graphics->popContext();
        playdate->sprite->markDirty(self->sprite);
    }

    return content;
}

static MELIntPointList chipLocationsAroundMarkedLocations(Board * _Nonnull self) {
    MELIntPointList locations = MELIntPointListEmpty;
    MELBoolean done[BOARD_GRID_SIZE] = {};

    MELIntPointList marked = self->marked;
    for (unsigned int index = 0; index < marked.count; index++) {
        const MELIntPoint location = marked.memory[index];
        for (MELDirection direction = 0; direction < DIRECTION_COUNT; direction++) {
            const MELIntPoint neighbor = MELIntPointAdd(location, MELDirectionIntPoint[direction]);
            const int neighborIndex = MELIntPointBoardIndex(neighbor);
            // NOTE: L'ordre entre chipAtLocation et !done est important car chipAtLocation vérifie si l'indice est correct.
            if (BoardChipAtLocation(self, neighbor) && !done[neighborIndex]) {
                MELIntPointListPush(&locations, neighbor);
                done[neighborIndex] = true;
            }
        }
    }
    return locations;
}

static void updateShaking(LCDSprite * _Nonnull sprite) {
    Board *self = playdate->sprite->getUserdata(sprite);
    Shaker shaker = self->shaker;
    if (shaker.time < shaker.duration) {
        shaker.time += DELTA;
        const float progress = MELEaseInOut(0, shaker.duration, shaker.time);
        const float intensity = (1.0f - progress) * shaker.intensity;
        self->camera = (MELPoint) {
            .y = MELRandomFloat(intensity) - intensity / 2.0f
        };
    } else {
        self->camera = MELPointZero;
        playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    }
    self->shaker = shaker;

    MELPoint camera = self->camera;
    MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x + camera.x + BOARD_WIDTH / 2, origin.y + camera.y + BOARD_HEIGHT / 2);
}

static void setCount(Board * _Nonnull self, uint8_t count) {
    const uint8_t oldCount = self->count;
    self->count = count;
#if LOG_COUNT_CHANGES
    playdate->system->logToConsole("Side %d, count changed: %d -> %d", self->side, (int)oldCount, (int)count);
#endif
#if CHECK_COUNT_CHANGES
    unsigned int realCount = 0;
    for (unsigned int y = 0; y < BOARD_ROWS + BOARD_HIDDEN_ROWS; y++) {
        for (unsigned int x = 0; x < BOARD_COLUMNS; x++) {
            LCDSprite *sprite = self->grid.memory[y * BOARD_COLUMNS + x];
            if (sprite) {
                realCount++;
            }
        }
    }
    if (realCount != count) {
        playdate->system->logToConsole("Bad count update, expected: %d, but was: %d (old count: %d)", realCount, count, oldCount);
    }
#endif
    if (oldCount < BOARD_NEAR_FULL && count >= BOARD_NEAR_FULL) {
        MELEventBusFireEvent(SideMayLoseEvent[self->side], true);
    } else if (oldCount >= BOARD_NEAR_FULL && count < BOARD_NEAR_FULL) {
        MELEventBusFireEvent(SideMayLoseEvent[self->side], false);
    }
}
