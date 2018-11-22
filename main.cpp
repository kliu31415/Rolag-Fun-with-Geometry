#include <iostream>
#include <cstdlib>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <queue>
#include "sdl_base.h"
#include "in_game.h"
#include "game_map.h"
#include "game_state.h"
#include "basic_game_funcs.h"
enum class global_game_state{main_menu, in_game} current_global_game_state = global_game_state::in_game;
int main()
{
    //the order of some inits matters
    sdl_settings::load_config();
    atexit(SDL_Quit);
    atexit(sdl_settings::output_config);
    initSDL("Rogue Like Arena Game!");
    Unit::init();
    Weapon::init();
    Projectile::init();
    Item::init();
    GameMap::init();
    init_new_game(currentGame);
    SDL_SetWindowIcon(getWindow(), IMG_Load("data/icon.png"));
    CPULoadCalculator cpuLoadCalculator(1e9);
    while(true)
    {
        cpuLoadCalculator.begin_counting();
        switch(current_global_game_state)
        {
        case global_game_state::main_menu:
            break;
        case global_game_state::in_game:
            handle_global_game_state_in_game(currentGame);
            break;
        }
        cpuLoadCalculator.end_counting();
        int fsz = getFontSize(0);
        drawText("CPU Core Load: " + format_to_places(cpuLoadCalculator.get_load(), 2), 0, getWindowH() - fsz, fsz, 100, 100, 100, 150);
        updateScreen();
    }
    return 0;
}
