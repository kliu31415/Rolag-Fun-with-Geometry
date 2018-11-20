#pragma once
#include <SDL2/SDL_image.h>
#include <string>
#include <memory>
#include <vector>
#include "math_funcs.h"
#include "weapon.h"
#include "basic_game_funcs.h"
#include "item.h"
class GameState;
class GameMap;
class TemporaryMod
{
public:
    int type;
    std::vector<double> vars;
    double timeLeft, maxTimeLeft;
    TemporaryMod(int type, double timeLeft, double var1);
    TemporaryMod(int type, double timeLeft, double var1, double var2);
};
class Unit: public std::enable_shared_from_this<Unit>
{
public:
    static constexpr int NUM_UNITS = 6;
protected:
    static int IDcounter; //each unit is assigned a unique ID to help organize things. This starts at 0 and increases every time a unit is created.
    enum class MovementBehavior{player, stationary, random, homing_simple, homing_erratic, cardinal};
    enum class FireBehavior{none, player, random, line_to_player};
    static MovementBehavior BASE_MOVEMENT_BEHAVIOR[NUM_UNITS];
    static FireBehavior BASE_FIRE_BEHAVIOR[NUM_UNITS];
    static double BASE_MOVEMENT_SPEED[NUM_UNITS], BASE_HP[NUM_UNITS];
    static double BASE_SIZE1[NUM_UNITS], BASE_VISION_DISTANCE[NUM_UNITS];
    static bool BASE_IS_SPECTRAL[NUM_UNITS];
    bool isSpectral;
    bool collidesWithTerrain(GameMap &game_map) const;
    bool collidesWithOtherUnit(GameState &game_state);
    bool showHPbar;
    double visionDistance;
    void resetToBaseStats();
    void handleCollision(Unit &other); //the other unit applies its collision effects to this unit
    void moveRandomly(double k);
    bool isPlayer() const;
    double movementSpeed;
    double omnivamp;
public:
    static SDL_Texture *sprites[NUM_UNITS];
    static double BASE_DAMAGE_ON_COLLISION[NUM_UNITS];
    std::unique_ptr<GeometricObject> shape;
    static GeometricObject::Type BASE_SHAPE[NUM_UNITS];
    static static_array<Point> BASE_VERTICES[NUM_UNITS];
    std::vector<Weapon> weapons;
    std::vector<Item> items;
    std::vector<TemporaryMod> tempMods;
    Affiliation affiliation, origAffiliation;
    int curWeapon;
    int type, ID;
    double HP, maxHP, angle, damageOnCollision;
    double moveX, moveY; //the most recent dX and dY
    static void init();
    static bool compare_y_with_unit(const std::shared_ptr<Unit> &a, const std::shared_ptr<Unit> &b);
    static bool compare_y_with_double(const std::shared_ptr<Unit> &a, const double &b);
    Unit() = default;
    Unit(int type, Affiliation affiliation, double x, double y);
    inline double getX() const {return shape->getX();}
    inline double getY() const {return shape->getY();}
    inline void setX(double x) {shape->setX(x);}
    inline void setY(double y) {shape->setY(y);}
    double getMovementSpeed() const;
    double dist(const Unit *other) const;
    void operate(GameState &game_state);
    void setInitialPosition(double x, double y);
    void moveToNewPosition(GameState &game_state, double dX, double dY);
    virtual void render(const GameState &game_state) const;
    static void dealDamage(Unit *a, Unit *b, double damage); //a deals damage to b
    bool isDead() const;
    void applyMod(const Item &item);
    void applyMods(const std::vector<Item> &items);
    void updateTemporaryMods();
    void addTemporaryMod(int type);
    bool hasItem(int type) const;
};

/**
Unit type list:
0 = Player
1 =
*/
