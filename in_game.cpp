#include "game_state.h"
#include "sdl_base.h"
#include "game_map.h"
#include "weapon.h"
#include <cstdlib>
#include <algorithm>
GameState currentGame;
void init_new_game(GameState &game_state)
{
    game_state.setHudSizeFraction(0.15);
    game_state.setPixelsPerTile(100);
    game_state.initNewGame();
}
void handle_global_game_state_in_game(GameState &game_state)
{
    game_state.inputEvents.clear();
    SDL_Event input;
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
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
    }
}
