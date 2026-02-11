//
//  gameflow.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#include "gameflow.h"

#include "image.h"
#include "cardsprite.h"
#include "magic_ball.h"
#include "win_lose_message.h"
#include "samples.h"
#include "savegame.h"
#include "text.h"
#include "stride.h"

#include "../gen/spritewinlosemessage.h"
#include "../gen/spriteupcoming.h"
#include "../gen/spritecards18.h"

static const int kLoseIndex = BOARD_HIDDEN_ROWS * BOARD_COLUMNS + BOARD_START_COLUMN;

static const MELLocalizedString kChainCount = (MELLocalizedString) {
    .text = {
        "%dchain",
        "%dcombo",
        "%dれんさ",
        "%dcombo"
    }
};

static void updateInitial(GameFlow * _Nonnull self);
static void updateNewHand(GameFlow * _Nonnull self);
static void updatePlay(GameFlow * _Nonnull self);
static void updateLockDelay(GameFlow * _Nonnull self);
static void updateChain(GameFlow * _Nonnull self);
static void updateResolve(GameFlow * _Nonnull self);
static void updatePause(GameFlow * _Nonnull self);
static void updateCommit(GameFlow * _Nonnull self);
static void updateDefend(GameFlow * _Nonnull self);
static void updateChipFall(GameFlow * _Nonnull self);
static void updateNoop(GameFlow * _Nonnull self);

static void updateReceivedChipsPreview(GameFlow * _Nonnull self);

void GameFlowInit(GameFlow * _Nonnull self, const Side side, Board * _Nullable board, CardList cards) {
    *self = (GameFlow) {
        .side = side,
        .board = board,
        .cards = cards,
        .state = GameFlowStateInitial,
        .nextState = GameFlowStateNewHand,
        .update = updateInitial,
    };
}

void GameFlowDeinit(GameFlow * _Nonnull self) {
    MELEventBusRemoveListeners(self);
    LCDSpriteRefListDeinit(&self->preview);
    LCDSpriteRefListDeinit(&self->chipPreview);
    if (self->actor) {
        self->actor->dealloc(self->actor);
        self->actor = NULL;
    }
    // NOTE: Le board est ajouté à la liste des sprites dans BoardAlloc donc il sera libéré au déinit de la scène.
}

void GameFlowPopulateWithCards(GameFlow * _Nonnull self, const char * _Nonnull cards, unsigned int length) {
    const int rows = length / BOARD_COLUMNS;

    Board *board = self->board;
    MELRectangle boardFrame = board->super.frame;
    for (int y = BOARD_ROWS + BOARD_HIDDEN_ROWS - rows; y < BOARD_ROWS + BOARD_HIDDEN_ROWS; y++) {
        for (int x = 0; x < BOARD_COLUMNS; x++) {
            LCDSprite *cardSprite;
            const char cardChar = *cards;
            cards++;
            if (cardChar == '_') {
                continue;
            } else if (cardChar == 'z') {
                cardSprite = CardSpriteMakeChip(board, x, 0);
            } else {
                cardSprite = CardSpriteMakeMain(CardForCharacter(cardChar), board);
                CardSpriteSetController(cardSprite, &self->actor->controller);
            }
            MELSprite *melSprite = playdate->sprite->getUserdata(cardSprite);
            melSprite->frame.origin = (MELPoint) {
                .x = boardFrame.origin.x + TILE_SIZE * x + TILE_SIZE / 2,
                .y = boardFrame.origin.y + TILE_SIZE * (y - BOARD_HIDDEN_ROWS) + TILE_SIZE / 2,
            };
            CardSpriteStop(cardSprite);
            BoardAttachSprite(board, cardSprite, LCDSpriteRefListEmpty);
        }
    }
    board->detached = 0;
}

