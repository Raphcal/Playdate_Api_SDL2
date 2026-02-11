//
//  common.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/01/2023.
//

#ifndef common_h
#define common_h

#include "pd_api.h"
#include "pd_api/pd_api_scoreboards.h"
#include "../lib/melstd.h"
#include "../lib/language.h"

#define ENABLE_GAME_CARD 0
#define ENABLE_TWO_PAIRS 0
#define ENABLE_OPENING_SHADE 0
#define ENABLE_SCOREBOARD_API 1
#define ENABLE_ENDING_SELECTION_IF_COMPLETED 0
#define ENABLE_ARCADE_BOARD_SERIALIZATION 0

#define MOCK_SCOREBOARD_API 1

#define CHECK_CLASS_CAST 1
#define CHECK_IF_ALREADY_LISTENING 1
#define CHECK_IF_PATTERN_IS_VALID 1
#define CHECK_COUNT_CHANGES 0

#define LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES 0
#define LOG_SPRITE_NOT_FOUND 0
#define LOG_NEW_HAND 0
#define LOG_BOARD_ATTACH_DETACH 0
#define LOG_SAVES 0
#define LOG_MOVES 0
#define LOG_COUNT_CHANGES 0
#define LOG_PATTERN_CPU 0
#define LOG_INSTANT_CPU_MISS 0
#define LOG_SCORE_ATTACK_LEVEL_UP 0
#define LOG_COMBO 0
#define LOG_LOCK_DELAY 0
#define LOG_MUSIC 0

#define TILE_SIZE 18

// Indice du premier fond, les couches suivantes ont BG + index * LAYER_MULTIPLIER
#define ZINDEX_BG 1
#define ZINDEX_LAYER_MULTIPLIER 4
#define ZINDEX_WHITE_GRID 2
#define ZINDEX_BOARD 2
#define ZINDEX_CARDS 3
#define ZINDEX_CARD_CURSOR 4
#define ZINDEX_PORTAITS 6
#define ZINDEX_INGAME_UI 6
#define ZINDEX_DIALOG 10
#define ZINDEX_DIALOG_PRESS_A 11
#define ZINDEX_DIALOG_BUTTON 10
#define ZINDEX_DANSE 12
#define ZINDEX_CHIP_PREVIEW 17
#define ZINDEX_SENDING_CHIP 18
#define ZINDEX_EXPLOSIONS 19
#define ZINDEX_WIN_LOSE 20
#define ZINDEX_SCROLLBAR 25
#define ZINDEX_FADE 100

#define DEFAULT_REFRESH_RATE 30
#define DEFAULT_FRAME_TIME (1.0f / DEFAULT_REFRESH_RATE)
#define SHOW_FRAME_RATE 0

#define TITLE_SCREEN_CARD_FOUNTAIN 0

#define DECK_CARD_COUNT 256

#define BOARD_COLUMNS 6
#define BOARD_ROWS 12
#define BOARD_HIDDEN_ROWS 2
#define BOARD_START_COLUMN 2
#define BOARD_WIDTH (BOARD_COLUMNS * TILE_SIZE)
#define BOARD_HEIGHT (BOARD_ROWS * TILE_SIZE)
#define BOARD_GRID_SIZE (BOARD_COLUMNS * (BOARD_ROWS + BOARD_HIDDEN_ROWS))
#define BOARD_NEAR_FULL (BOARD_COLUMNS * (BOARD_ROWS - 3))

#define CARD_DISAPPEAR_DURATION 0.3f

#define CARD_FALL_INITIAL_SPEED 96.0f
#define CHIP_FALL_INITIAL_SPEED 128.0f
#define FALL_ACCELERATION 300.0f
#define LOCK_DELAY 500
#define LOCK_DELAY_MAX_KICK 3
#define MOVE_DURATION 0.1f

#define SCORE_ATTACK_CHIP_SCORE 100
#define SCORE_ATTACK_BOARD_CLEAR_BONUS 1000
#define SCORE_ATTACK_ATTACH_SCORE 5
#define SCORE_ATTACK_CARD_COUNT_TO_LEVEL_UP 20
#define SCORE_ATTACK_LEVEL_MAX 100
#define SCORE_ATTACK_CHEER_COMBO_MAX 200

extern PlaydateAPI * _Nullable playdate;
extern LCDFont * _Nullable fontMagic;
extern LCDFont * _Nullable fontMagicSmall;
extern LCDFont * _Nullable fontRoobert10Bold;
extern LCDFont * _Nullable fontRoobert11Bold;
extern LCDFont * _Nullable fontRoobert11MonoMediumNumerals;
extern LCDFont * _Nullable fontJfDotK14Bold;
extern LCDFont * _Nullable fontJfDotIzumi16Bold;
extern LCDFont * _Nullable fontNontendoBold;
extern LCDFont * _Nullable currentFont;
extern LCDFont * _Nullable smallFont;
extern PDMenuItem * _Nullable languageMenuItem;
extern int REFRESH_RATE;
extern float DELTA;

void setRefreshRate(int refreshRate);
void setLanguage(MELLanguage language);

#endif /* common_h */
