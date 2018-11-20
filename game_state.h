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
    int prelevelMenuPos;
    double hudSizeFraction;
    int pixelsPerTile;
    std::shared_ptr<Player> player;
    std::vector<DisappearingObject> disapObjs;
    std::vector<Item> shopItems;
    std::vector<Weapon> shopWeapons;
    void renderHUD();
    void initPrelevelMenu();
    void initNewFloor();
    void initBoss();
    void partitionUnits();
public:
    static constexpr SDL_Color hudColor{200, 200, 200, 255};
    static constexpr SDL_Color hudBoundaryColor{50, 50, 50, 255};
    static constexpr double hudBoundaryFraction = 0.01;
    int currentFloor;
    enum class GameStateState{inGame, preLevelMenu, boss};
    std::stack<GameStateState> game_state_state;
    std::vector<SDL_Event> inputEvents;
    std::vector<std::vector<std::shared_ptr<Unit> > > unitPartition;
    GameMap game_map;
    GameState();
    int getFloorDisplayW() const;
    int getFloorDisplayH() const;
    double getHudSizeFraction() const;
    void setHudSizeFraction(double f);
    int getCurrentFloor() const;
    int getHudStartX() const;
    int getHudW() const;
    int getHudH() const;
    int getHudBoundarySize() const;
    int getPixelsPerTile() const;
    void setPixelsPerTile(int p);
    void initNewGame();
    void operateGameInGame();
    void renderInGame();
    void operatePreLevelMenu();
    void renderPreLevelMenu();
    std::shared_ptr<Player> getPlayer();
};
extern GameState currentGame;
