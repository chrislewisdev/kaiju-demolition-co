#include <gb/gb.h>
#include "global.h"
// Generated files
#include "gen/numbers.h"
#include "gen/level-select-screen.h"
#include "gen/cursor.h"

#define IS_UNLOCKED(ID) (unlockedLevels >> ID) & 1

void renderNumber(uint8_t value, uint8_t x, uint8_t y) {
    for (uint8_t i = 0; i < 5; i++) {
        set_bkg_tile_xy(x + i, y, NUMBERS_TILES_BASE);
    }
    uint8_t first_digit = value % 10;
    set_bkg_tile_xy(x + 2, y, NUMBERS_TILES_BASE + first_digit);
    uint8_t second_digit = value / 10 % 10;
    set_bkg_tile_xy(x + 1, y, NUMBERS_TILES_BASE + second_digit);
    uint8_t third_digit = value / 100 % 10;
    set_bkg_tile_xy(x, y, NUMBERS_TILES_BASE + third_digit);
}

void showUnlockedLevels() {
    for (uint8_t levelId = 2; levelId < 15; levelId++) {
        if (IS_UNLOCKED(levelId)) {
            const uint8_t xStart = 3;
            const uint8_t yStart = 6;
            uint8_t x = xStart + (levelId % 5) * 3;
            uint8_t y = yStart + (levelId / 5) * 3;

            set_bkg_tile_xy(x, y, 0x1d);
            set_bkg_tile_xy(x+1, y, 0x1e);
            set_bkg_tile_xy(x, y+1, 0x23);
            set_bkg_tile_xy(x+1, y+1, 0x24);
        }
    }
}

void stateInitLevelSelect() {
    set_bkg_data(0, level_select_screen_TILE_COUNT, level_select_screen_tiles);
    set_bkg_tiles(0, 0, 20, 18, level_select_screen_map);
    SHOW_BKG;
    HIDE_WIN;

    set_bkg_data(NUMBERS_TILES_BASE, numbers_TILE_COUNT, numbers_tiles);

    hide_sprites_range(0, 10);
    set_sprite_data(0, cursor_TILE_COUNT, cursor_tiles);
    set_sprite_tile(0, 0);
    set_sprite_prop(0, 0);
    move_sprite(0, 24, 72);
    SHOW_SPRITES;

    showUnlockedLevels();
}

void stateUpdateLevelSelect() {
    if (input & J_LEFT && !(previousInput & J_LEFT) && selectedLevelIndex > 0) {
        selectedLevelIndex--;
    } else if (input & J_RIGHT && !(previousInput & J_RIGHT) && selectedLevelIndex < 14) {
        selectedLevelIndex++;
    } else if (input & J_UP && !(previousInput & J_UP) && selectedLevelIndex >= 5) {
        selectedLevelIndex -= 5;
    } else if (input & J_DOWN && !(previousInput & J_DOWN) && selectedLevelIndex <= 9) {
        selectedLevelIndex += 5;
    }

    // Move cursor
    const uint8_t xStart = 24;
    const uint8_t yStart = 72;
    uint8_t x = xStart + (selectedLevelIndex % 5) * 24;
    uint8_t y = yStart + (selectedLevelIndex / 5) * 24;
    move_sprite(0, x, y);

    // Update cost display
    uint8_t cost = selectedLevelIndex >= 2 ? 1 : 0;
    renderNumber(cost, 12, 16);

    if (input & J_A && !(previousInput & J_A)) {
        if (IS_UNLOCKED(selectedLevelIndex)) {
            stateChangeToGame();
            return;
        } else if (score >= cost) {
            score -= cost;
            unlockedLevels |= 1 << selectedLevelIndex;
            showUnlockedLevels();
        }
    }

    renderNumber(score, 4, 16);
}
