#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <gb/gb.h>

#define NUMBERS_TILES_BASE  50
#define TILE_SIZE           8
#define GROUND_TILE_Y       14
#define BKG_TILES_BASE      0
#define WIN_TILES_BASE      30
#define GAME_MAX_TURNS      3
// States
#define STATE_TITLE         0
#define STATE_GAME          1
#define STATE_LEVEL_SELECT  2

// Persistent data
extern uint8_t score;
extern uint16_t unlockedLevels;

extern uint8_t input, previousInput;
extern uint8_t selectedLevelIndex;

void stateChangeToGame();
void stateChangeToLevelSelect();
void initialiseMemory();

#endif
