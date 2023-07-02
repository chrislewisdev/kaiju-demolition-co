#include <gb/gb.h>
#include <stdint.h>
#include <gbdk/platform.h>
// Local modules
#include "global.h"
#include "game.h"
#include "title.h"
#include "level-select.h"

/*
* Global state
*/ 
uint8_t input = 0, previousInput = 0;
uint8_t score = 0;
uint8_t applicationState = STATE_TITLE;
uint8_t selectedLevelIndex = 0;
uint16_t unlockedLevels = 0b0000000000000011;

void stateChangeToGame() {
    applicationState = STATE_GAME;
    stateInitGame();
}

void stateChangeToLevelSelect() {
    applicationState = STATE_LEVEL_SELECT;
    stateInitLevelSelect();
}

void main() {
    stateInitTitle();

    while (1) {
        wait_vbl_done();

        previousInput = input;
        input = joypad();

        if (applicationState == STATE_TITLE) {
            stateUpdateTitle();
        } else if (applicationState == STATE_LEVEL_SELECT) {
            stateUpdateLevelSelect();
        } else if (applicationState == STATE_GAME) {
            stateUpdateGame();
        }
    }
}
