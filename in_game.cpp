#include "game_state.h"
#include "sdl_base.h"
#include "game_map.h"
#include "weapon.h"
#include "menu.h"
#include <cstdlib>
#include <algorithm>
GameState currentGame;
void init_new_game(GameState &game_state)
{
    calculateGameVars();
    menuStack = std::stack<std::shared_ptr<Menu> >();
    current_global_game_state = global_game_state::in_game;
    game_state.setHudSizeFraction(0.17);
    game_state.initNewGame();
}
void handle_global_game_state_in_game(GameState &game_state)
{
    BASE_FPS = std::max(30, getFPS()); //don't let giant lagg spikes screw things up too much
    if(game_state.getPlayer()->age < 1) //it's the start of a floor, don't let the floor loading screw things up
        BASE_FPS = std::max(BASE_FPS, getDisplayHertz());
    TICK_SPEED = 1.0 / (BASE_FPS * ITER_PER_FRAME);
    game_state.inputEvents.clear();
    SDL_Event input;
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_WINDOWEVENT:
            if(input.window.event==SDL_WINDOWEVENT_RESIZED || input.window.event==SDL_WINDOWEVENT_SIZE_CHANGED)
                game_state.game_map.generateMinimapTexture(game_state);
            break;
        case SDL_KEYDOWN:
            switch(input.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                //only open the pause menu if we aren't already paused
                /*if(game_state.game_state_state.top() != GameState::GameStateState::pause)*/
                if(game_state.game_state_state.top() == GameState::GameStateState::inGame)
                {
                    game_state.game_state_state.push(GameState::GameStateState::pause);
                    addToMenuStack(pause_menu);
                }
                break;
            }
            break;
        }
        game_state.inputEvents.push_back(input);
    }
    switch(game_state.game_state_state.top())
    {
    case GameState::GameStateState::inGame:
        game_state.operateGameInGame();
        game_state.drawInGame();
        break;
    case GameState::GameStateState::preLevelMenu:
        game_state.operatePreLevelMenu();
        game_state.drawPreLevelMenu();
        break;
    case GameState::GameStateState::deathScreen:
        game_state.operateDeathScreen();
        game_state.drawDeathScreen();
        break;
    case GameState::GameStateState::transition:
        game_state.operateTransitionScreen();
        game_state.drawTransitionScreen();
        break;
    case GameState::GameStateState::pause:
        game_state.drawPauseMenu(); //draw first because operate might pop the stack
        game_state.operatePauseMenu();
        break;
    case GameState::GameStateState::console:
        game_state.drawConsole(); //draw first
        game_state.operateConsole();
        break;
    }
}
