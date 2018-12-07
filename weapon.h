#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <SDL2/SDL_mixer.h>
#include "math_funcs.h"
#include "basic_game_funcs.h"
#include "item.h"
#include "temporary_mod.h"
class GameState;
class Unit;
class Projectile;
class Weapon
{
public:
    static constexpr int NUM_WEAPONS = 2000;
    enum class Name{PoofyPistol = 0, BromineBow = 1, CadmiumCrossbow = 2, DeviceWH722 = 3, UnbrandedSniperRifle = 4,
    PaintballGun = 5,
    DartGun = 200, SmolStick = 201, AWBP = 202, EnglishTextbook = 203,
    GenericShotgun = 400, HuntingRifle = 401, Flamethrower = 402, DeluxePingPongMachine = 403,
    MagnetShooter = 600, Flashlight = 601,
    BigStick = 800, WaterGun = 801, ShotgunTM = 802, P89 = 803, GammaRayGun = 804,
    BiggerStick = 1000, SawedOnShotgun = 1001, P0 = 1002, NihilistsGun = 1003, SprinklerHead = 1004,
    G3SR20 = 1005,
    BiggestStick = 1200, Pi = 1201, SSS = 1202, Railgun = 1203, Blowgun = 1204,
    Laser = 1205, QuasarGun = 1206, BrokenLaser = 1207};
private:
    static audio_chunk FIRE_AUDIO[NUM_WEAPONS];
    static int BASE_AMMO[NUM_WEAPONS];
    static bool BASE_IS_SPECTRAL[NUM_WEAPONS];
    static double BASE_FIRE_RATE[NUM_WEAPONS], BASE_PROJ_LIFESPAN[NUM_WEAPONS], BASE_DAMAGE[NUM_WEAPONS], BASE_SHOT_SPEED[NUM_WEAPONS];
    static double BASE_SPREAD[NUM_WEAPONS];
    static int BASE_PROJ_PER_SHOT[NUM_WEAPONS];
    static SDL_Texture *wield_sprites[NUM_WEAPONS];
    static int BASE_PROJ_TYPE[NUM_WEAPONS];
    static int BASE_PIERCE_COUNT[NUM_WEAPONS];
    static void (*BASE_PROJ_MOVEMENT_FUNC[NUM_WEAPONS])(GameState&, const std::shared_ptr<Unit>&, Projectile&);
    static void (*BASE_FIRE_FUNC[NUM_WEAPONS])(GameState&, const std::shared_ptr<Unit>&, const Weapon&);
    static bool HAS_SPRITE[NUM_WEAPONS];
    double damage, spread, projOffset, shotSpeed, projLifespan, fireRate;
    std::vector<TemporaryMod> tempMods;
    void resetToBaseStats();
    void addTemporaryMod(TemporaryMod::Name name, TemporaryMod::Mod mod, TemporaryMod::Type type, double duration, double var1);
    void fireWeapon(GameState &game_state, std::shared_ptr<Unit> unit);
    bool isLaser() const;
    bool isWaterUser() const;
    bool isSniper() const;
    bool isBow() const;
public:
    static std::string BASE_NAME[NUM_WEAPONS], BASE_DESC[NUM_WEAPONS];
    static SDL_Texture *sprites[NUM_WEAPONS];
    static double BASE_SPELL_COOLDOWN[NUM_WEAPONS], BASE_SPELL_COST[NUM_WEAPONS], BASE_FIRERATE_IRREGULARITY[NUM_WEAPONS];
    static int BASE_AMMO_PER_PURCHASE[NUM_WEAPONS], BASE_AMMO_COST[NUM_WEAPONS];
    static double BASE_MAXHP_PCT_DAMAGE[Weapon::NUM_WEAPONS];
    static int BASE_COST[NUM_WEAPONS];
    int type;
    int ammo;
    int pierceCount;
    double fireCounter;
    static void init();
    Weapon();
    Weapon(int type);
    Weapon(Name name);
    void draw(const GameState &game_state, int x, int y, double angle) const;
    void draw_wield(const GameState &game_state, int x, int y, double angle) const;
    void update(GameState &game_state, std::shared_ptr<Unit> unit, bool fire);
    void applyMods(const std::vector<Item> &items);
    bool cast_spell(GameState &game_state, std::shared_ptr<Unit> unit);
    void resetFireCounter(const Unit *unit);
    double getFireRate(const Unit *unit) const;
    double getProjLifespan(const Unit *unit) const;
    double getProjSpread(const Unit *unit) const;
    double getProjSpeed(const Unit *unit) const;
    double getProjDamage(const Unit *unit) const;
    double getProjPierceCount(const Unit *unit) const;
    double getProjMaxHpPctDamage(const Unit *unit) const;
    void updateTemporaryMods();
};
class Projectile
{
    static constexpr int NUM_PROJECTILES = 17;
    static SDL_Texture *sprites[NUM_PROJECTILES];
    static std::string BASE_SHAPE[NUM_PROJECTILES];
    static double BASE_SIZE1[NUM_PROJECTILES];
    static static_array<Point> BASE_VERTICES[NUM_PROJECTILES];
    static SDL_Texture* (*GEN_CUSTOM_TEXTURE[NUM_PROJECTILES])();
    double timeTillExpiration;
    fast_set<int> alreadyCollidedWith; //contains IDs of units it already hits. Only useful for piercing projectiles.
public:
    void (*MOVEMENT_FUNC)(GameState&, const std::shared_ptr<Unit>&, Projectile&);
    SDL_Texture *sprite;
    int type, collisionsLeft;
    bool is_spectral;
    double dX, dY, damage, maxHPPctdamage;
    Affiliation affiliation;
    std::unique_ptr<GeometricObject> shape;
    std::shared_ptr<Unit> owner;
    double age, rand_var, misc_var[4];
    Projectile(Affiliation affiliation, int type, const std::shared_ptr<Unit> &owner, double damage,
               double x, double y, double dX, double dY, double timeTillExpiration, double angle,
               void (*moveFunc)(GameState&, const std::shared_ptr<Unit>&, Projectile&) = nullptr);
    static void create(GameState &game_state, const std::shared_ptr<Unit> &unit, int type, double dmg,
                    double shotSpeed, double projAngle, double lifespan, double offset = NOT_SET,
                    void (*moveFunc)(GameState&, const std::shared_ptr<Unit>&, Projectile&) = nullptr);
    static void create_no_relative_velocity(GameState &game_state, const std::shared_ptr<Unit> &unit, int type, double dmg,
                    double shotSpeed, double projAngle, double lifespan, double offset = NOT_SET,
                    void (*moveFunc)(GameState&, const std::shared_ptr<Unit>&, Projectile&) = nullptr);
    static void create_normal(GameState &game_state, const std::shared_ptr<Unit> &unit, int type, double dmg, double shotSpeed,
                        double projAngle, double lifespan, double origAngle, bool relVelocity = true); //better start location for the player
    static void init();
    double getX() const;
    double getY() const;
    double getAngle() const;
    void setX(double);
    void setY(double);
    void setAngle(double);
    bool collide(GameState &game_state);
    void move(GameState &game_state);
    void draw(const GameState &game_state) const;
    bool isSpectral();
};
