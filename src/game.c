#include "global.h"
#include "game.h"
// Generated files
#include "gen/ball.h"
#include "gen/pointer.h"
#include "gen/smoke.h"
#include "gen/hud.h"
#include "gen/numbers.h"
#include "gen/kaiju.h"
#include "gen/level0.h"
#include "gen/level1.h"
#include "gen/level2.h"
#include "gen/tileset.h"

#define BALL_ANIMATION_FRAMES   5

uint8_t screenShakeStrength = 0;
uint8_t turnsTaken = 0;
uint8_t cursorPosition = 16;
// Vertical position/speed - uses fixed-point math for realistic-ish acceleration
uint16_t ball_y = CURSOR_Y;
int16_t ball_y_speed = BALL_SPEED_INITIAL;
// Horizontal position/speed - not using fixed point since speed is constant right now
uint8_t ball_x = 16;
int8_t ball_x_speed = 1;
// True when the player is setting up their shot, false otherwise.
uint8_t isReleased = FALSE;
// Storage for all temporary smoke effects
SmokeEffect smokeEffects[SMOKE_SPRITE_RANGE];
// Ball animation variables
uint8_t ballAnimationIndex = 0, ballAnimationTimer = BALL_ANIMATION_FRAMES;

void stateInitGame() {
    if (selectedLevelIndex == 0) {
        set_bkg_data(BKG_TILES_BASE, tileset_TILE_COUNT, tileset_tiles);
        set_bkg_tiles(0, 0, 21, 19, level0_map);
    } else if (selectedLevelIndex == 2) {
        set_bkg_data(BKG_TILES_BASE, tileset_TILE_COUNT, tileset_tiles);
        set_bkg_tiles(0, 0, 21, 19, level2_map);
    } else {
        set_bkg_data(BKG_TILES_BASE, tileset_TILE_COUNT, tileset_tiles);
        set_bkg_tiles(0, 0, 21, 19, level1_map);
    }
    SHOW_BKG;

    set_win_data(NUMBERS_TILES_BASE, numbers_TILE_COUNT, numbers_tiles);

    set_win_data(WIN_TILES_BASE, hud_TILE_COUNT, hud_tiles);
    set_win_based_tiles(0, 0, 20, 1, hud_map, WIN_TILES_BASE);
    move_win(0, 136);
    SHOW_WIN;

    set_sprite_data(0, ball_TILE_COUNT, ball_tiles);

    set_sprite_data(1, pointer_TILE_COUNT, pointer_tiles);
    set_sprite_tile(POINTER_SPRITE, 1);
    move_sprite(POINTER_SPRITE, 16, 16);

    set_sprite_data(2, smoke_TILE_COUNT, smoke_tiles);
    set_sprite_data(3, kaiju_TILE_COUNT, kaiju_tiles);

    set_sprite_tile(BALL_SPRITE, 3);
    move_sprite(BALL_SPRITE, ball_x, ball_y);
    SHOW_SPRITES;

    turnsTaken = 0;
}

void shakeScreen() {
    screenShakeStrength = 1;
}

void screenShakeDoUpdate() {
    move_bkg(screenShakeStrength, screenShakeStrength);
    if (screenShakeStrength > 0) {
        screenShakeStrength--;
    }
}

void spawnSmokeEffect(uint8_t x, uint8_t y) {
    for (uint8_t i = 0; i < SMOKE_SPRITE_RANGE; i++) {
        if (!smokeEffects[i].isDisplayed) {
            smokeEffects[i].isDisplayed = TRUE;
            smokeEffects[i].x = x;
            smokeEffects[i].y = y;
            smokeEffects[i].timer = 15;
            return;
        }
    }
}

void handleCollision(uint8_t x, uint8_t y, uint8_t tileId) {
    if (tileId == 13) {
        for (int8_t dy = -2; dy <= 2; dy++) {
            for (int8_t dx = -2; dx <= 2; dx++) {
                if (dx == 0 && dy == 0) continue;
                if (y + dy > GROUND_TILE_Y) continue;

                uint8_t id = get_bkg_tile_xy(x + dx, y + dy);
                handleCollision(x + dx, y + dy, id);
            }
        }
    } else if (tileId == 14) {
        ball_y_speed = -2 << 8;
        // Don't blow up this tile.
        return;
    }

    set_bkg_tile_xy(x, y, 0);
    spawnSmokeEffect((x + 1) * TILE_SIZE, (y + 2) * TILE_SIZE);
    shakeScreen();
    score++;
}