static void setState(GameFlow * _Nonnull self, GameFlowState state) {
    self->state = state;
    switch (state) {
        case GameFlowStateInitial:
            self->update = updateInitial;
            break;
        case GameFlowStateNewHand:
            self->update = updateNewHand;
            break;
        case GameFlowStatePlay:
            self->update = updatePlay;
            break;
        case GameFlowStateLockDelay:
            self->update = updateLockDelay;
            break;
        case GameFlowStateChain:
            self->update = updateChain;
            break;
        case GameFlowStateResolve:
            self->update = updateResolve;
            break;
        case GameFlowStatePause:
            self->update = updatePause;
            break;
        case GameFlowStateCommit:
            self->update = updateCommit;
            break;
        case GameFlowStateDefend:
            self->update = updateDefend;
            break;
        case GameFlowStateChipFall:
            self->update = updateChipFall;
            break;
        case GameFlowStateLost:
            playdate->sprite->setVisible(self->cursor, false);
            self->update = updateNoop;
            break;
        case GameFlowStateWon:
            playdate->sprite->setVisible(self->cursor, false);
            self->update = updateNoop;
            break;
    }
}

void GameFlowSetState(GameFlow * _Nonnull self, GameFlowState state) {
    setState(self, state);
}

static void chipsSentFromOtherSide(void * _Nonnull userdata, int count) {
    GameFlow *self = userdata;
    int total = self->receivedChips + count;
    if (total < 0) {
        // TODO: Garder un multiplicateur sur toute le combo en cours ?
        self->receivedChips = 0;
        MagicBallConstructorCounter(self->side, -total * 1.5f);
    } else {
        self->receivedChips = total;
    }
    if (count < 0) {
        self->waitingForDefense = false;
    }
    updateReceivedChipsPreview(self);
}

static void otherSideLost(void * _Nonnull userdata, int _) {
    GameFlow *self = userdata;
    setState(self, GameFlowStateWon);
    WinLoseMessageConstructorWithBoard(self->board, true);
}

static Card drawCard(GameFlow * _Nonnull self) {
    const unsigned int index = self->cardIndex;
    self->cardIndex = (index + 1) % DECK_CARD_COUNT;
    return self->cards.memory[index];
}

static void updateReceivedChipsPreview(GameFlow * _Nonnull self) {
    LCDSpriteRefListDeallocReverse(&self->chipPreview);

    const int chipStack = BOARD_COLUMNS;
    const int starChip = BOARD_COLUMNS * BOARD_ROWS / 2;

    const AnimationName chipAnimation = AnimationNameStand;
    const AnimationName stackAnimation = AnimationNameWalk;
    const AnimationName starAnimation = AnimationNameRun;

    loadSpriteUpcomingPalette();

    uint32_t total = self->receivedChips;
    float x = self->board->super.frame.origin.x + spriteUpcoming.size.width / 2.0f;
    const float y = self->board->super.frame.origin.y - spriteUpcoming.size.height / 2.0f;
    while (total > 0) {
        AnimationName animationName;
        if (total >= starChip) {
            if (self->side == SideRight) {
                MELEventBusFireEvent(EventLeftSideSentStar, 1);
            }
            total -= starChip;
            animationName = starAnimation;
        } else if (total >= chipStack) {
            total -= chipStack;
            animationName = stackAnimation;
        } else {
            total -= 1;
            animationName = chipAnimation;
        }

        LCDSprite *sprite = ImageConstructor((MELPoint) {
            .x = x,
            .y = y
        }, playdate->graphics->getTableBitmap(spriteUpcoming.palette, animationName));
        playdate->sprite->setZIndex(sprite, ZINDEX_CHIP_PREVIEW);
        LCDSpriteRefListPush(&self->chipPreview, sprite);

        x += spriteUpcoming.size.width;
    }
}

#pragma mark - Gestion des mises à jour du jeu.

