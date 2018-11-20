#include <iostream>
#include <cstdlib>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include "sdl_base.h"
#include "in_game.h"
#include "game_map.h"
#include "game_state.h"
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
    while(true)
    {
        switch(current_global_game_state)
        {
        case global_game_state::main_menu:
            break;
        case global_game_state::in_game:
            handle_global_game_state_in_game(currentGame);
            break;
        }
    }
    return 0;
}
