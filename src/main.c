#include <gb/gb.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include "gen/building-bg.h"
#include "gen/ball.h"

#define BALL_SPRITE 0
#define CURSOR_Y    16
// #define BALL_SPEED_INITIAL 300
// #define BALL_SPEED_ACCEL    75
#define BALL_SPEED_INITIAL 50
#define BALL_SPEED_ACCEL    25

void main(void) {
    set_bkg_data(0, building_bg_TILE_COUNT, building_bg_tiles);
    set_bkg_tiles(0, 0, 20, 18, building_bg_map);
    SHOW_BKG;

    uint8_t cursorPosition = 16;
    uint8_t ball_x = cursorPosition;
    uint16_t ball_y = CURSOR_Y;
    int16_t ball_y_speed = BALL_SPEED_INITIAL;
    int8_t ball_x_speed = 1;
    set_sprite_data(0, ball_TILE_COUNT, ball_tiles);
    set_sprite_tile(BALL_SPRITE, 0);
    move_sprite(BALL_SPRITE, ball_x, ball_y);
    SHOW_SPRITES;

    uint8_t isReleased = FALSE;
    uint8_t input, previousInput;
    while(1) {
        wait_vbl_done();

        previousInput = input;
        input = joypad();

        if (input & J_RIGHT && !(previousInput & J_RIGHT)) {
            cursorPosition += 8;
        } else if (input & J_LEFT && !(previousInput & J_LEFT)) {
            cursorPosition -= 8;
        }

        if (input & J_A && !(previousInput & J_A)) {
            isReleased = TRUE;
        }

        if (isReleased) {
            ball_y_speed += BALL_SPEED_ACCEL;
            ball_y += ball_y_speed;
            ball_x += ball_x_speed;

            if (ball_x < 0 || ball_x > 160 || ball_y < 4 || (ball_y >> 8) > 144) {
                isReleased = FALSE;
                ball_y_speed = BALL_SPEED_INITIAL;
            }

            uint8_t tile_x = (ball_x - 8) / 8;
            uint8_t tile_y = ((ball_y >> 8) - 16) / 8;
            uint8_t tile_under = get_bkg_tile_xy(tile_x, tile_y+1);
            uint8_t tile_above = get_bkg_tile_xy(tile_x, tile_y-1);
            uint8_t tile_left = get_bkg_tile_xy(tile_x-1, tile_y);
            uint8_t tile_right = get_bkg_tile_xy(tile_x+1, tile_y);
            if (tile_under != 0 && ball_y_speed > 0) {
                set_bkg_tile_xy(tile_x, tile_y+1, 0);
                ball_y_speed = -ball_y_speed;
            } else if (tile_above != 0 && ball_y_speed >> 8 < 0) {
                set_bkg_tile_xy(tile_x, tile_y-1, 0);
                ball_y_speed = -ball_y_speed;
            }
            if (tile_right != 0 && ball_x_speed > 0) {
                set_bkg_tile_xy(tile_x+1, tile_y, 0);
                ball_x_speed = -ball_x_speed;
            } else if (tile_left != 0 && ball_x_speed < 0) {
                set_bkg_tile_xy(tile_x-1, tile_y, 0);
                ball_x_speed = -ball_x_speed;
            }
        } else {
            ball_x = cursorPosition;
            ball_y = CURSOR_Y << 8;
        }
        move_sprite(BALL_SPRITE, ball_x, (ball_y >> 8));
    }
}
