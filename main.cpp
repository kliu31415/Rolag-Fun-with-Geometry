#include <iostream>
#include <cstdlib>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <algorithm>
#include <queue>
#include "sdl_base.h"
#include "in_game.h"
#include "game_map.h"
#include "game_state.h"
#include "basic_game_funcs.h"
#include "menu.h"
int main()
{
    sdl_settings::load_config();
    atexit(SDL_Quit);
    atexit(TTF_Quit);
    atexit(IMG_Quit);
    atexit(Mix_Quit);
    atexit([]() -> void{Mix_HaltMusic();}); //mpg123 was crashing on exit before, and this seems to stop the crashes
    atexit(sdl_settings::output_config);
    atexit([]() -> void {println("exit function called");});
    initSDL("FUWIGE");
    initMusic();
    //the order of some inits matters
    Unit::init();
    Weapon::init();
    Projectile::init();
    Item::init();
    GameMap::init();
    Room::init();
    StandardMenu::init();
    ConsoleWindow::init();
    Menu::game_state = &currentGame;
    SDL_SetWindowIcon(getWindow(), IMG_Load("data/icon.png"));
    CPULoadCalculator cpuLoadCalculator(1e9);
    current_global_game_state = global_game_state::menu;
    calculateGameVars();
    BASE_FPS = getDisplayHertz();
    addToMenuStack(main_menu);
    init_console_screen("welcome"); //pushes the welcome message onto menuStack
    while(true)
    {
        cpuLoadCalculator.begin_counting();
        switch(current_global_game_state)
        {
        case global_game_state::menu:
            if(menuStack.empty()) //this occurred once as a bug. Not sure how to reproduce. Maybe using return instead of break in the operate console menu code fixes it?
                print_error("menuStack is empty");
            if(menuStack.top() == main_menu)
                switch_to_music("main menu");
            menuStack.top()->draw(); //call draw before operate because operate() might pop the menu stack
            menuStack.top()->operate();
            break;
        case global_game_state::in_game:
            handle_global_game_state_in_game(currentGame);
            break;
        }
        cpuLoadCalculator.end_counting();
        int fsz = getFontSize(0);
        if(SHOW_EXTRA_INFO)
        {
            VerticalTextDrawer text(0, getWindowH() - 4*fsz, fsz, getWindowW());
            text.draw("FPS: " + to_str(getFPS()), VerticalTextDrawer::Justify::left, 100, 100, 150, 150);
            text.draw("Load: " + format_to_places(cpuLoadCalculator.get_load(), 2), VerticalTextDrawer::Justify::left, 100, 100, 150, 150);
            text.draw("Projectile Count: " + to_str(currentGame.game_map.projectiles.size()), VerticalTextDrawer::Justify::left, 100, 100, 150, 150);
            text.draw("Unit Count: " + to_str(currentGame.game_map.units.size()), VerticalTextDrawer::Justify::left, 100, 100, 150, 150);
        }
        updateScreen();
    }
    return 0;
}
