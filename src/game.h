#ifndef _GAME_H
#define _GAME_H

#include <gb/gb.h>

#define BALL_SPRITE         0
#define POINTER_SPRITE      1
#define SMOKE_SPRITE_START  2
#define SMOKE_SPRITE_RANGE  4
#define CURSOR_Y            20
#define BALL_SPEED_INITIAL  25
#define BALL_SPEED_ACCEL    10

typedef struct SmokeEffect {
    uint8_t x, y;
    uint8_t isDisplayed;
    uint8_t timer;
} SmokeEffect;

void stateInitGame();
void stateUpdateGame();

#endif