#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "math_funcs.h"
#include "basic_game_funcs.h"
#include "item.h"
class GameState;
class Unit;
class Weapon
{
public:
    static constexpr int NUM_WEAPONS = 6;
private:
    static int BASE_AMMO[NUM_WEAPONS];
    static double BASE_FIRE_RATE[NUM_WEAPONS], BASE_PROJ_LIFESPAN[NUM_WEAPONS], BASE_DAMAGE[NUM_WEAPONS], BASE_SHOT_SPEED[NUM_WEAPONS];
    static double BASE_SPREAD[NUM_WEAPONS], BASE_FIRERATE_IRREGULARITY[NUM_WEAPONS];
    static int BASE_PROJ_PER_SHOT[NUM_WEAPONS];
    static SDL_Texture *wield_sprites[NUM_WEAPONS];
    static int BASE_PROJ_TYPE[NUM_WEAPONS];
    double damage, spread;
    void resetToBaseStats();
public:
    static std::string BASE_NAME[NUM_WEAPONS], BASE_DESC[NUM_WEAPONS];
    static SDL_Texture *sprites[NUM_WEAPONS];
    static int BASE_AMMO_PER_PURCHASE[NUM_WEAPONS], BASE_AMMO_COST[NUM_WEAPONS];
    static int BASE_COST[NUM_WEAPONS];
    int type;
    int ammo;
    double fireCounter, fireRate;
    static void init();
    Weapon(int type);
    void render(const GameState &game_state, int x, int y, double angle) const;
    void render_wield(const GameState &game_state, int x, int y, double angle) const;
    void update(GameState &game_state, std::shared_ptr<Unit> unit, bool fire);
    void resetFireCounter();
    void applyMod(const Item &item);
    void applyMods(const std::vector<Item> &items);
};
class Projectile
{
    static constexpr int NUM_PROJECTILES = 3;
    static SDL_Texture *sprites[NUM_PROJECTILES];
    static std::string BASE_SHAPE[NUM_PROJECTILES];
    static double BASE_SIZE1[NUM_PROJECTILES];
    double dX, dY, damage, angle;
    int type;
    double timeTillExpiration;
public:
    Affiliation affiliation;
    std::unique_ptr<GeometricObject> shape;
    std::shared_ptr<Unit> owner;
    Projectile(Affiliation affiliation, int type, std::shared_ptr<Unit> owner, double damage, double x, double y, double dX, double dY, double timeTillExpiration, double angle);
    static void init();
    double getX() const;
    double getY() const;
    void setX(double);
    void setY(double);
    bool collide(GameState &game_state);
    void move(GameState &game_state);
    void render(const GameState &game_state) const;
};
