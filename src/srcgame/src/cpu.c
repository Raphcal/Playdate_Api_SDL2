//
//  cpu.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 29/12/2023.
//

#include "cpu.h"

#include "gameflow.h"
#include "cardsprite.h"

static void delayNextButtonPress(Cpu * _Nonnull self) {
    self->time = self->timeBetweenButtonPress / 2 + MELRandomFloat(self->timeBetweenButtonPress);
}

Actor * _Nonnull CpuAlloc(Actor * _Nonnull actor) {
    Cpu *self = (Cpu *)actor;
    Cpu *copy = playdate->system->realloc(NULL, sizeof(Cpu));
    *copy = *self;
    delayNextButtonPress(copy);
    return &copy->super;
}

void CpuUpdate(Actor * _Nonnull actor) {
    Cpu *self = (Cpu *)actor;

    const float time = self->time;
    self->time = MELFloatMax(time - DELTA, 0.0f);

    GameFlow *gameFlow = self->super.gameFlow;
    if (!gameFlow || time != 0.0f) {
        // Pas de mouvement tant que le jeu n'est pas initialisé ou pendant l'attente entre 2 coups.
        self->super.controller = (MELController) {};
        return;
    }

    Board *board = gameFlow->board;
    CardSprites hand = gameFlow->hand;

    if (hand.main == NULL || hand.extra == NULL) {
        return;
    }

    const MELIntPoint main = BoardLocationForLCDSprite(board, hand.main);

    MELController controller = (MELController) {};
    controller.axe.x = self->target.x - main.x;
    controller.axe.y = self->isPressingDown || (self->willPressDownWhenOverTarget && self->target.x == main.x);

    const MELDirection targetDirection = self->targetDirection;

    CardSprite *extra = playdate->sprite->getUserdata(hand.extra);
    const MELDirection currentDirection = extra->direction;
    switch (currentDirection) {
        case MELDirectionUp:
            if (targetDirection == MELDirectionRight || targetDirection == MELDirectionDown) {
                controller.pressedA = true;
                delayNextButtonPress(self);
            }
            // NOTE: Faire une propriété "droitier/gaucher" pour gérer la préférence sur les directions en commun.
            else if (targetDirection == MELDirectionLeft || targetDirection == MELDirectionDown) {
                controller.pressedB = true;
                delayNextButtonPress(self);
            }
            break;
        case MELDirectionRight:
            if (targetDirection == MELDirectionDown || targetDirection == MELDirectionLeft) {
                controller.pressedA = true;
                delayNextButtonPress(self);
            }
            else if (targetDirection == MELDirectionUp || targetDirection == MELDirectionLeft) {
                controller.pressedB = true;
                delayNextButtonPress(self);
            }
            break;
        case MELDirectionDown:
            if (targetDirection == MELDirectionLeft || targetDirection == MELDirectionUp) {
                controller.pressedA = true;
                delayNextButtonPress(self);
            }
            else if (targetDirection == MELDirectionRight || targetDirection == MELDirectionUp) {
                controller.pressedB = true;
                delayNextButtonPress(self);
            }
            break;
        case MELDirectionLeft:
            if (targetDirection == MELDirectionUp || targetDirection == MELDirectionRight) {
                controller.pressedA = true;
                delayNextButtonPress(self);
            }
            else if (targetDirection == MELDirectionDown) {
                controller.pressedB = true;
                delayNextButtonPress(self);
            }
            break;
    }

    self->super.controller = controller;
}
