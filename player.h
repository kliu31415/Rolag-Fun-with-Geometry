#pragma once
#include <SDL2/SDL_image.h>
#include <vector>
#include "unit.h"
class GameState;
class Player: public Unit
{
    SDL_Scancode moveUp, moveDown, moveLeft, moveRight, cycleWeapon;
    std::vector<int> pressTimer;
    void updateKeyPressTimer(const uint8_t *keystate, int &timer, SDL_Scancode key);
    void updateMinimap(GameMap &game_map) = delete; //updating the minimap is expensive and it's not really
public:
    Player();
    Player(double startX, double startY);
    static void init();
    void moveToNewPosition(GameMap &game_map, double dX, double dY);
    void handleInput(GameState &game_state);
    void handleCollisionsWithHurtTiles(GameState &game_state); //and collapsing tiles too
    bool isOnPortal(GameState &game_state) const;
    void applyFirstTimeItemEffects(GameState &game_state, Item::Name name);
};
