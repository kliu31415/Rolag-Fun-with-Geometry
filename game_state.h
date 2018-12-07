#pragma once
#include <vector>
#include <memory>
#include <vector>
#include <stack>
#include "player.h"
#include "game_map.h"
#include "basic_game_funcs.h"
#include "item.h"
#include "weapon.h"
class GameState
{
    static constexpr SDL_Color hudColor{200, 200, 200, 255};
    static constexpr SDL_Color hudBoundaryColor{50, 50, 50, 255};
    static constexpr double hudBoundaryFraction = 0.01;
    int prelevelMenuPos;
    double hudSizeFraction;
    std::shared_ptr<Player> player;
    std::vector<Item> shopItems;
    std::vector<Weapon> shopWeapons;
    void drawHUD();
    void initPrelevelMenu(void (GameState::*draw_f1)());
    void initDeathScreen();
    void initNewFloor();
    void initBoss();
    void partitionUnits();
    void drawMainMenu(); //dummy function. don't actually use for anything besides transitions.
    void completeInitingNewFloor();
    void completeInitingPrelevelMenu();
public:
    double item_cost_mult, weapon_cost_mult, ammo_cost_mult;
    int currentFloor;
    enum class GameStateState{inGame, preLevelMenu, deathScreen, transition, pause, console};
    std::stack<GameStateState> game_state_state;
    std::vector<SDL_Event> inputEvents;
    std::vector<std::vector<std::shared_ptr<Unit> > > unitPartition;
    std::vector<std::vector<double> > unitPartitionY;
    GameMap game_map;
    GameState();
    void updateExtraAesthetics();
    void clearExtraAesthetics();
    void drawExtraAesthetics();
    int getFloorDisplayW() const;
    int getFloorDisplayH() const;
    double getHudSizeFraction() const;
    void setHudSizeFraction(double f);
    int getCurrentFloor() const;
    int getHudStartX() const;
    int getHudW() const;
    int getHudH() const;
    int getHudBoundarySize() const;
    void initNewGame();
    void operateGameInGame(bool isPlayerAlreadyDead = false);
    void drawInGame();
    void operatePreLevelMenu();
    void drawPreLevelMenu();
    void operateDeathScreen();
    void drawDeathScreen();
    void initTransitionScreen(void (GameState::*drawScreen1)(), void (GameState::*drawScreen2)(), void (GameState::*init)() = nullptr);
    void operateTransitionScreen();
    void drawTransitionScreen();
    void operatePauseMenu();
    void drawPauseMenu();
    void initConsole();
    void operateConsole();
    void drawConsole();
    void draw_main_menu(); //dummy
    void draw_console_cur_floor(); //dummy function because we must pass a GameState member to initTransitionScreen
    void draw_console_prev_floor(); //dummy function because we must pass a GameState member to initTransitionScreen
    std::shared_ptr<Player> __attribute__((always_inline)) getPlayer() const {return player;}
};
extern GameState currentGame;
