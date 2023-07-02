#include <gb/gb.h>
#include "global.h"
// Generated files
#include "gen/title-screen.h"

void stateInitTitle() {
    set_bkg_data(0, title_screen_TILE_COUNT, title_screen_tiles);
    set_bkg_tiles(0, 0, 20, 18, title_screen_map);
    SHOW_BKG;
}

void stateUpdateTitle() {
    if (input & J_START && !(previousInput & J_START)) {
        stateChangeToLevelSelect();
    }
}