void ballDoCollision() {
    uint8_t tile_x = (ball_x - 8) / TILE_SIZE;
    uint8_t tile_y = ((ball_y >> 8) - 16) / TILE_SIZE;

    /**
     * For each direction up/down/left/right, if we are moving in that direction
     * and there is a tile there, demolish it.
    */

    uint8_t tile_under = get_bkg_tile_xy(tile_x, tile_y + 1);
    if (tile_under > 1 && ball_y_speed > 0) {
        ball_y_speed = -ball_y_speed / 2;
        if (tile_y + 1 <= GROUND_TILE_Y) {
            handleCollision(tile_x, tile_y + 1, tile_under);
        }
    }

    uint8_t tile_above = get_bkg_tile_xy(tile_x, tile_y - 1);
    if (tile_above > 1 && ball_y_speed >> 8 < 0) {
        ball_y_speed = -ball_y_speed;
        if (tile_y - 1 <= GROUND_TILE_Y) {
            handleCollision(tile_x, tile_y - 1, tile_above);
        }
    }
    
    uint8_t tile_right = get_bkg_tile_xy(tile_x + 1, tile_y);
    if (tile_right > 1 && ball_x_speed > 0) {
        ball_x_speed = -ball_x_speed;
        if (tile_y <= GROUND_TILE_Y) {
            handleCollision(tile_x + 1, tile_y, tile_right);
        }
    }
    
    uint8_t tile_left = get_bkg_tile_xy(tile_x - 1, tile_y);
    if (tile_left > 1 && ball_x_speed < 0) {
        ball_x_speed = -ball_x_speed;
        if (tile_y <= GROUND_TILE_Y) {
            handleCollision(tile_x - 1, tile_y, tile_left);
        }
    }
}

void ballDoMovement() {
    ball_y_speed += BALL_SPEED_ACCEL;
    ball_y += ball_y_speed;
    ball_x += ball_x_speed;

    if (ball_x < 8 && ball_x_speed < 0) {
        ball_x_speed = -ball_x_speed;
    } else if (ball_x > 160 && ball_x_speed > 0) {
        ball_x_speed = -ball_x_speed;
    }

    // If out of bounds, reset the ball
    if (ball_y < 4 || (ball_y >> 8) > 129) {
        isReleased = FALSE;
        ball_y_speed = BALL_SPEED_INITIAL;
    }

    ballDoCollision();
}

void ballDoPlayerControls() {
    if (input & J_RIGHT && !(previousInput & J_RIGHT)) {
        cursorPosition += TILE_SIZE;
        ball_x_speed = 1;
    } else if (input & J_LEFT && !(previousInput & J_LEFT)) {
        cursorPosition -= TILE_SIZE;
        ball_x_speed = -1;
    }

    if (input & J_A && !(previousInput & J_A)) {
        isReleased = TRUE;
        turnsTaken++;
    }

    ball_x = cursorPosition;
    ball_y = CURSOR_Y << 8;
}

void ballDoUpdate() {
    if (isReleased) {
        ballDoMovement();
        hide_sprite(POINTER_SPRITE);

        ballAnimationTimer--;
        if (ballAnimationTimer == 0) {
            ballAnimationIndex = ballAnimationIndex == 1 ? 0 : 1;
            ballAnimationTimer = BALL_ANIMATION_FRAMES;
        }
        set_sprite_tile(BALL_SPRITE, 4 + ballAnimationIndex);
    } else {
        ballDoPlayerControls();
        
        move_sprite(POINTER_SPRITE, cursorPosition, 28);
        set_sprite_prop(POINTER_SPRITE, ball_x_speed > 0 ? 0 : S_FLIPX);
        set_sprite_prop(BALL_SPRITE, ball_x_speed > 0 ? 0 : S_FLIPX);
        set_sprite_tile(BALL_SPRITE, 3);
    }

    // Consider moving this into a graphics update function if we start separating things out
    move_sprite(BALL_SPRITE, ball_x, (ball_y >> 8));
}

void smokeDoUpdate() {
    for (uint8_t i = 0; i < SMOKE_SPRITE_RANGE; i++) {
        if (smokeEffects[i].isDisplayed) {
            set_sprite_tile(SMOKE_SPRITE_START + i, 2);
            move_sprite(SMOKE_SPRITE_START + i, smokeEffects[i].x, smokeEffects[i].y);

            smokeEffects[i].timer--;
            if (smokeEffects[i].timer == 0) {
                smokeEffects[i].isDisplayed = FALSE;
            }
        } else {
            hide_sprite(SMOKE_SPRITE_START + i);
        }
    }
}

void scoreDoUpdate() {
    // Zero out all values first
    for (uint8_t i = 0; i < 7; i++) {
        set_win_tile_xy(7 + i, 0, NUMBERS_TILES_BASE);
    }

    // Set three digits accordingly
    uint8_t first_digit = score % 10;
    set_win_tile_xy(11, 0, NUMBERS_TILES_BASE + first_digit);
    uint8_t second_digit = score / 10 % 10;
    set_win_tile_xy(10, 0, NUMBERS_TILES_BASE + second_digit);
    uint8_t third_digit = score / 100 % 10;
    set_win_tile_xy(9, 0, NUMBERS_TILES_BASE + third_digit);
}

void stateUpdateGame() {
    ballDoUpdate();
    smokeDoUpdate();
    screenShakeDoUpdate();
    scoreDoUpdate();

    // Check for state change here
    if (turnsTaken >= GAME_MAX_TURNS && !isReleased) {
        stateChangeToLevelSelect();
    }
}