static void updateInitial(GameFlow * _Nonnull self) {
    self->nextState = GameFlowStateNewHand;
    self->pause = 0.25f;
    setState(self, GameFlowStatePause);

    self->actor->gameFlow = self;

    self->nextHand[0] = drawCard(self);
    self->nextHand[1] = drawCard(self);

    const MELBoolean previewCardsAreOnTheRightOfTheBoard = self->side != SideRight;

    MELRectangle boardFrame = self->board->super.frame;
    for (int index = 0; index < 2; index++) {
        MELPoint origin;
        if (previewCardsAreOnTheRightOfTheBoard) {
            origin.x = boardFrame.origin.x + boardFrame.size.width + TILE_SIZE + TILE_SIZE / 2;
        } else {
            origin.x = boardFrame.origin.x - TILE_SIZE - TILE_SIZE / 2;
        }
        origin.y = boardFrame.origin.y + (1 - index) * TILE_SIZE + TILE_SIZE / 2;
        LCDSprite *sprite = ImageConstructor(origin, CardGetBitmap(self->nextHand[index]));
        playdate->sprite->setZIndex(sprite, ZINDEX_CARDS);
        LCDSpriteRefListPush(&self->preview, sprite);
    }

    // Curseur
    MELSprite *cursor = new(MELSprite);
    LCDSprite *cursorSprite = MELSpriteInitWithCenter(cursor, &spriteCards18, MELPointZero);
    MELSpriteSetAnimation(cursor, AnimationNameJump);
    playdate->sprite->setZIndex(cursorSprite, ZINDEX_CARD_CURSOR);
    playdate->sprite->setVisible(cursorSprite, false);
    playdate->sprite->setUpdateFunction(cursorSprite, MELSpriteUpdate);
    self->cursor = cursorSprite;
    LCDSpriteRefListPush(&currentScene->sprites, cursorSprite);

    MELEventBusAddListener(SideSentChipsEvent[SideOpposite[self->side]], chipsSentFromOtherSide, self);
    MELEventBusAddListener(SideLostEvent[SideOpposite[self->side]], otherSideLost, self);
}

static void updateNewHand(GameFlow * _Nonnull self) {
    Card *nextHand = self->nextHand;
    CardSprites sprites;
    self->hand = sprites = BoardSpritesForMainCardAndExtraCard(self->board, nextHand[0], nextHand[1]);
    self->chainCount = 0;
    self->chips = 0;

    CardSprite *main = playdate->sprite->getUserdata(sprites.main);
    main->cursor = playdate->sprite->getUserdata(self->cursor);
    playdate->sprite->setVisible(self->cursor, true);

    // Nouvelle main
    CardSpriteSetController(sprites.main, &self->actor->controller);
    CardSpriteSetController(sprites.extra, &self->actor->controller);
    CardSpriteSetLinkedSprite(sprites.main, sprites.extra);
    CardSpriteSetLinkedSprite(sprites.extra, sprites.main);

    Card hand[2] = {nextHand[0], nextHand[1]};
    self->nextHand[0] = drawCard(self);
    self->nextHand[1] = drawCard(self);

    // Aperçu de la main suivante
    playdate->sprite->setImage(self->preview.memory[0], CardGetBitmap(self->nextHand[0]), kBitmapUnflipped);
    playdate->sprite->setImage(self->preview.memory[1], CardGetBitmap(self->nextHand[1]), kBitmapUnflipped);

    self->actor->handChanged(self->actor, hand, self->nextHand);

    setState(self, GameFlowStatePlay);
}

static void updatePlay(GameFlow * _Nonnull self) {
    Board *board = self->board;
    CardSprites hand = self->hand;
    CardSprite *extra = playdate->sprite->getUserdata(hand.extra);
    if (BoardIsSpriteAboveSomething(board, hand.main) || (extra->state == CardSpriteStateExtra && BoardIsSpriteAboveSomething(self->board, hand.extra))) {
#if LOG_LOCK_DELAY
        playdate->system->logToConsole("Lock delay start!");
#endif
        CardSpriteLockDelay(hand.main);
        setState(self, GameFlowStateLockDelay);
    }
}

