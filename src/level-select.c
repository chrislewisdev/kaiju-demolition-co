#include <gb/gb.h>
#include "global.h"
// Generated files
#include "gen/numbers.h"
#include "gen/level-select-screen.h"
#include "gen/cursor.h"

void stateInitLevelSelect() {
    set_bkg_data(0, level_select_screen_TILE_COUNT, level_select_screen_tiles);
    set_bkg_tiles(0, 0, 20, 18, level_select_screen_map);
    SHOW_BKG;
    HIDE_WIN;

    set_bkg_data(NUMBERS_TILES_BASE, numbers_TILE_COUNT, numbers_tiles);

    hide_sprites_range(0, 10);
    set_sprite_data(0, cursor_TILE_COUNT, cursor_tiles);
    set_sprite_tile(0, 0);
    move_sprite(0, 24, 72);
    SHOW_SPRITES;

    // Update the score display
    uint8_t x = 6, y = 15;
    for (uint8_t i = 0; i < 7; i++) {
        set_bkg_tile_xy(x + i, y, NUMBERS_TILES_BASE);
    }
    uint8_t first_digit = score % 10;
    set_bkg_tile_xy(x + 4, y, NUMBERS_TILES_BASE + first_digit);
    uint8_t second_digit = score / 10 % 10;
    set_bkg_tile_xy(x + 3, y, NUMBERS_TILES_BASE + second_digit);
    uint8_t third_digit = score / 100 % 10;
    set_bkg_tile_xy(x + 2, y, NUMBERS_TILES_BASE + third_digit);
}

void stateUpdateLevelSelect() {
    if (input & J_A && !(previousInput & J_A) && (unlockedLevels >> selectedLevelIndex & 1)) {
        stateChangeToGame();
    }

    if (input & J_LEFT && !(previousInput & J_LEFT) && selectedLevelIndex > 0) {
        selectedLevelIndex--;
    } else if (input & J_RIGHT && !(previousInput & J_RIGHT) && selectedLevelIndex < 14) {
        selectedLevelIndex++;
    } else if (input & J_UP && !(previousInput & J_UP) && selectedLevelIndex >= 5) {
        selectedLevelIndex -= 5;
    } else if (input & J_DOWN && !(previousInput & J_DOWN) && selectedLevelIndex <= 9) {
        selectedLevelIndex += 5;
    }

    const uint8_t xStart = 24;
    const uint8_t yStart = 72;
    uint8_t x = xStart + (selectedLevelIndex % 5) * 24;
    uint8_t y = yStart + (selectedLevelIndex / 5) * 24;
    move_sprite(0, x, y);
}
