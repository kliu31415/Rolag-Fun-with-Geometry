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
    static constexpr int NUM_UNITS = 7;
protected:
    static int IDcounter; //each unit is assigned a unique ID to help organize things. This starts at 0 and increases every time a unit is created.
    enum class MovementBehavior{none, player, random, homing_simple, homing_erratic, cardinal, bouncy};
    enum class WeaponBehavior{none, player, random, line_to_player, same_angle};
    enum class RotationBehavior{none, player, constant, constant_bidirectional};
    static MovementBehavior BASE_MOVEMENT_BEHAVIOR[NUM_UNITS];
    static WeaponBehavior BASE_WEAPON_BEHAVIOR[NUM_UNITS];
    static RotationBehavior BASE_ROTATION_BEHAVIOR[NUM_UNITS];
    static double BASE_MOVEMENT_SPEED[NUM_UNITS], BASE_HP[NUM_UNITS], BASE_ROTATION_VAR1[NUM_UNITS];
    static double BASE_SIZE1[NUM_UNITS], BASE_VISION_DISTANCE[NUM_UNITS];
    static bool BASE_IS_SPECTRAL[NUM_UNITS];
    static int BASE_WEAPON[NUM_UNITS];
    bool isSpectral;
    bool collidesWithOtherUnit(GameState &game_state); //also modifies the unit if they collide
    bool showHPbar;
    double visionDistance;
    double movementSpeed;
    double omnivamp;
    void resetToBaseStats();
    void handleCollision(Unit &other); //the other unit applies its collision effects to this unit
    void moveRandomly(const GameMap &game_map, double k);
    void moveToNewPosition(GameState &game_state, double dX, double dY);
    void tryMovingToPosition(GameState &game_state, double dX, double dY);
    void rotateToNewAngle(GameState &game_state, double dAngle);
    bool tryToMoveToNewPositionAndAngle(GameState &game_state, double dX, double dY, double dAngle);
public:
    static SDL_Texture *sprites[NUM_UNITS];
    static double BASE_PROJECTILE_OFFSET[NUM_UNITS];
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
    double HP, maxHP, damageOnCollision;
    double moveX, moveY, moveAngle; //the most recent dX, dY, dAngle
    static void init();
    static bool compare_y_with_unit(const std::shared_ptr<Unit> &a, const std::shared_ptr<Unit> &b);
    static bool compare_y_with_double(const std::shared_ptr<Unit> &a, const double &b);
    Unit() = default;
    Unit(int type, Affiliation affiliation, double x, double y);
    bool isPlayer() const;
    inline double getX() const {return shape->getX();}
    inline double getY() const {return shape->getY();}
    inline void setX(double x) {shape->setX(x);}
    inline void setY(double y) {shape->setY(y);}
    inline double getAngle() const {return shape->getAngle();}
    inline void setAngle(double angle) {shape->setAngle(angle);}
    double getMovementSpeed(const GameMap &game_map) const;
    double dist(const Unit *other) const;
    void operate(GameState &game_state);
    void setInitialPosition(double x, double y);
    virtual void draw(const GameState &game_state) const;
    static void dealDamage(Unit *a, Unit *b, double damage); //a deals damage to b
    bool isDead() const;
    void applyMod(const Item &item);
    void applyMods(const std::vector<Item> &items);
    void updateTemporaryMods();
    void addTemporaryMod(int type);
    double getCenterDistanceFromClosestEnemy(const GameMap &game_Map) const;
    bool hasItem(Item::Name name) const;
    bool collidesWithOtherUnit_checkToValidateMap(std::vector<std::shared_ptr<Unit> > units) const;
    bool collidesWithTerrain(GameMap &game_map) const;
};
