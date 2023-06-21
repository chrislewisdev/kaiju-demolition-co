#include <gb/gb.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include "gen/building-bg.h"
#include "gen/ball.h"
#include "gen/pointer.h"

#define BALL_SPRITE     0
#define POINTER_SPRITE  1
#define CURSOR_Y        16
#define BALL_SPEED_INITIAL  25
#define BALL_SPEED_ACCEL    10
#define TILE_SIZE       8

/*
* Global state
*/ 
uint8_t cursorPosition = 16;
// Vertical position/speed - uses fixed-point math for realistic-ish acceleration
uint16_t ball_y = CURSOR_Y;
int16_t ball_y_speed = BALL_SPEED_INITIAL;
// Horizontal position/speed - not using fixed point since speed is constant right now
uint8_t ball_x = 16;
int8_t ball_x_speed = 1;
// True when the player is setting up their shot, false otherwise.
uint8_t isReleased = FALSE;
uint8_t input = 0, previousInput = 0;

void ballDoCollision() {
    uint8_t tile_x = (ball_x - 8) / TILE_SIZE;
    uint8_t tile_y = ((ball_y >> 8) - 16) / TILE_SIZE;

    /**
     * For each direction up/down/left/right, if we are moving in that direction
     * and there is a tile there, demolish it.
    */

    uint8_t tile_under = get_bkg_tile_xy(tile_x, tile_y + 1);
    if (tile_under != 0 && ball_y_speed > 0) {
        set_bkg_tile_xy(tile_x, tile_y + 1, 0);
        ball_y_speed = -ball_y_speed;
    }

    uint8_t tile_above = get_bkg_tile_xy(tile_x, tile_y - 1);
    if (tile_above != 0 && ball_y_speed >> 8 < 0) {
        set_bkg_tile_xy(tile_x, tile_y - 1, 0);
        ball_y_speed = -ball_y_speed;
    }
    
    uint8_t tile_right = get_bkg_tile_xy(tile_x + 1, tile_y);
    if (tile_right != 0 && ball_x_speed > 0) {
        set_bkg_tile_xy(tile_x + 1, tile_y, 0);
        ball_x_speed = -ball_x_speed;
    }
    
    uint8_t tile_left = get_bkg_tile_xy(tile_x - 1, tile_y);
    if (tile_left != 0 && ball_x_speed < 0) {
        set_bkg_tile_xy(tile_x - 1, tile_y, 0);
        ball_x_speed = -ball_x_speed;
    }
}

void ballDoMovement() {
    ball_y_speed += BALL_SPEED_ACCEL;
    ball_y += ball_y_speed;
    ball_x += ball_x_speed;

    // If out of bounds, reset the ball
    if (ball_x < 8 || ball_x > 160 || ball_y < 4 || (ball_y >> 8) > 144) {
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
    }

    ball_x = cursorPosition;
    ball_y = CURSOR_Y << 8;
}

void ballDoUpdate() {
    if (isReleased) {
        ballDoMovement();
        hide_sprite(POINTER_SPRITE);
    } else {
        ballDoPlayerControls();
        
        move_sprite(POINTER_SPRITE, cursorPosition, 24);
        set_sprite_prop(POINTER_SPRITE, ball_x_speed > 0 ? 0 : S_FLIPX);
    }

    // Consider moving this into a graphics update function if we start separating things out
    move_sprite(BALL_SPRITE, ball_x, (ball_y >> 8));
}

void main() {
    set_bkg_data(0, building_bg_TILE_COUNT, building_bg_tiles);
    set_bkg_tiles(0, 0, 20, 18, building_bg_map);
    SHOW_BKG;

    set_sprite_data(0, ball_TILE_COUNT, ball_tiles);
    set_sprite_tile(BALL_SPRITE, 0);
    move_sprite(BALL_SPRITE, ball_x, ball_y);

    set_sprite_data(1, pointer_TILE_COUNT, pointer_tiles);
    set_sprite_tile(POINTER_SPRITE, 1);
    move_sprite(POINTER_SPRITE, 16, 16);

    SHOW_SPRITES;

    while (1) {
        wait_vbl_done();

        previousInput = input;
        input = joypad();

        ballDoUpdate();
    }
}
