#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef enum {
    GAME_UNKNOWN = 0,
    GAME_EU4,
    GAME_EU5,
    GAME_HOI4
} GameType;

GameType detect_game(void);
int      run_patcher(void);