static void updateLockDelay(GameFlow * _Nonnull self) {
    Board *board = self->board;
    CardSprites hand = self->hand;

    CardSprite *main = playdate->sprite->getUserdata(hand.main);

    PDButtons pressing;
    playdate->system->getButtonState(&pressing, NULL, NULL);
    const MELBoolean isPressingDown = pressing & kButtonDown;

    const uint32_t lockDelay = main->lockDelay;
    const MELBoolean timeIsUp = playdate->system->getCurrentTimeMilliseconds() - lockDelay >= LOCK_DELAY;
    if (!timeIsUp && !isPressingDown) {
        // Attente
        return;
    }
#if LOG_LOCK_DELAY
        playdate->system->logToConsole("Lock ended!");
#endif

    if (BoardIsSpriteAboveSomething(board, hand.main)) {
        CardSpriteStop(hand.main);
        CardSpriteFall(hand.extra, LCDSpriteRefListEmpty, CARD_FALL_INITIAL_SPEED, 0.0f);
        BoardAttachSprite(board, hand.main, LCDSpriteRefListEmpty);
        self->hand = (CardSprites) {};
        setState(self, GameFlowStateChain);
        SamplePlay(SampleAttach);
        playdate->sprite->setVisible(self->cursor, false);
        return;
    }

    CardSprite *extra = playdate->sprite->getUserdata(hand.extra);
    if (extra->state == CardSpriteStateExtra && BoardIsSpriteAboveSomething(self->board, hand.extra)) {
        CardSpriteStop(hand.extra);
        CardSpriteFall(hand.main, LCDSpriteRefListEmpty, CARD_FALL_INITIAL_SPEED, 0.0f);
        BoardAttachSprite(board, hand.extra, LCDSpriteRefListEmpty);
        self->hand = (CardSprites) {};
        setState(self, GameFlowStateChain);
        SamplePlay(SampleAttach);
        playdate->sprite->setVisible(self->cursor, false);
    }
}

static void updateChain(GameFlow * _Nonnull self) {
    if (self->board->detached == 0) {
        setState(self, GameFlowStateResolve);
    }
}

static void updateResolve(GameFlow * _Nonnull self) {
    const MELBoolean isLeftOrCenterSide = self->side == SideLeft || self->side == SideCenter;
    HandList hands = BoardResolve(self->board);

    uint32_t chips = self->chips;
    for (unsigned int index = 0; index < hands.count; index++) {
        const Hand hand = hands.memory[index];
        chips += HandGetChipCount(hand);
        if (isLeftOrCenterSide) {
            currentSaveGame.handCount[hand.type]++;
            MELEventBusFireEventVoidPointer(EventLeftSideMadePokerHand, &hand);
        }
    }
    self->chips = chips;

    if (hands.count > 0) {
        const int chainCountStartingAt0 = self->chainCount++;
        SamplePlayWithRate(SampleMagic, 1.0f + chainCountStartingAt0 * 0.25f);

        if (isLeftOrCenterSide) {
            if (currentScene->type == SceneTypeGame) {
                currentSaveGame.bestCombo = MELIntMax(currentSaveGame.bestCombo, chainCountStartingAt0);
            }
            MELEventBusFireEvent(EventLeftSideMadeCombo, chainCountStartingAt0);
        }
    }
    HandListDeinit(&hands);

    if (self->board->marked.count > 0) {
        self->pause = CARD_DISAPPEAR_DURATION;
        self->nextState = GameFlowStateCommit;
        setState(self, GameFlowStatePause);
    } else {
        setState(self, GameFlowStateCommit);
    }
}

static void updatePause(GameFlow * _Nonnull self) {
    self->pause -= DELTA;

    if (self->pause <= 0) {
        setState(self, self->nextState);
    }
}

