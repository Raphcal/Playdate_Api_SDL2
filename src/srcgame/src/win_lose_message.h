//
//  win_lose_message.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 13/01/2024.
//

#ifndef win_lose_message_h
#define win_lose_message_h

#include "common.h"
#include "../lib/melice.h"

#include "board.h"

LCDSprite * _Nonnull WinLoseMessageConstructorWithBoard(Board * _Nonnull board, MELBoolean hasWin);

LCDSprite * _Nonnull WinLoseMessageConstructor(MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);
MELSprite * _Nullable WinLoseMessageLoader(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

#endif /* win_lose_message_h */
