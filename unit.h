#pragma once
#include <SDL2/SDL_image.h>
#include <string>
#include <memory>
#include <vector>
#include "math_funcs.h"
#include "weapon.h"
#include "basic_game_funcs.h"
#include "item.h"
#include "temporary_mod.h"
class GameState;
class GameMap;
class Unit: public std::enable_shared_from_this<Unit>
{
public:
    static constexpr int NUM_UNITS = 100;
protected:
    static int IDcounter; //each unit is assigned a unique ID to help organize things. This starts at 0 and increases every time a unit is created.
    enum class MovementBehavior{none, player, random, homing_simple, homing_erratic, cardinal, bouncy, follow_angle, pure_custom};
    enum class WeaponBehavior{none, player, random, line_to_player, same_angle};
    enum class RotationBehavior{none, player, unidirectional, bidirectional};
    static MovementBehavior BASE_MOVEMENT_BEHAVIOR[NUM_UNITS];
    static WeaponBehavior BASE_WEAPON_BEHAVIOR[NUM_UNITS];
    static RotationBehavior BASE_ROTATION_BEHAVIOR[NUM_UNITS];
    /*the reason ROTATION_FUNC returns a double but MOVEMENT_FUNC doesn't is because MOVEMENT_FUNC just modifies moveX and moveY
    (usually moveX and moveY have already been computed), while ROTATION_FUNC computes the angle
    */
    static double (*BASE_ROTATION_FUNC[NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit);
    static void (*BASE_MOVEMENT_FUNC[NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit);
    static void (*BASE_ON_DEATH_FUNC[NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit);
    static void (*BASE_TAKE_DAMAGE_FUNC[NUM_UNITS])(Unit *a, Unit *b, double damage);
    static SDL_Texture* (*GEN_CUSTOM_TEXTURE[NUM_UNITS])();
    static double BASE_MOVEMENT_SPEED[NUM_UNITS], BASE_HP[NUM_UNITS];
    static double BASE_SIZE1[NUM_UNITS], BASE_VISION_DISTANCE[NUM_UNITS];
    static double BASE_MONEY_REWARD[NUM_UNITS];
    static double BASE_HP_REGEN[NUM_UNITS];
    static bool BASE_IS_SPECTRAL[NUM_UNITS];
    static std::string BASE_NAME[NUM_UNITS];
    static int BASE_WEAPON[NUM_UNITS];
    static void (*BASE_OTHER_BEHAVIOR[NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle);
    bool collidesWithOtherUnit(GameState &game_state); //also modifies the unit if they collide
    bool showHPbar;
    double visionDistance, movementSpeed, omnivamp, rand_var;
    double cdMult;
    double time_since_stuck; //to get out of MovementBehavior::follow_angle traps
    double time_since_took_damage;
    void handleCollision(Unit &other); //the other unit applies its collision effects to this unit
    void moveRandomly(const GameState &game_state, double k);
    void moveToNewPosition(GameState &game_state, double dX, double dY);
    void tryMovingToPosition(GameState &game_state, double dX, double dY);
    void rotateToNewAngle(GameState &game_state, double dAngle);
    bool tryToMoveToNewPositionAndAngle(GameState &game_state, double dX, double dY, double dAngle);
public:
    SDL_Texture *sprite;
    static SDL_Texture *sprites[NUM_UNITS];
    static double BASE_PROJECTILE_OFFSET[NUM_UNITS];
    static double BASE_DAMAGE_ON_COLLISION[NUM_UNITS];
    std::unique_ptr<GeometricObject> shape;
    std::shared_ptr<Unit> owner;
    int belongs_to_room;
    bool is_active;
    static GeometricObject::Type BASE_SHAPE[NUM_UNITS];
    static static_array<Point> BASE_VERTICES[NUM_UNITS];
    std::vector<Weapon> weapons;
    std::vector<Item> items, unshownItems;
    std::vector<TemporaryMod> tempMods;
    Affiliation affiliation, origAffiliation;
    int curWeapon;
    int type, ID, money;
    bool isSpectral;
    double misc_var[4];
    double mana, maxMana, manaRegen, HP, maxHP, HPregen, HPregenOnKill, damageOnCollision, age;
    double spellSecTillCastable, spellSecCooldown;
    double damageReceivedMult, damageDealtMult;
    double moveX, moveY, moveAngle; //the most recent dX, dY, dAngle
    static void init();
    static bool compare_y_with_unit(const std::shared_ptr<Unit> &a, const std::shared_ptr<Unit> &b);
    static bool compare_y_with_double(const std::shared_ptr<Unit> &a, const double &b);
    Unit() = default;
    Unit(int type, Affiliation affiliation, double x, double y);
    bool isPlayer() const;
    void resetToBaseStats();
    double __attribute__((always_inline)) getX() const {return shape->getX();}
    double __attribute__((always_inline)) getY()const {return shape->getY();}
    void __attribute__((always_inline)) setX(double x) {shape->setX(x);}
    void __attribute__((always_inline)) setY(double y) {shape->setY(y);}
    double __attribute__((always_inline)) getAngle() const {return shape->getAngle();}
    void __attribute__((always_inline)) setAngle(double angle) {shape->setAngle(angle);}
    double getMovementSpeed(const GameState &game_state) const;
    double getVisionDistance(const GameState &game_state) const;
    bool isActive(const GameState &game_state) const;
    double dist(const Unit *other) const;
    void operate(GameState &game_state);
    void setInitialPosition(double x, double y);
    void draw(const GameState &game_state) const;
    void drawHPbar(const GameState &game_state) const;
    static void dealDamage(Unit *a, Unit *b, double damage); //a deals damage to b
    bool __attribute__((always_inline)) isDead() const {return HP<=0;}
    void die(GameState &game_state);
    void applyMods(const std::vector<Item> &items);
    void updateTemporaryMods();
    void addTemporaryMod(TemporaryMod::Name name, TemporaryMod::Mod mod, TemporaryMod::Type type, double duration, double var1);
    double getCenterDistanceFromClosestEnemy(const GameMap &game_Map) const;
    bool hasItem(Item::Name name) const;
    bool collidesWithOtherUnit_checkToValidateMap(std::vector<std::shared_ptr<Unit> > units) const;
    bool collidesWithTerrain(const GenericMapRoom &room) const;
    void addMoney(double amount);
    bool isLegalPosition(const GenericMapRoom &room) const;
    bool isDrawable(const GameState &game_state) const;
};