static void updateCommit(GameFlow * _Nonnull self) {
    Board *board = self->board;
    const MELIntPoint center = MELIntPointListCenter(board->marked);
    BoardCommit(board);

    if (board->grid.memory[kLoseIndex] != NULL) {
        BoardShake(board, 1.0f, 10.0f);
        self->pause = 1.25f;
        self->nextState = GameFlowStateLost;
        setState(self, GameFlowStatePause);

        WinLoseMessageConstructorWithBoard(self->board, false);
        MELEventBusFireEvent(SideLostEvent[self->side], self->side);
        playdate->system->logToConsole("Perdu");
        return;
    }

    const int32_t chips = self->chips;
    if (chips > 0) {
        self->chips = 0;

        const uint32_t chainCount = self->chainCount;
        if (chainCount > 1) {
#if LOG_COMBO
            playdate->system->logToConsole("%dx combo", chainCount);
#endif
            const char *format = MELLocalizedStringGetText(kChainCount);
            char *text = NULL;
            playdate->system->formatString(&text, format, chainCount);

            const float textWidth = playdate->graphics->getTextWidth(fontMagicSmall, text, 99, kUTF8Encoding, 0);
            const MELPoint boardOrigin = board->super.frame.origin;
            const MELPoint textOrigin = (MELPoint) {
                .x = MELFloatBound(textWidth / 2 + 5, boardOrigin.x + center.x * TILE_SIZE + TILE_SIZE / 2.0f, LCD_COLUMNS - textWidth / 2 - 5),
                .y = boardOrigin.y + (center.y - BOARD_HIDDEN_ROWS) * TILE_SIZE + TILE_SIZE / 2.0f,
            };

            // Affichage du compteur de chaînes
            LCDSprite *chainSprite = TextConstructor(textOrigin, fontMagicSmall, kDrawModeCopy, text, 99);
            playdate->system->realloc(text, 0);

            playdate->sprite->setZIndex(chainSprite, ZINDEX_EXPLOSIONS);
            StrideSpriteBy(chainSprite, MELPointMake(0, -32), 0, 1.0f);
            StrideSetDestroyWhenStrideEnds(chainSprite, true);
        }

        const MELPoint boardOrigin = board->super.frame.origin;
        const MELPoint magicBallOrigin = (MELPoint) {
            .x = boardOrigin.x + center.x * TILE_SIZE + TILE_SIZE / 2.0f,
            .y = boardOrigin.y + (center.y - BOARD_HIDDEN_ROWS) * TILE_SIZE + TILE_SIZE / 2.0f,
        };
        const int32_t totalChips = chips * (1 + chainCount * 0.5f);
        if (self->receivedChips > 0) {
            MagicBallConstructorDefend(self->side, totalChips, magicBallOrigin);
            self->waitingForDefense = true;
        } else {
            MagicBallConstructorAttack(self->side, totalChips, magicBallOrigin);
        }
    }

    if (board->detached == 0) {
        const uint32_t receivedChips = self->receivedChips;
        if (receivedChips > 0 && self->waitingForDefense) {
            setState(self, GameFlowStateDefend);
        } else if (receivedChips > 0) {
            BoardSpritesForChips(self->board, receivedChips);
            self->receivedChips = 0;
            updateReceivedChipsPreview(self);
            SamplePlay(SampleChipsDrop);
            setState(self, GameFlowStateChipFall);
        } else {
            if (board->count == 0 && (self->side == SideLeft || self->side == SideCenter)) {
                MELEventBusFireEvent(EventLeftSideClearedBoard, 0);
            }
            setState(self, GameFlowStateNewHand);
        }
    } else {
        setState(self, GameFlowStateChain);
    }
}

static void updateDefend(GameFlow * _Nonnull self) {
    if (self->waitingForDefense) {
        return;
    }
    // NOTE: Pas de chute des jetons en attente après une défense pour laisser la possibilité au joueur de contre-attaquer.
    setState(self, GameFlowStateNewHand);
}

static void updateChipFall(GameFlow * _Nonnull self) {
    if (self->board->detached == 0) {
        SamplePlay(SampleChipsCollision);
        setState(self, GameFlowStateNewHand);
    }
}

static void updateNoop(GameFlow * _Nonnull self) {
    // Aucune action.
}
