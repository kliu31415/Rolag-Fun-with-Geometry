#include "game_map.h"
#include "unit.h"
#include "weapon.h"
#include "item.h"
#include "math_funcs.h"
#include "game_state.h"
void GameMap::init()
{
    blank_sprite = loadTexture("data/tile/blank.png", 255, 255, 255);
    for(int i=0; i<NUM_DIRT_SPRITES; i++)
        dirt_sprites[i] = loadTexture(("data/tile/dirt_" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(int i=0; i<NUM_WALL_SPRITES; i++)
        wall_sprites[i] = loadTexture(("data/tile/wall_" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(int i=0; i<NUM_HURT_SPRITES; i++)
        hurt_sprites[i] = loadTexture(("data/tile/hurt_" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(int i=0; i<NUM_COLLAPSE_SPRITES; i++)
        collapse_sprites[i] = loadTexture(("data/tile/collapse_" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(int i=0; i<NUM_SLIP_SPRITES; i++)
        slip_sprites[i] = loadTexture(("data/tile/slip_" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(int i=0; i<NUM_PORTAL_SPRITES; i++)
    {
        portal_closed_sprites[i] = loadTexture(("data/tile/portal_closed_" + to_str(i) + ".png").c_str(), 255, 255, 255);
        portal_open_sprites[i] = loadTexture(("data/tile/portal_open_" + to_str(i) + ".png").c_str(), 255, 255, 255);
    }
    //floors
    int cur = 1;
    BASE_FLOOR_TIME[cur] = 160;
    BASE_FLOOR_NAME[cur] = "Red Room";
    //
    cur = 2;
    BASE_FLOOR_TIME[cur] = 80;
    BASE_FLOOR_NAME[cur] = "Red Room";
    //floors
    cur = 3;
    BASE_FLOOR_TIME[cur] = 112;
    BASE_FLOOR_NAME[cur] = "Blue Room";
    //
    cur = 4;
    BASE_FLOOR_TIME[cur] = 82;
    BASE_FLOOR_NAME[cur] = "Blue Room";
    //
    cur = 5;
    BASE_FLOOR_TIME[cur] = 115;
    BASE_FLOOR_NAME[cur] = "Yellow Room";
    //
    cur = 6;
    BASE_FLOOR_TIME[cur] = 85;
    BASE_FLOOR_NAME[cur] = "ellow Room";
    //
    cur = 7;
    BASE_FLOOR_TIME[cur] = 117;
    BASE_FLOOR_NAME[cur] = "Purpl Ro";
    //
    cur = 8;
    BASE_FLOOR_TIME[cur] = 87;
    BASE_FLOOR_NAME[cur] = "Purple";
    //
    cur = 9;
    BASE_FLOOR_TIME[cur] = 120;
    BASE_FLOOR_NAME[cur] = "een R"; //Green Room
    //
    cur = 10;
    BASE_FLOOR_TIME[cur] = 90;
    BASE_FLOOR_NAME[cur] = "random_hex"; //Green Room
    //
    cur = 11;
    BASE_FLOOR_TIME[cur] = 125;
    BASE_FLOOR_NAME[cur] = "random_hex"; //Orange Room
    //
    cur = 12;
    BASE_FLOOR_TIME[cur] = 95;
    BASE_FLOOR_NAME[cur] = "random_hex"; //Orange Room
    //
    cur = 13;
    BASE_FLOOR_TIME[cur] = 125;
    BASE_FLOOR_NAME[cur] = "random_hex"; //BW Room 1
    //
    cur = 14;
    BASE_FLOOR_TIME[cur] = 100;
    BASE_FLOOR_NAME[cur] = "random_hex";
    //
    cur = 15;
    BASE_FLOOR_TIME[cur] = 140;
    BASE_FLOOR_NAME[cur] = "random_hex"; //BW Room 2
    //
    cur = 16;
    BASE_FLOOR_TIME[cur] = 110;
    BASE_FLOOR_NAME[cur] = "random_hex";
    //kworker
    cur = 17;
    BASE_FLOOR_TIME[cur] = 300;
    BASE_FLOOR_NAME[cur] = "/usr/sbin";
    //US Army
    cur = 18;
    BASE_FLOOR_TIME[cur] = 150;
    BASE_FLOOR_NAME[cur] = "/dev/input";
    //US Marines
    cur = 19;
    BASE_FLOOR_TIME[cur] = 360;
    BASE_FLOOR_NAME[cur] = "/dev/input";
    //units
    for(auto &i: UNIT_SPAWN_WEIGHT)
        for(auto &j: i)
            j = 0;
    //
    UNIT_SPAWN_WEIGHT[1][1] = 1;
    UNIT_SPAWN_WEIGHT[1][2] = 0.3;
    UNIT_SPAWN_WEIGHT[1][3] = 0.2;
    UNIT_SPAWN_WEIGHT[1][4] = 0.5;
    UNIT_SPAWN_WEIGHT[1][5] = 0.8;
    UNIT_SPAWN_WEIGHT[1][6] = 0.4;
    //
    UNIT_SPAWN_WEIGHT[3][37] = 1;
    UNIT_SPAWN_WEIGHT[3][38] = 1;
    UNIT_SPAWN_WEIGHT[3][48] = 1;
    //
    UNIT_SPAWN_WEIGHT[5][10] = 1;
    UNIT_SPAWN_WEIGHT[5][11] = 1;
    UNIT_SPAWN_WEIGHT[5][12] = 0.5;
    UNIT_SPAWN_WEIGHT[5][13] = 0.4;
    UNIT_SPAWN_WEIGHT[5][14] = 0.4;
    //
    UNIT_SPAWN_WEIGHT[7][39] = 1;
    UNIT_SPAWN_WEIGHT[7][49] = 1;
    //
    UNIT_SPAWN_WEIGHT[9][20] = 1;
    UNIT_SPAWN_WEIGHT[9][21] = 1;
    UNIT_SPAWN_WEIGHT[9][22] = 1;
    UNIT_SPAWN_WEIGHT[9][23] = 1;
    UNIT_SPAWN_WEIGHT[9][26] = 1;
    UNIT_SPAWN_WEIGHT[9][40] = 0.4;
    //
    UNIT_SPAWN_WEIGHT[11][27] = 2;
    UNIT_SPAWN_WEIGHT[11][28] = 2;
    //
    UNIT_SPAWN_WEIGHT[13][33] = 1;
    UNIT_SPAWN_WEIGHT[13][34] = 1;
    //
    UNIT_SPAWN_WEIGHT[15][33] = 1;
    UNIT_SPAWN_WEIGHT[15][34] = 1;
    UNIT_SPAWN_WEIGHT[15][42] = 1;
    UNIT_SPAWN_WEIGHT[15][50] = 1;
    UNIT_SPAWN_WEIGHT[15][52] = 1;
    //
    for(int f=7 ; f>0; f-=2) //weaker units can spawn on later floors too
        for(int g=f-2; g>0; g-=2)
            for(int i=0; i<Unit::NUM_UNITS; i++)
                UNIT_SPAWN_WEIGHT[f][i] += UNIT_SPAWN_WEIGHT[g][i] * square(g / (double)f);
    for(int i=0; i<NUM_FLOORS; i++)
    {
        TOTAL_UNIT_SPAWN_WEIGHT[i] = 0;
        for(int j=0; j<Unit::NUM_UNITS; j++)
            TOTAL_UNIT_SPAWN_WEIGHT[i] += UNIT_SPAWN_WEIGHT[i][j];
    }
    //items
    for(auto &i: ITEM_SHOP_SPAWN_WEIGHT)
        for(auto &j: i)
            j = 0;
    for(int f=1; f<=14; f+=2)
        for(int i=0; i<200; i++)
            if(Item::BASE_NAME[(f-1)*100 + i] != "")
                ITEM_SHOP_SPAWN_WEIGHT[f][(f-1)*100 + i] = 1;
    for(int i=NUM_FLOORS-1; i>0; i--) //items can also spawn on later floors, not just the one where they're initially available
        for(int j=1; j<i; j++)
            for(int k=0; k<Item::NUM_ITEMS; k++)
                ITEM_SHOP_SPAWN_WEIGHT[i][k] += ITEM_SHOP_SPAWN_WEIGHT[j][k] * (j/(int)i);
    for(int i=0; i<NUM_FLOORS; i++)
    {
        TOTAL_ITEM_SHOP_SPAWN_WEIGHT[i] = 0;
        for(int j=0; j<Item::NUM_ITEMS; j++)
            TOTAL_ITEM_SHOP_SPAWN_WEIGHT[i] += ITEM_SHOP_SPAWN_WEIGHT[i][j];
    }
    //weapons
    for(auto &i: WEAPON_SHOP_SPAWN_WEIGHT)
        for(auto &j: i)
            j = 0;
    for(int f=1; f<=14; f+=2)
        for(int i=0; i<200; i++)
            if(Weapon::BASE_NAME[(f-1)*100 + i] != "")
                WEAPON_SHOP_SPAWN_WEIGHT[f][(f-1)*100 + i] = 1;
    for(int i=NUM_FLOORS-1; i>0; i--) //weapons can also spawn on later floors, not just the one where they're initially available
        for(int j=1; j<i; j++)
            for(int k=0; k<Weapon::NUM_WEAPONS; k++)
                WEAPON_SHOP_SPAWN_WEIGHT[i][k] += WEAPON_SHOP_SPAWN_WEIGHT[j][k] * (j/(int)i);
    for(int i=0; i<NUM_FLOORS; i++)
    {
        TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[i] = 0;
        for(int j=0; j<Weapon::NUM_WEAPONS; j++)
            TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[i] += WEAPON_SHOP_SPAWN_WEIGHT[i][j];
    }
}

void Unit::init()
{
    for(auto &i: BASE_WEAPON_BEHAVIOR) //units don't fire by default
        i = WeaponBehavior::none;
    for(auto &i: BASE_ROTATION_BEHAVIOR) //units don't rotate by default
        i = RotationBehavior::none;
    for(auto &i: BASE_WEAPON) //units don't have weapons by default
        i = NOT_SET;
    for(auto &i: BASE_IS_SPECTRAL) //units aren't spectral by default
        i = false;
    for(auto &i: BASE_OTHER_BEHAVIOR) //units don't have other behavior by default
        i = nullptr;
    for(auto &i: BASE_MOVEMENT_FUNC) //units don't have custom movement modifiers by default
        i = nullptr;
    for(auto &i: BASE_ON_DEATH_FUNC) //units don't do anything special on death by default
        i = nullptr;
    for(auto &i: BASE_TAKE_DAMAGE_FUNC) //units don't do anything special on taking damage by default
        i = nullptr;
    for(auto &i: BASE_HP_REGEN) //units don't regen by default
        i = 0;
    for(auto &i: GEN_CUSTOM_TEXTURE) //no custom texture by default
        i = nullptr;
    //no need to set BASE_ROTATION_FUNC to nullptr because it shouldn't be called if it's nullptr anyways
    //
    int cur = 0;
    BASE_NAME[cur] = "player";
    BASE_WEAPON_BEHAVIOR[cur] = WeaponBehavior::player;
    BASE_HP[cur] = 50;
    BASE_MOVEMENT_SPEED[cur] = 6;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.7;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::player;
    BASE_DAMAGE_ON_COLLISION[cur] = 20;
    BASE_PROJECTILE_OFFSET[cur] = 0.7;
    BASE_VISION_DISTANCE[cur] = 8;
    //
    cur = 1;
    BASE_NAME[cur] = "red square";
    BASE_HP[cur] = 10;
    BASE_MOVEMENT_SPEED[cur] = 3.5;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::random;
    BASE_DAMAGE_ON_COLLISION[cur] = 20;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(1.2 * randf() < TICK_SPEED * DIFFICULTY)
        {
            double ang = randf() * 2*PI;
            Projectile::create(game_state, unit, 3, 5, 5, ang, 2);
        }
    };
    //
    cur = 2;
    BASE_NAME[cur] = "yellow square";
    BASE_HP[cur] = 7;
    BASE_MOVEMENT_SPEED[cur] = 5.5;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_VISION_DISTANCE[cur] = 7;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_simple;
    BASE_DAMAGE_ON_COLLISION[cur] = 15;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(unit->dist(game_state.getPlayer().get()) < unit->getVisionDistance(game_state) && unit->misc_var[0] + 1 < unit->age)
        {
            unit->misc_var[0] = unit->age;
            double ang = std::atan2(game_state.getPlayer()->getY() - unit->getY(), game_state.getPlayer()->getX() - unit->getX());
            for(int i=-1; i<=1; i++)
                Projectile::create(game_state, unit, 5, 5, 5, ang + i * PI/6, 2);
        }
    };
    //
    cur = 3;
    BASE_NAME[cur] = "green square";
    BASE_HP[cur] = 12;
    BASE_MOVEMENT_SPEED[cur] = 4.9;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_VISION_DISTANCE[cur] = 7;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_DAMAGE_ON_COLLISION[cur] = 25;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(1.5 * randf() < TICK_SPEED * DIFFICULTY)
        {
            double ang = std::atan2(game_state.getPlayer()->getY() - unit->getY(), game_state.getPlayer()->getX() - unit->getX());
            Projectile::create(game_state, unit, 4, 5, 5, ang, 2);
        }
    };
    //
    cur = 4;
    BASE_NAME[cur] = "orange square";
    BASE_HP[cur] = 5;
    BASE_MOVEMENT_SPEED[cur] = 5.8;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::cardinal;
    BASE_DAMAGE_ON_COLLISION[cur] = 14;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(2 * randf() < TICK_SPEED * DIFFICULTY)
        {
            for(int i=0; i<4; i++)
                Projectile::create(game_state, unit, 8, 5, 5.5, unit->getAngle() + i * PI/2, 2);
        }
    };
    //
    cur = 5;
    BASE_NAME[cur] = "red T";
    BASE_HP[cur] = 5;
    BASE_MOVEMENT_SPEED[cur] = 4.5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(8);
    BASE_VERTICES[cur][0] = Point(0.01, -0.14);
    BASE_VERTICES[cur][1] = Point(0.01, -0.38);
    BASE_VERTICES[cur][2] = Point(-0.28, -0.38);
    BASE_VERTICES[cur][3] = Point(-0.28, 0.38);
    BASE_VERTICES[cur][4] = Point(0.01, 0.38);
    BASE_VERTICES[cur][5] = Point(0.01, 0.13);
    BASE_VERTICES[cur][6] = Point(0.32, 0.13);
    BASE_VERTICES[cur][7] = Point(0.32, -0.14);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 13;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::unidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 3.5;}; // radians/s ccw rotation
    BASE_PROJECTILE_OFFSET[cur] = 0.33;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.8 * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 3, 5, 4.9, unit->getAngle(), 2);
        }
    };
    //
    cur = 6;
    BASE_NAME[cur] = "red 4-star";
    BASE_HP[cur] = 55;
    BASE_MOVEMENT_SPEED[cur] = 3.2;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(8);
    BASE_VERTICES[cur][0] = Point(0.01, -0.93);
    BASE_VERTICES[cur][1] = Point(0.24, -0.24);
    BASE_VERTICES[cur][2] = Point(0.93, 0.01);
    BASE_VERTICES[cur][3] = Point(0.24, 0.24);
    BASE_VERTICES[cur][4] = Point(0.01, 0.93);
    BASE_VERTICES[cur][5] = Point(-0.24, 0.24);
    BASE_VERTICES[cur][6] = Point(-0.93, 0.01);
    BASE_VERTICES[cur][7] = Point(-0.23, -0.23);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 23;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 3.1;}; // radians/s ccw rotation
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.6 * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 3, 5, 5, randf() * 2 * PI, 2);
        }
    };
    //
    cur = 7;
    BASE_NAME[cur] = "red 5-star";
    BASE_HP[cur] = 240;
    BASE_MOVEMENT_SPEED[cur] = 2.8;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(10);
    BASE_VERTICES[cur][0] = Point(0.01, -1.69);
    BASE_VERTICES[cur][1] = Point(0.42, -0.40);
    BASE_VERTICES[cur][2] = Point(1.71, -0.37);
    BASE_VERTICES[cur][3] = Point(0.70, 0.41);
    BASE_VERTICES[cur][4] = Point(1.06, 1.71);
    BASE_VERTICES[cur][5] = Point(0.01, 0.97);
    BASE_VERTICES[cur][6] = Point(-1.05, 1.71);
    BASE_VERTICES[cur][7] = Point(-0.68, 0.41);
    BASE_VERTICES[cur][8] = Point(-1.7, -0.37);
    BASE_VERTICES[cur][9] = Point(-0.4, -0.4);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 45;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::unidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 2.77;};
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.07 * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 3, 5, 5.57, randf() * 2 * PI, INF);
        }
    };
    //
    cur = 8;
    BASE_NAME[cur] = "green 4-star";
    BASE_HP[cur] = 220;
    BASE_MOVEMENT_SPEED[cur] = 3.3;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[6];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 38;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
     {return 16.5 * std::pow(std::sin(unit->age * 5 / (2*PI)), 3);};
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.0045 * (square(16.5) - square(dAngle)) * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 4, 5, 5.87, unit->getAngle() + PI/2 * randz(0, 3), INF);
        }
    };
    //
    cur = 9;
    BASE_NAME[cur] = "fat yellow arrow";
    BASE_HP[cur] = 220;
    BASE_MOVEMENT_SPEED[cur] = 3.3;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(7);
    BASE_VISION_DISTANCE[cur] = INF;
    BASE_VERTICES[cur][0] = Point(0.00, -0.76);
    BASE_VERTICES[cur][1] = Point(0.92, 0.01);
    BASE_VERTICES[cur][2] = Point(0.00, 0.78);
    BASE_VERTICES[cur][3] = Point(0.00, 0.39);
    BASE_VERTICES[cur][4] = Point(-0.91, 0.39);
    BASE_VERTICES[cur][5] = Point(-0.91, -0.38);
    BASE_VERTICES[cur][6] = Point(0.00, -0.38);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::follow_angle;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_DAMAGE_ON_COLLISION[cur] = 40;
    BASE_PROJECTILE_OFFSET[cur] = 0.92;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double HPpercent = unit->HP / unit->maxHP;
        if(fmod(unit->age, 3 + 2 * unit->HP / unit->maxHP) < 1)
        {
            unit->moveX = 0;
            unit->moveY = 0;
            return;
        }
        unit->moveX *= 2 - HPpercent;
        unit->moveY *= 2 - HPpercent;
    };
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double HPpercent = unit->HP / unit->maxHP;
        if(fmod(unit->age, 3 + 2 * HPpercent) < 1)
        {
            return 20 - 10 * HPpercent;
        }
        double angle = unit->getAngle();
        auto p = game_state.getPlayer();
        double target = std::atan2(p->getY() - unit->getY(), p->getX() - unit->getX());
        double diff = std::fmod(target - angle + 100*PI, 2*PI); //100*PI just ensures the first argument is positive
        if(diff > PI)
            return (4 - 2 * HPpercent) * (diff - 2*PI);
        return (4 - 2 * HPpercent) * (diff);
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpercent = unit->HP / unit->maxHP;
        if(0.02 * (1 + HPpercent) * randf() < TICK_SPEED * DIFFICULTY &&
           fmod(unit->age, 3 + 2 * HPpercent) < 1)
        {
            Projectile::create(game_state, unit, 5, 5, 4.21, unit->getAngle(), INF);
        }
    };
    //
    cur = 10;
    BASE_NAME[cur] = "yellow T";
    BASE_HP[cur] = 10;
    BASE_MOVEMENT_SPEED[cur] = 4.8;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[5];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 19;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::unidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 3.7;};
    BASE_PROJECTILE_OFFSET[cur] = 0.33;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.5 * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 5, 5.5, 6, unit->getAngle(), 2);
        }
    };
    //
    cur = 11;
    BASE_NAME[cur] = "red triangle";
    BASE_HP[cur] = 12;
    BASE_MOVEMENT_SPEED[cur] = 4.5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(3);
    BASE_VERTICES[cur][0] = Point(0, -0.44);
    BASE_VERTICES[cur][1] = Point(0.37, 0.23);
    BASE_VERTICES[cur][2] = Point(-0.37, 0.23);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_DAMAGE_ON_COLLISION[cur] = 22;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::unidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 5 * square(std::sin((unit->rand_var + 2) * getTicksS()));};
    BASE_VISION_DISTANCE[cur] = 8;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double HPpercent = unit->HP / unit->maxHP;
        unit->moveX *= 1.5 - HPpercent/2;
        unit->moveY *= 1.5 - HPpercent/2;
    };
    //
    cur = 12;
    BASE_NAME[cur] = "red pentagon";
    BASE_HP[cur] = 25;
    BASE_MOVEMENT_SPEED[cur] = 4.5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(5);
    BASE_VERTICES[cur][0] = Point(0, -0.46);
    BASE_VERTICES[cur][1] = Point(0.40, -0.15);
    BASE_VERTICES[cur][2] = Point(0.25, 0.35);
    BASE_VERTICES[cur][3] = Point(-0.25, 0.35);
    BASE_VERTICES[cur][4] = Point(-0.41, -0.15);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::random;
    BASE_DAMAGE_ON_COLLISION[cur] = 25;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double dA = 11 * square(std::sin((unit->rand_var + 5) * getTicksS()));
        double dA2 = std::max(0.0, 5 - unit->time_since_took_damage);
        return dA + dA2;
    };
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mult = std::max(1.0, 1.5 -  0.5 * unit->time_since_took_damage);
        unit->moveX *= mult;
        unit->moveY *= mult;
    };
    //
    cur = 13;
    BASE_NAME[cur] = "yellow pentagon";
    BASE_HP[cur] = 36;
    BASE_MOVEMENT_SPEED[cur] = 3.5;;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[12];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::random;
    BASE_DAMAGE_ON_COLLISION[cur] = 35;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double dA = std::max(2.1, 20 * (0.8 - unit->time_since_took_damage));
        if(unit->rand_var < 0.5)
            return -dA;
        return dA;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double prob = 1 / std::max(1e-9, 30 * (0.8 - unit->time_since_took_damage));
        if(2 * prob * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 5, 2.7, 5.5, -PI/10 + unit->getAngle() + randz(0, 4) * 2*PI / 5, 2);
        }
    };
    //
    cur = 14;
    BASE_NAME[cur] = "yellow 4-star";
    BASE_HP[cur] = 100;
    BASE_MOVEMENT_SPEED[cur] = 4.1;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[6];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 30;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 5;};
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.2 * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 5, 5.5, 3.87, unit->getAngle() + PI/2 * randz(0, 3), 2);
        }
    };
    //
    cur = 15;
    BASE_NAME[cur] = "red 6-star";
    BASE_HP[cur] = 350;
    BASE_MOVEMENT_SPEED[cur] = 3.9;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(12);
    BASE_VERTICES[cur][0] = Point(0, -1.71);
    BASE_VERTICES[cur][1] = Point(0.61, -0.86);
    BASE_VERTICES[cur][2] = Point(1.72, -0.85);
    BASE_VERTICES[cur][3] = Point(1.18, 0.01);
    BASE_VERTICES[cur][4] = Point(1.71, 0.86);
    BASE_VERTICES[cur][5] = Point(0.61, 0.87);
    BASE_VERTICES[cur][6] = Point(0, 1.72);
    BASE_VERTICES[cur][7] = Point(-.62, 0.88);
    BASE_VERTICES[cur][8] = Point(-1.73, 0.87);
    BASE_VERTICES[cur][9] = Point(-1.17, 0.01);
    BASE_VERTICES[cur][10] = Point(-1.72, -0.86);
    BASE_VERTICES[cur][11] = Point(-0.62, -0.86);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 44;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double HPpct = unit->HP / unit->maxHP;
        return (14 - 5 * HPpct) * std::pow(std::fabs(std::sin((10 - 7 * HPpct) * getTicksS())), 2.3);
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.2 * randf() < TICK_SPEED * DIFFICULTY)
        {
            game_state.game_map.projectiles.emplace_back(Affiliation::generic_enemy, 3, unit, 5.5, unit->getX(), unit->getY(), 0, 0, INF, 0,
                [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
            {
                if(proj.age > 1)
                {
                    proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                    proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                }
            });
            Projectile &p = game_state.game_map.projectiles.back();
            p.dX = game_state.getPlayer()->getX() - unit->getX();
            p.dY = game_state.getPlayer()->getY() - unit->getY();
            normToHypot(p.dX, p.dY, 7.5);
        }
        double HPpct = unit->HP / unit->maxHP;
        if((0.4 + HPpct) * 0.11 * randf() < TICK_SPEED * DIFFICULTY && unit->moveAngle > 5)
        {
            double theta = randf() * 2 * PI;
            double r = 5 + randf() * 4 - 4 * HPpct;
            game_state.game_map.projectiles.emplace_back(Affiliation::generic_enemy, 3, unit, 4.32, unit->getX(), unit->getY(),
                                                         r * std::cos(theta), r * std::sin(theta), INF, 0);
        }
    };
    //
    cur = 16;
    BASE_NAME[cur] = "yellow 5-star";
    BASE_HP[cur] = 330;
    BASE_MOVEMENT_SPEED[cur] = 2.7;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[7];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 42;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        return 4.2;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.5 * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 5, 5.5, NOT_SET, unit->getAngle(), INF, NOT_SET,
                [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
            {
                double dX = game_state.getPlayer()->getX() - unit->getX();
                double dY = game_state.getPlayer()->getY() - unit->getY();
                normToHypot(dX, dY, 4);
                proj.setX(proj.getX() + dX * TICK_SPEED);
                proj.setY(proj.getY() + dY * TICK_SPEED);
            });
        }
        double HPpct = unit->HP / unit->maxHP;
        if((2 + 2 * HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            int numProj = 30 - 15*HPpct;
            for(int i=0; i<numProj; i++)
                Projectile::create(game_state, unit, 5, 4.3, 5, i * 2*PI / numProj, INF);
        }
    };
    //
    cur = 17;
    BASE_NAME[cur] = "fat green arrow";
    BASE_HP[cur] = 340;
    BASE_MOVEMENT_SPEED[cur] = 3.7;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[9];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::follow_angle;
    BASE_DAMAGE_ON_COLLISION[cur] = 35;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double HPpercent = unit->HP / unit->maxHP;
        if(fmod(unit->age, 2 + 2 * HPpercent) < 0.5)
        {
            return 30 - 15 * HPpercent;
        }
        double angle = unit->getAngle();
        auto p = game_state.getPlayer();
        double target = std::atan2(p->getY() - unit->getY(), p->getX() - unit->getX());
        double diff = std::fmod(target - angle + 100*PI, 2*PI); //100*PI just ensures the first argument is positive
        if(diff > PI)
            return (5 - 3 * HPpercent) * (diff - 2*PI);
        return (5 - 3 * HPpercent) * (diff);
    };
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpercent = unit->HP / unit->maxHP;
        if(0.08*randf() < TICK_SPEED * DIFFICULTY && fmod(unit->age, 2 + 2 * HPpercent) < 0.5)
        {
            Projectile::create(game_state, unit, 4, 5.5, 5, 0, INF, NOT_SET,
                [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
            {
                double HPpercent = unit->HP / unit->maxHP;
                if(unit->isDead() || fmod(unit->age, 2 + 2 * HPpercent) >= 0.5)
                {
                    proj.dX = std::pow(proj.age, 1.8) * std::cos(unit->getAngle());
                    proj.dY = std::pow(proj.age, 1.8) * std::sin(unit->getAngle());
                }
                proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                proj.setY(proj.getY() + proj.dY * TICK_SPEED);
            });
        }
    };
    //
    cur = 18;
    BASE_NAME[cur] = "yellow 6-star";
    BASE_HP[cur] = 440;
    BASE_MOVEMENT_SPEED[cur] = 2.5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[15];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 45;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        return 5;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.25 * randf() < TICK_SPEED * DIFFICULTY)
        {
            game_state.game_map.projectiles.emplace_back(Affiliation::generic_enemy, 5, unit, 2, unit->getX(), unit->getY(), 0, 0, INF, 0,
                [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
            {
                double HPpct = unit->HP / unit->maxHP;
                if(proj.age > HPpct)
                {
                    proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                    proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                }
            });
            Projectile &p = game_state.game_map.projectiles.back();
            p.dX = game_state.getPlayer()->getX() - unit->getX();
            p.dY = game_state.getPlayer()->getY() - unit->getY();
            normToHypot(p.dX, p.dY, 7.5);
        }
        double HPpct = unit->HP / unit->maxHP;
        if((1.7 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            int numProj = 75 - 10*HPpct;
            double START = std::atan2(game_state.getPlayer()->getY() - unit->getY(), game_state.getPlayer()->getX() - unit->getX()) + 0.55;
            for(int i=0; i<numProj; i++)
                Projectile::create(game_state, unit, 5, 5.5, 4, START + i * 5.2 / numProj, 2);
        }
    };
    //
    cur = 19;
    BASE_NAME[cur] = "purple 4-star";
    BASE_HP[cur] = 380;
    BASE_MOVEMENT_SPEED[cur] = 3.2;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[6];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 35;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {
        return 12 * square(std::sin(4 * getTicksS()));};
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(unit->moveAngle > 11.8 && 0.0018 * randf() < TICK_SPEED * DIFFICULTY)
        {
            double HPpct = unit->HP / unit->maxHP;
            Projectile::create(game_state, unit, 7, 5.5, 5 + 2*(1-HPpct)*randf(), unit->getAngle() + randz(0, 3) * PI/2, INF, NOT_SET);
        }
    };
    //
    cur = 20;
    BASE_NAME[cur] = "yellow triangle";
    BASE_HP[cur] = 18;
    BASE_MOVEMENT_SPEED[cur] = 4.5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[11];
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_DAMAGE_ON_COLLISION[cur] = 27;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 5.5 * square(std::sin((unit->rand_var + 2) * getTicksS()));};
    BASE_VISION_DISTANCE[cur] = 8;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double HPpercent = unit->HP / unit->maxHP;
        unit->moveX *= 1.7 - 0.7*HPpercent;
        unit->moveY *= 1.7 - 0.7*HPpercent;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if((1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 5, 5.5, 7.3, unit->getAngle() + PI/6 + randz(0, 2) * 2*PI/3, 2, NOT_SET);
        }
    };
    //
    cur = 21;
    BASE_NAME[cur] = "green triangle";
    BASE_HP[cur] = 26;
    BASE_MOVEMENT_SPEED[cur] = 4.4;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[11];
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_DAMAGE_ON_COLLISION[cur] = 34;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 6 * square(std::sin((unit->rand_var + 2) * getTicksS()));};
    BASE_VISION_DISTANCE[cur] = 8;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double HPpercent = unit->HP / unit->maxHP;
        unit->moveX *= 2 - HPpercent;
        unit->moveY *= 2 - HPpercent;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if((1.5 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            for(int i=0; i<3; i++)
                Projectile::create(game_state, unit, 4, 5.5, 6.9, unit->getAngle() + PI/6 + i * 2*PI/3, 2, NOT_SET);
        }
    };
    //
    cur = 22;
    BASE_NAME[cur] = "green pentagon";
    BASE_HP[cur] = 50;
    BASE_MOVEMENT_SPEED[cur] = 3.4;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[13];
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 30;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double dA = std::max(2.3 + unit->rand_var, 50 * (0.5 - unit->time_since_took_damage));
        if(std::fmod(unit->rand_var, 0.01) < 0.005)
            return -dA;
        return dA;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(unit->time_since_took_damage < 0.2 && 0.1 * randf() < TICK_SPEED * DIFFICULTY)
        {
            for(int i=0; i<5; i++)
            {
                Projectile::create(game_state, unit, 4, 5.5, 5.15, -PI/10 + unit->getAngle() + i * 2*PI / 5, 2, NOT_SET,
                                   [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                    {
                        if(std::fmod(proj.age, 0.2*proj.rand_var + 0.2) < 0.2)
                        {
                            proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                        }
                    });
            }
        }
    };
    //
    cur = 23;
    BASE_NAME[cur] = "red heart";
    BASE_HP[cur] = 18;
    BASE_HP_REGEN[cur] = 1;
    BASE_MOVEMENT_SPEED[cur] = 4.5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(10);
    BASE_VERTICES[cur][0] = Point(0, -0.22);
    BASE_VERTICES[cur][1] = Point(0.15, -0.39);
    BASE_VERTICES[cur][2] = Point(0.31, -0.39);
    BASE_VERTICES[cur][3] = Point(0.44, -0.18);
    BASE_VERTICES[cur][4] = Point(0.36, 0.11);
    BASE_VERTICES[cur][5] = Point(0, 0.42);
    BASE_VERTICES[cur][6] = Point(-0.36, 0.11);
    BASE_VERTICES[cur][7] = Point(-0.44, -0.18);
    BASE_VERTICES[cur][8] = Point(-0.31, -0.39);
    BASE_VERTICES[cur][9] = Point(-0.15, -0.39);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_DAMAGE_ON_COLLISION[cur] = 40;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double dA = 2 * unit->rand_var + 2;
        if(std::fmod(unit->rand_var, 0.01) < 0.005)
            dA = -dA;
        return dA;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double mult = std::max(1.0, 2 -  4 * unit->time_since_took_damage);
        unit->moveX *= mult;
        unit->moveY *= mult;
    };
    //
    cur = 24;
    BASE_NAME[cur] = "yellow 5-star";
    BASE_HP[cur] = 450;
    BASE_MOVEMENT_SPEED[cur] = 4.5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[7];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_DAMAGE_ON_COLLISION[cur] = 45;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double dA = 2 * unit->rand_var + 4;
        if(std::fmod(unit->rand_var, 0.01) < 0.005)
            dA = -dA;
        return dA;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(0.7 * (1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            int numProj = 20 - 8*HPpct;
            for(int i=0; i<numProj; i++)
                Projectile::create(game_state, unit, 5, 5.5, 7 - 2*HPpct + randf(), randf() * 2*PI, INF, NOT_SET,
                    [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                {
                    if(std::fmod(proj.age, 0.5*proj.rand_var + 0.5) < 0.5)
                    {
                        proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                        proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                    }
                });
        }
    };
    //
    cur = 25;
    BASE_NAME[cur] = "fat orange arrow";
    BASE_HP[cur] = 200;
    BASE_MOVEMENT_SPEED[cur] = 3.5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[9];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::follow_angle;
    BASE_DAMAGE_ON_COLLISION[cur] = 45;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        double HPpercent = unit->HP / unit->maxHP;
        double angle = unit->getAngle();
        auto p = game_state.getPlayer();
        double target = std::atan2(p->getY() - unit->getY(), p->getX() - unit->getX());
        double diff = std::fmod(target - angle + 100*PI, 2*PI); //100*PI just ensures the first argument is positive
        if(diff > PI)
            return (13 - 6 * HPpercent) * (diff - 2*PI);
        return (13 - 6 * HPpercent) * (diff);
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(0.08 * (1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 5, 5.5, 9 - 3*HPpct + 2*randf(), unit->getAngle(), INF, NOT_SET,
                [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
            {
                if(std::fmod(proj.age, 0.2*proj.rand_var + 0.4) > 0.3)
                {
                    proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                    proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                }
            });
        }
    };
    //
    cur = 26;
    BASE_NAME[cur] = "green T";
    BASE_HP[cur] = 20;
    BASE_MOVEMENT_SPEED[cur] = 5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[5];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 25;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::unidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 3.7;};
    BASE_PROJECTILE_OFFSET[cur] = 0.33;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(1.1 * randf() < TICK_SPEED * DIFFICULTY)
        {
            for(int i=-1; i<=1; i++)
                Projectile::create(game_state, unit, 4, 6, 4.15, unit->getAngle() + i*PI/2, 2);
        }
    };
    //
    cur = 27;
    BASE_NAME[cur] = "yellow heart";
    BASE_HP[cur] = 30;
    BASE_HP_REGEN[cur] = 2.5;
    BASE_MOVEMENT_SPEED[cur] = 4.1;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[23];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 45;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::unidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 1.8 + unit->rand_var;};
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(2 * randf() < TICK_SPEED * DIFFICULTY)
        {
            for(int i=0; i<=6; i++)
                Projectile::create(game_state, unit, 5, 5.5, 4.4, unit->getAngle() + i*PI/3, 2);
        }
    };
    //
    cur = 28;
    BASE_NAME[cur] = "orange T";
    BASE_HP[cur] = 40;
    BASE_MOVEMENT_SPEED[cur] = 4.1;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[5];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 40;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 1.8 + unit->rand_var;};
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(std::fmod(unit->age, 1 + 0.5*unit->rand_var) < 0.4 && 0.08 * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 8, 6, 5.5, unit->getAngle(), 2);
        }
    };
    //
    cur = 29;
    BASE_NAME[cur] = "orange heart";
    BASE_HP[cur] = 300;
    BASE_MOVEMENT_SPEED[cur] = 4.1;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[23];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 45;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit)
     -> double {return 3.8 + unit->rand_var + 3 * std::sin(unit->age);};
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(0.08 * (1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            Projectile::create(game_state, unit, 8, 6, 6 - 1.5*HPpct, unit->getAngle(), INF, NOT_SET,
                [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
            {
                double HPpct = unit->HP / unit->maxHP;
                if(unit->time_since_took_damage < 0.4 - 0.3*HPpct) //ok might not work if playing at 20fps xD
                {
                    double ms = std::hypot(proj.dX, proj.dY);
                    proj.dX = game_state.getPlayer()->getX() - proj.getX();
                    proj.dY = game_state.getPlayer()->getY() - proj.getY();
                    normToHypot(proj.dX, proj.dY, ms);
                    proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                    proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                    /*double theta = randf() * 2*PI;
                    proj.dX = ms * std::cos(theta);
                    proj.dY = ms * std::sin(theta);*/
                }
                else
                {
                    proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                    proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                }
            });
        }
    };
    //
    cur = 30;
    BASE_NAME[cur] = "green 5-star";
    BASE_HP[cur] = 600;
    BASE_MOVEMENT_SPEED[cur] = 4.1;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[7];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 45;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit)
     -> double {return 3.1 + unit->rand_var + 2 * std::sin(3 * unit->age);};
    BASE_PROJECTILE_OFFSET[cur] = 0;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(unit->misc_var[1] == 0)
        {
            if(0.035 * (1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
            {
                Projectile::create(game_state, unit, 4, 6, 6 + randf() * (2 - HPpct), randf() * 2*PI, INF, NOT_SET,
                [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                {
                    double dX = 0, dY = 0;
                    if(unit->misc_var[0]==0 || proj.misc_var[0]==0)
                    {
                        dX += proj.dX;
                        dY += proj.dY;
                    }
                    else
                    {
                        double ms = std::max(std::fabs(proj.dX), std::fabs(proj.dY));
                        double theta = std::atan2(proj.getY() - unit->getY(), proj.getX() - unit->getX());
                        dX += -ms * std::sin(theta);
                        dY += ms * std::cos(theta);
                    }
                    proj.setX(proj.getX() + dX * TICK_SPEED);
                    proj.setY(proj.getY() + dY * TICK_SPEED);
                    if(2 * randf() < TICK_SPEED * DIFFICULTY)
                        proj.misc_var[0] = randz(0, 2);
                });
            }
        }
        else
        {
            if(0.035 * (1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
            {
                auto p = game_state.getPlayer();
                double ang = std::atan2(p->getY() - unit->getY(), p->getX() - unit->getX());
                Projectile::create(game_state, unit, 4, 5, 6 + randf() * (3 - HPpct), ang, INF, NOT_SET,
                [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                {
                    double dX = 0, dY = 0;
                    if(unit->misc_var[0]==0)
                    {
                        dX += proj.dX;
                        dY += proj.dY;
                    }
                    else
                    {
                        double ms = std::max(std::fabs(proj.dX), std::fabs(proj.dY));
                        double theta = std::atan2(proj.getY() - unit->getY(), proj.getX() - unit->getX());
                        dX += -ms * std::sin(theta);
                        dY += ms * std::cos(theta);
                    }
                    proj.setX(proj.getX() + dX * TICK_SPEED);
                    proj.setY(proj.getY() + dY * TICK_SPEED);
                });
            }
        }
        for(int i=0; i<2; i++)
            if(2 * randf() < TICK_SPEED * DIFFICULTY)
                flip(unit->misc_var[i]);
    };
    //
    cur = 31;
    BASE_NAME[cur] = "kworker";
    BASE_HP[cur] = 400;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::pure_custom;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(4);
    BASE_VERTICES[cur][0] = Point(-0.4, -0.4);
    BASE_VERTICES[cur][1] = Point(0.4, -0.4);
    BASE_VERTICES[cur][2] = Point(0.4, 0.4);
    BASE_VERTICES[cur][3] = Point(-0.4, 0.4);
    BASE_DAMAGE_ON_COLLISION[cur] = 100;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 0;};
    BASE_PROJECTILE_OFFSET[cur] = 0;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        static double rest_time = 1, time_till_finished_moving;
        static bool inited_movement = false;
        double HPpct = unit->HP / unit->maxHP;
        if(rest_time <= 0)
        {
            if(!inited_movement)
            {
                inited_movement = true;
                if(randz(0, 1) == 0)
                {
                    unit->moveX = randf() * (8 - 4 * HPpct) + 4;
                    double v1 = (1 + randf() * (game_state.game_map.getNumColumns() - 2));
                    double v2 = (1 + randf() * (game_state.game_map.getNumColumns() - 2));
                    double destination = (v1 + v2) / 2 - unit->getX();
                    if(sign(unit->moveX) != sign(destination))
                        unit->moveX = -unit->moveX;
                    time_till_finished_moving = destination / unit->moveX;
                }
                else
                {
                    unit->moveY = randf() * (8 - 4 * HPpct) + 4;
                    double v1 = (1 + randf() * (game_state.game_map.getNumRows() - 2));
                    double v2 = (1 + randf() * (game_state.game_map.getNumRows() - 2));
                    double destination = (v1 + v2) / 2 - unit->getY();
                    if(sign(unit->moveY) != sign(destination))
                        unit->moveY = -unit->moveY;
                    time_till_finished_moving = destination / unit->moveY;
                }
            }
            time_till_finished_moving -= TICK_SPEED;
            if((unit->moveX==0 && unit->moveY==0) || time_till_finished_moving < 0)
            {
                unit->moveX = 0;
                unit->moveY = 0;
                inited_movement = false;
                rest_time = randf() * 0.5;
            }
        }
        else rest_time -= TICK_SPEED * (2 - HPpct);
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(randf() > HPpct)
        {
            if((0.23 - 0.04*HPpct) * randf() < TICK_SPEED * DIFFICULTY)
            {
                game_state.game_map.projectiles.emplace_back(Affiliation::generic_enemy, 11, unit, 4, unit->getX(), unit->getY(), 0, 0,
                    INF, randf()*2*PI, [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                {
                    proj.dX = proj.misc_var[0] + proj.misc_var[1] * std::sin(proj.misc_var[1] * proj.age);
                    proj.dY = proj.misc_var[2] + proj.misc_var[3] * std::cos(proj.misc_var[3] * proj.age);
                    proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                    proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                    proj.setAngle(proj.getAngle() + (2 + 8 * proj.rand_var) * TICK_SPEED);
                });
                auto &proj = game_state.game_map.projectiles.back();
                double r = (1.5 - 0.5*HPpct) * (3 + 3*randf());
                double theta = randf() * 2*PI;
                proj.misc_var[0] = r * std::cos(theta);
                proj.misc_var[2] = r * std::sin(theta);
                proj.misc_var[1] = (1.5 - 0.5*HPpct) * (1 + 5 * randf()) * randsign();
                proj.misc_var[3] = (1.5 - 0.5*HPpct) * (1 + 5 * randf()) * randsign();
            }
            if(6 * randf() < TICK_SPEED * DIFFICULTY)
            {
                int numProj = 12 + 10 * randf() - 10 * HPpct;
                for(int i=0; i<numProj; i++)
                {
                    Projectile::create_no_relative_velocity(game_state, unit, 10, 3, 4 + randf() * (3 - 3*HPpct), randf()*2*PI, INF, NOT_SET,
                        [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                    {
                        if(unit->misc_var[1] == 1)
                        {
                            proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                        }
                        if(std::fmod(proj.rand_var, 0.01) < 0.005)
                            proj.setAngle(proj.getAngle() + (2 + 8 * proj.rand_var) * TICK_SPEED);
                        else proj.setAngle(proj.getAngle() - (2 + 8 * proj.rand_var) * TICK_SPEED);
                    });
                }
            }
            if(6 * randf() < TICK_SPEED * DIFFICULTY)
            {
                int numProj = 12 + 10 * randf() - 10 * HPpct;
                for(int i=0; i<numProj; i++)
                {
                    Projectile::create_no_relative_velocity(game_state, unit, 10, 3, 4 + randf() * (3 - 3*HPpct), randf()*2*PI, INF, NOT_SET,
                        [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                    {
                        if(unit->misc_var[1] == 1)
                        {
                            proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                        }
                        else
                        {
                            proj.setX(proj.getX() - proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() - proj.dY * TICK_SPEED);
                        }
                        if(std::fmod(proj.rand_var, 0.01) < 0.005)
                            proj.setAngle(proj.getAngle() + (2 + 8 * proj.rand_var) * TICK_SPEED);
                        else proj.setAngle(proj.getAngle() - (2 + 8 * proj.rand_var) * TICK_SPEED);
                    });
                }
            }
            if(6 * randf() < TICK_SPEED * DIFFICULTY)
            {
                int numProj = 12 + 10 * randf() - 10 * HPpct;
                for(int i=0; i<numProj; i++)
                {
                    Projectile::create_no_relative_velocity(game_state, unit, 10, 3, 4 + randf() * (3 - 3*HPpct), randf()*2*PI, INF, NOT_SET,
                        [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                    {
                        if(2 * randf() < TICK_SPEED * DIFFICULTY)
                            proj.misc_var[0] = randz(0, 4);
                        switch((int)proj.misc_var[0])
                        {
                        case 0:
                            proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                            break;
                        case 1:
                            proj.setX(proj.getX() - proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                            break;
                        case 2:
                            proj.setX(proj.getX() - proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() - proj.dY * TICK_SPEED);
                            break;
                        case 3:
                            proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() - proj.dY * TICK_SPEED);
                            break;
                        default:
                            break;
                        }
                        if(std::fmod(proj.rand_var, 0.01) < 0.005)
                            proj.setAngle(proj.getAngle() + (2 + 8 * proj.rand_var) * TICK_SPEED);
                        else proj.setAngle(proj.getAngle() - (2 + 8 * proj.rand_var) * TICK_SPEED);
                    });
                }
            }
        }
        else
        {
            if((0.23 - 0.04*HPpct) * randf() < TICK_SPEED * DIFFICULTY)
            {
                int is1 = randz(0, 1);
                game_state.game_map.projectiles.emplace_back(Affiliation::generic_enemy, 10 + is1, unit, 3 + is1, unit->getX(), unit->getY(), 0,
                        0, INF, 0, [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                {
                    if((proj.type==10) ^ (unit->misc_var[2]==0))
                    {
                        proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                        proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                        proj.misc_var[0] = 0;
                    }
                    else
                    {
                        if(unit->HP / unit->maxHP < 0.7)
                        {
                            double ms = std::max(std::fabs(proj.dX), std::fabs(proj.dY));
                            double theta = std::atan2(proj.getY() - unit->getY(), proj.getX() - unit->getX());
                            double dX = -ms * std::sin(theta);
                            double dY = ms * std::cos(theta);
                            proj.setX(proj.getX() + dX * TICK_SPEED);
                            proj.setY(proj.getY() + dY * TICK_SPEED);
                        }
                        if(proj.misc_var[0]==0)
                        {
                            proj.misc_var[0] = 1;
                            if(randz(0, 1) == 0)
                            {
                                proj.dX = -proj.dX;
                                proj.dY = -proj.dY;
                            }
                            if(randz(0, 1) == 0)
                            {
                                std::swap(proj.dX, proj.dY);
                            }
                        }
                    }
                    if(std::fmod(proj.rand_var, 0.01) < 0.005)
                        proj.setAngle(proj.getAngle() + (2 + 8 * proj.rand_var) * TICK_SPEED);
                    else proj.setAngle(proj.getAngle() - (2 + 8 * proj.rand_var) * TICK_SPEED);
                });
                auto &proj = game_state.game_map.projectiles.back();
                if(randz(0, 1) == 0)
                {
                    proj.dX = randsign() * (5 + 4 * randf());
                }
                else
                {
                    proj.dY = randsign() * (5 + 4 * randf());
                }
            }
        }
        if((3 - std::log2(1 + std::max(0.0, unit->age - unit->misc_var[3]))) * 2 * randf() < TICK_SPEED * DIFFICULTY)
        {
            flip(unit->misc_var[2]);
            unit->misc_var[3] = unit->age;
        }
        for(int i=0; i<2; i++)
            if(2 * randf() < TICK_SPEED * DIFFICULTY)
                flip(unit->misc_var[i]);
    };
    //
    cur = 32; //borderless player
    BASE_NAME[cur] = "player";
    BASE_WEAPON_BEHAVIOR[cur] = WeaponBehavior::player;
    BASE_HP[cur] = 50;
    BASE_MOVEMENT_SPEED[cur] = 6;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.6;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::player;
    BASE_DAMAGE_ON_COLLISION[cur] = 20;
    BASE_PROJECTILE_OFFSET[cur] = 0.7;
    //
    cur = 33;
    BASE_NAME[cur] = "white triangle";
    BASE_HP[cur] = 60;
    BASE_MOVEMENT_SPEED[cur] = 4;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[11];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_DAMAGE_ON_COLLISION[cur] = 40;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return (7 * unit->rand_var + 3) * square(std::sin((6 * unit->rand_var + 1) * getTicksS()));};
    BASE_VISION_DISTANCE[cur] = 6;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double HPpercent = unit->HP / unit->maxHP;
        unit->moveX *= 2 - HPpercent;
        unit->moveY *= 2 - HPpercent;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(unit->time_since_took_damage < 0.04 && unit->age - unit->misc_var[0] > 0.05)
        {
            unit->misc_var[0] = unit->age;
            for(int i=0; i<3; i++)
                Projectile::create(game_state, unit, 12, 8, 5 + 3 * randf(), unit->getAngle() + PI/6 + i * 2*PI/3, 1, NOT_SET);
        }
    };
    //
    cur = 34;
    BASE_NAME[cur] = "white square";
    BASE_HP[cur] = 100;
    BASE_MOVEMENT_SPEED[cur] = 5;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::cardinal;
    BASE_DAMAGE_ON_COLLISION[cur] = 50;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_VISION_DISTANCE[cur] = 6;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double HPpercent = unit->HP / unit->maxHP;
        unit->moveX *= 1.2 - 0.2*HPpercent;
        unit->moveY *= 1.2 - 0.2*HPpercent;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        //randomly fire a stream of evenly spaced projectiles in 4 directions. Increase rate when HP is low.
        if(randf() < TICK_SPEED && unit->age - unit->misc_var[0] > HPpct + 0.5)
        {
            unit->misc_var[1] = 1;
        }
        if(unit->misc_var[1] > 0)
        {
            if(unit->age - unit->misc_var[2] > 0.08)
            {
                unit->misc_var[1]++;
                if(unit->misc_var[1] == 5)
                    unit->misc_var[1] = 0;
                unit->misc_var[2] = unit->age;
                for(int i=0; i<4; i++)
                    Projectile::create(game_state, unit, 12, 8, 6 - 2*HPpct, i * PI/2, 2);
            }
        }
    };
    //
    cur = 35;
    BASE_NAME[cur] = "white 4-star";
    BASE_HP[cur] = 1500;
    BASE_MOVEMENT_SPEED[cur] = 5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[6];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 70;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return (2 + std::max(0.0, 3 * (1.5 - unit->time_since_took_damage)) + square(std::sin((4 * unit->rand_var + 2) * getTicksS())));};
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(0.5 * randf() < TICK_SPEED * DIFFICULTY || (unit->time_since_took_damage < 0.04 && unit->age - unit->misc_var[0] > 0.05))
        {
            unit->misc_var[0] = unit->age;
            for(int i=0; i<4; i++)
            {
                Projectile::create(game_state, unit, 12, 8, 0, unit->getAngle() + i * PI/2, 5, NOT_SET,
                    [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                {
                    proj.dX = proj.misc_var[0] + proj.misc_var[1] * std::sin(proj.misc_var[1] * proj.age);
                    proj.dY = proj.misc_var[2] + proj.misc_var[3] * std::cos(proj.misc_var[3] * proj.age);
                    proj.setX(proj.getX() + proj.dX * TICK_SPEED);
                    proj.setY(proj.getY() + proj.dY * TICK_SPEED);
                });
                auto &proj = game_state.game_map.projectiles.back();
                double r = (1.5 - 0.5*HPpct) * (2.1 + 2.1*randf());
                double theta = randf() * 2*PI;
                proj.misc_var[0] = r * std::cos(theta);
                proj.misc_var[2] = r * std::sin(theta);
                proj.misc_var[1] = (1.5 - 0.5*HPpct) * (1 + 3 * randf()) * randsign();
                proj.misc_var[3] = (1.5 - 0.5*HPpct) * (1 + 3 * randf()) * randsign();
            }
        }
    };
    //
    cur = 36;
    BASE_NAME[cur] = "white 5-star";
    BASE_HP[cur] = 1500;
    BASE_MOVEMENT_SPEED[cur] = 5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[7];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 70;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return (2 + std::max(0.0, 1.2 * (1.5 - unit->time_since_took_damage)) + square(std::sin((4 * unit->rand_var + 2) * getTicksS())));};
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(0.04 * randf() < TICK_SPEED * DIFFICULTY)
        {
                Projectile::create(game_state, unit, 12, 8, 5 - HPpct + randf(), randf()*2*PI, 5, NOT_SET,
                    [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                {
                    double HPpct = unit->HP / unit->maxHP;
                    double mag = 1 + std::max(0.0, 0.4 * (1.5 - unit->time_since_took_damage));
                    mag *= 1 + (1 - HPpct) * 0.5 * std::sin((2 + 5 * proj.rand_var) * proj.age);
                    double dX = proj.dX * mag;
                    double dY = proj.dY * mag;
                    proj.setX(proj.getX() + dX * TICK_SPEED);
                    proj.setY(proj.getY() + dY * TICK_SPEED);
                });
        }
    };
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = 1 + std::max(0.0, 0.4 * (1.5 - unit->time_since_took_damage));
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    //
    cur = 37;
    BASE_NAME[cur] = "purple square";
    BASE_HP[cur] = 17;
    BASE_MOVEMENT_SPEED[cur] = 4.5;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::cardinal;
    BASE_DAMAGE_ON_COLLISION[cur] = 25;
    BASE_PROJECTILE_OFFSET[cur] = BASE_SIZE1[cur] / 2;
    BASE_VISION_DISTANCE[cur] = 8;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = std::max(1.0,  2.0 - 0.2 * unit->dist(game_state.getPlayer().get()));
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(unit->dist(game_state.getPlayer().get()) < unit->getVisionDistance(game_state) && unit->misc_var[0] + 1 < unit->age)
        {
            unit->misc_var[0] = unit->age;
            double ang = std::atan2(game_state.getPlayer()->getY() - unit->getY(), game_state.getPlayer()->getX() - unit->getX());
            for(double i=-1.5; i<=1.5; i++)
                Projectile::create(game_state, unit, 7, 5, 5, ang + i * PI/8, 2);
        }
    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double ang = unit->getAngle();
        for(int i=0; i<4; i++)
        {
            Projectile::create_no_relative_velocity(game_state, unit, 7, 3.5, 6, ang + i * PI/2, 2);
        }
    };
    //
    cur = 38;
    BASE_NAME[cur] = "red crescent";
    BASE_HP[cur] = 14;
    BASE_MOVEMENT_SPEED[cur] = 4.7;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(10);
    BASE_VERTICES[cur][0] = Point(-0.01, 0.00);
    BASE_VERTICES[cur][1] = Point(0.07, 0.31);
    BASE_VERTICES[cur][2] = Point(0.25, 0.49);
    BASE_VERTICES[cur][3] = Point(-0.06, 0.43);
    BASE_VERTICES[cur][4] = Point(-0.26, 0.23);
    BASE_VERTICES[cur][5] = Point(-0.32, 0.00);
    BASE_VERTICES[cur][6] = Point(-0.26, -0.23);
    BASE_VERTICES[cur][7] = Point(-0.06, -0.43);
    BASE_VERTICES[cur][8] = Point(0.25, -0.49);
    BASE_VERTICES[cur][9] = Point(0.07, -0.31);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 2 + unit->rand_var;};
    BASE_DAMAGE_ON_COLLISION[cur] = 20;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if((1.2 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            double ang = unit->getAngle();
            for(int i=-1; i<=1; i++)
                Projectile::create_no_relative_velocity(game_state, unit, 3, 3.5, 6.5, ang + i * PI/4, 1);
        }
    };
    //
    cur = 39;
    BASE_NAME[cur] = "yellow crescent";
    BASE_HP[cur] = 27;
    BASE_MOVEMENT_SPEED[cur] = 5;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[38];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 2 + unit->rand_var;};
    BASE_DAMAGE_ON_COLLISION[cur] = 27;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = 1 + 0.25 * std::sin((1 + 2 * unit->rand_var) * unit->age);
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if((1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            double ang = unit->getAngle();
            for(double i=-1.5; i<=1.5; i++)
                Projectile::create_no_relative_velocity(game_state, unit, 5, 4, 7, ang + i * PI/6, 2);
        }
    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double ang = unit->getAngle();
        for(int i=0; i<6; i++)
        {
            Projectile::create_no_relative_velocity(game_state, unit, 5, 4, 7, ang + i * PI/3, 2);
        }
    };
    //
    cur = 40;
    BASE_NAME[cur] = "green crescent";
    BASE_HP[cur] = 45;
    BASE_MOVEMENT_SPEED[cur] = 4.7;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[38];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 2 + 1.5 * std::sin((3 + 2 * unit->rand_var) * unit->age);};
    BASE_DAMAGE_ON_COLLISION[cur] = 27;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = 1 + 0.3 * std::sin((3 + 2 * unit->rand_var) * unit->age);
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(unit->misc_var[0] != 0)
        {
            if(unit->age - unit->misc_var[1] > 0.2)
            {
                unit->misc_var[0]--;
                unit->misc_var[1] = unit->age;
                double ang = unit->getAngle();
                for(double i=-2; i<=2; i++)
                    Projectile::create(game_state, unit, 4, 4.5, 7.5, ang + i * PI/8, 1);
            }
        }
        else if((1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            unit->misc_var[0] = 3;
        }
    };
    //
    cur = 42;
    BASE_NAME[cur] = "white crescent";
    BASE_HP[cur] = 110;
    BASE_MOVEMENT_SPEED[cur] = 4;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[38];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 2.5 + 2.5 * std::sin((3 + 2 * unit->rand_var) * unit->age);};
    BASE_DAMAGE_ON_COLLISION[cur] = 50;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = 1 + std::sin((3 + 2 * unit->rand_var) * unit->age);
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(unit->misc_var[0] != 0)
        {
            if(unit->age - unit->misc_var[1] > 0.2)
            {
                unit->misc_var[0]--;
                unit->misc_var[1] = unit->age;
                double ang = unit->getAngle();
                for(double i=-3; i<=3; i++)
                    Projectile::create(game_state, unit, 12, 5.5, 8, ang + i * PI/12, 1);
            }
        }
        else if((1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            unit->misc_var[0] = 4;
        }
    };
    //
    cur = 43;
    BASE_NAME[cur] = "big red plus";
    BASE_HP[cur] = 280;
    BASE_MOVEMENT_SPEED[cur] = 4;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = static_array<Point>(12);
    BASE_VERTICES[cur][0] = Point(0.48, -1.46);
    BASE_VERTICES[cur][1] = Point(0.48, -0.47);
    BASE_VERTICES[cur][2] = Point(1.47, -0.47);
    BASE_VERTICES[cur][3] = Point(1.47, 0.48);
    BASE_VERTICES[cur][4] = Point(0.48, 0.48);
    BASE_VERTICES[cur][5] = Point(0.48, 1.47);
    BASE_VERTICES[cur][6] = Point(-0.47, 1.47);
    BASE_VERTICES[cur][7] = Point(-0.47, 0.48);
    BASE_VERTICES[cur][8] = Point(-1.46, 0.48);
    BASE_VERTICES[cur][9] = Point(-1.46, -0.47);
    BASE_VERTICES[cur][10] = Point(-0.47, -0.47);
    BASE_VERTICES[cur][11] = Point(-0.47, -1.46);
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 2 + unit->rand_var + std::sin((1 + 2 * unit->rand_var) * unit->age);};
    BASE_DAMAGE_ON_COLLISION[cur] = 40;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if((0.7 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            auto &p = game_state.game_map.projectiles;
            const static double xoffset = 1.47;
            const static double yoffset[]{-0.4, -0.2, 0, 0.2, 0.4};
            double x = unit->getX(), y = unit->getY();
            for(int i=0; i<=3; i++)
            {
                for(int j=0; j<5; j++)
                {
                    double uang = unit->getAngle() + i * PI / 2;
                    double theta = std::atan2(yoffset[j], xoffset);
                    double r = std::hypot(xoffset, yoffset[j]);
                    p.emplace_back(Affiliation::generic_enemy, 3, unit, 4, x + r * std::cos(theta + uang), y + r * std::sin(theta + uang),
                               6 * std::cos(uang), 6 * std::sin(uang), 5, 0);
                }
            }
        }
        if(std::fmod(unit->age, 1 + HPpct) < 1 && 0.11 * randf() < TICK_SPEED * DIFFICULTY)
        {
            for(int i=0; i<=3; i++)
            {
                Projectile::create(game_state, unit, 3, 4, 5.5, unit->getAngle() + i * PI/2 + PI/4, 5, 0.66);
            }
        }
    };
    //
    cur = 44;
    BASE_NAME[cur] = "big white plus";
    BASE_HP[cur] = 1400;
    BASE_MOVEMENT_SPEED[cur] = 4.7;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[43];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
        {return 2 + unit->rand_var + 2 * std::sin((1 + 2 * unit->rand_var) * unit->age);};
    BASE_DAMAGE_ON_COLLISION[cur] = 100;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = 1 + 0.6 * std::sin((1 + 2 * unit->rand_var) * unit->age);
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if((1.3 + 0.7*HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            auto &p = game_state.game_map.projectiles;
            const static double xoffset = 1.47;
            const static double yoffset[]{-0.4, -0.2, 0, 0.2, 0.4};
            double x = unit->getX(), y = unit->getY();
            for(int i=0; i<=3; i++)
            {
                double rnd = randf();
                for(int j=0; j<5; j++)
                {
                    double uang = unit->getAngle() + i * PI / 2;
                    double theta = std::atan2(yoffset[j], xoffset);
                    double r = std::hypot(xoffset, yoffset[j]);
                    p.emplace_back(Affiliation::generic_enemy, 12, unit, 7, x + r * std::cos(theta + uang), y + r * std::sin(theta + uang),
                            8 * std::cos(uang), 8 * std::sin(uang), 20, unit->getAngle(),
                            [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                    {
                        double HPpct = unit->HP / unit->maxHP;
                        if(proj.misc_var[1] + proj.rand_var + 1 < proj.age && std::fmod(proj.age, 0.5 + HPpct + proj.rand_var) < 1)
                        {
                            proj.misc_var[1] = proj.age;
                            proj.misc_var[0] = (int)(1.99 * std::sin(std::sin(2 * proj.age + (1 + proj.rand_var)) + proj.age * (2 + proj.rand_var)));
                        }
                        if(proj.misc_var[0] != 0)
                        {
                            proj.setX(proj.getX() + proj.misc_var[0] * proj.dX * TICK_SPEED);
                            proj.setY(proj.getY() + proj.misc_var[0] * proj.dY * TICK_SPEED);
                        }
                    });
                    p.back().rand_var = rnd; //all in a group have the same rand_var
                }
            }
        }
        if(std::fmod(unit->age, 1 + HPpct) < 1 && 0.15 * randf() < TICK_SPEED * DIFFICULTY)
        {
            for(int i=0; i<=3; i++)
            {
                Projectile::create(game_state, unit, 12, 7, 6, unit->getAngle() + i * PI/2 + PI/4, 5, 0.66);
            }
        }
    };
    //
    cur = 48;
    BASE_NAME[cur] = "red circle";
    BASE_HP[cur] = 15;
    BASE_MOVEMENT_SPEED[cur] = 4.2;
    BASE_SHAPE[cur] = GeometricObject::Type::circle;
    BASE_SIZE1[cur] = 0.45;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_DAMAGE_ON_COLLISION[cur] = 15;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = std::max(1.0,  2.0 - 0.2 * unit->dist(game_state.getPlayer().get()));
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double ang = unit->getAngle();
        for(int i=0; i<8; i++)
        {
            Projectile::create_no_relative_velocity(game_state, unit, 3, 3, 5.8, ang + i * PI/4, 1);
        }
    };
    //
    cur = 49;
    BASE_NAME[cur] = "yellow circle";
    BASE_HP[cur] = 37;
    BASE_MOVEMENT_SPEED[cur] = 3.9;
    BASE_SHAPE[cur] = GeometricObject::Type::circle;
    BASE_SIZE1[cur] = 0.45;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::random;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_DAMAGE_ON_COLLISION[cur] = 30;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = std::max(1.0,  3.0 - 0.4 * unit->dist(game_state.getPlayer().get()));
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double ang = unit->getAngle();
        for(int i=0; i<12; i++)
        {
            Projectile::create_no_relative_velocity(game_state, unit, 5, 3, 5.8, ang + i * PI/6, 1);
        }
    };
    //
    cur = 50;
    BASE_NAME[cur] = "white heart";
    BASE_HP[cur] = 90;
    BASE_HP_REGEN[cur] = 20;
    BASE_MOVEMENT_SPEED[cur] = 7;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[23];
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_DAMAGE_ON_COLLISION[cur] = 53;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 3 + 2 * unit->rand_var;};
    BASE_PROJECTILE_OFFSET[cur] = Polygon(NOT_SET, NOT_SET, BASE_VERTICES[cur]).getRadius();
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(unit->misc_var[0] != 0)
        {
            if(unit->age - unit->misc_var[1] > 0.3)
            {
                unit->misc_var[0]--;
                unit->misc_var[1] = unit->age;
                double ang = unit->getAngle();
                for(double i=0; i<=8; i++)
                    Projectile::create(game_state, unit, 12, 8, 6, ang + i * PI/4, 1);
            }
        }
        else if((2 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            unit->misc_var[0] = 3;
        }
    };
    //
    cur = 51;
    BASE_NAME[cur] = "mini blue heart";
    BASE_HP[cur] = INF;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[23];
    for(auto &i: BASE_VERTICES[cur])
    {
        i.x /= 2;
        i.y /= 2;
    }
    BASE_DAMAGE_ON_COLLISION[cur] = 0;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::pure_custom;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(unit->owner->isDead() || game_state.game_map.isFloorFinished()) //this dies when its owner dies or the floor is over
            unit->HP = 0;
        //rotate around the owner
        //[0] = r, [1] = theta_offset
        unit->setX(unit->owner->getX() + unit->misc_var[0] * std::cos(unit->owner->getAngle() + unit->misc_var[1]));
        unit->setY(unit->owner->getY() + unit->misc_var[0] * std::sin(unit->owner->getAngle() + unit->misc_var[1]));
    };
    BASE_TAKE_DAMAGE_FUNC[cur] = [](Unit *a, Unit *b, double damage) -> void
    {
        Unit::dealDamage(b, a, damage * 0.25); //CODE_WARNING: care to avoid infinite recursion here
    };
    //
    cur = 52;
    BASE_NAME[cur] = "mini blue heart";
    BASE_HP[cur] = INF;
    BASE_SHAPE[cur] = GeometricObject::Type::polygon;
    BASE_VERTICES[cur] = BASE_VERTICES[51];
    BASE_MOVEMENT_SPEED[cur] = 6;
    BASE_DAMAGE_ON_COLLISION[cur] = 0;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::bidirectional;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double {return 3 + 2 * unit->rand_var;};
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(game_state.game_map.isFloorFinished()) //this dies when the floor is over
            unit->HP = 0;
    };
    BASE_TAKE_DAMAGE_FUNC[cur] = [](Unit *a, Unit *b, double damage) -> void
    {
        Unit::dealDamage(b, a, damage * 0.25); //CODE_WARNING: care to avoid infinite recursion here
    };
    //
    cur = 53;
    BASE_NAME[cur] = "big green square";
    BASE_HP[cur] = 400;
    BASE_MOVEMENT_SPEED[cur] = 4;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 1.6;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::bouncy;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::unidirectional;
    BASE_PROJECTILE_OFFSET[cur] = 0.8;
    BASE_DAMAGE_ON_COLLISION[cur] = 50;
    BASE_ROTATION_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> double
    {
        if(std::fmod(unit->age, 2) < 1)
            return PI;
        return 0;
    };
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = 1.5 - 0.5 * unit->HP / unit->maxHP;
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        if(0.2 * randf() < TICK_SPEED * DIFFICULTY)
        {
            double ang = unit->getAngle();
            for(int i=0; i<4; i++)
                Projectile::create(game_state, unit, 4, 6, 5, ang + i * PI/2, 5);
        }
    };
    //
    cur = 54;
    BASE_NAME[cur] = "small rainbow circle";
    BASE_HP[cur] = 50;
    BASE_MOVEMENT_SPEED[cur] = 6;
    BASE_SHAPE[cur] = GeometricObject::Type::circle;
    BASE_SIZE1[cur] = 0.2;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_DAMAGE_ON_COLLISION[cur] = 50;
    BASE_VISION_DISTANCE[cur] = INF;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double mag = 1.5 - 0.5 * unit->HP / unit->maxHP;
        unit->moveX *= mag;
        unit->moveY *= mag;
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {

    };
    GEN_CUSTOM_TEXTURE[cur] = []() -> SDL_Texture*
    {
        static static_array<SDL_Texture*> textures(20);
        bool alreadyInit = false;
        if(!alreadyInit)
        {
            alreadyInit = true;
            for(int i=0; i<(int)textures.size(); i++)
            {
                SDL_Surface *s = SDL_CreateRGBSurface(0, 50, 50, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
                for(int i=0; i<50; i++)
                {
                    for(int j=0; j<50; j++)
                    {
                        Uint32 *pixel = (Uint32*)((uint8_t*)s->pixels + i * s->pitch + j * 4);
                        if(square(i - 24.5) + square(j - 24.5) < square(24.5))
                            *pixel = (((Uint32)(randz(0, (1<<24)-1))) << 8) | 0xff;
                        else *pixel = 0xffffffffu;
                    }
                }
                SDL_SetColorKey(s, SDL_TRUE, SDL_MapRGB(s->format, 255, 255, 255));
                SDL_Texture *t = SDL_CreateTextureFromSurface(getRenderer(), s);
                SDL_FreeSurface(s);
                textures[i] = t;
            }
        }
        return textures.get(randz(0, textures.size()-1));
    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double ang = unit->getAngle();
        for(int i=0; i<12; i++)
        {
            Projectile::create_no_relative_velocity(game_state, unit, 9 + 7 * randz(0, 1), 9, 8, ang + i * PI/6, 1);
        }
    };
    //
    cur = 55;
    BASE_NAME[cur] = "big white square";
    BASE_HP[cur] = 1000;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 1.6;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::pure_custom;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_DAMAGE_ON_COLLISION[cur] = 200;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        if(unit->moveX == 0)
        {
            if(randf() < TICK_SPEED)
            {
                if(randf() < 0.5)
                    unit->moveX = (4 + 8 * randf());
                else unit->moveX = -(4 + 8 * randf());
            }
        }
        else if(3 * randf() < TICK_SPEED)
        {
            unit->moveX = 0;
        }
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if((1 + HPpct) * (0.3 + 0.15 * game_state.game_map.units.size()) * randf() < TICK_SPEED * DIFFICULTY)
        {
            int utype;
            double y = 1.5;
            if(unit->age > 5 && randf() < 0.5)
                y = game_state.game_map.getNumRows() - 2.5;
            switch(randz(0, 3))
            {
            case 0:
                utype = 33;
                break;
            case 1:
                utype = 34;
                break;
            case 2:
                utype = 42;
                break;
            case 3:
                utype = 56;
                break;
            }
            for(int tries=0; tries<10; tries++)
            {
                std::shared_ptr<Unit> u = std::make_shared<Unit>(utype, Affiliation::generic_enemy, 1 + randf() * 19, y);
                if(!u->collidesWithOtherUnit_checkToValidateMap(game_state.game_map.units) && !u->collidesWithTerrain(game_state.game_map) &&
                   u->dist(game_state.getPlayer().get()) > 2.5)
                {
                    game_state.game_map.units.push_back(u);
                    break;
                }
            }
        }
        if((1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
        {
            for(int i=0; i<26; i++)
                Projectile::create(game_state, unit, 12, 8, 7, i * PI/13, 5);
        }
        if(unit->moveX==0 && 0.1 * randf() < TICK_SPEED)
        {
            double ang = std::atan2(game_state.getPlayer()->getY() - unit->getY(), game_state.getPlayer()->getX() - unit->getX());
            Projectile::create(game_state, unit, 12, 8, 10, ang, 5);
        }
        for(auto &i: game_state.game_map.units)
        {
            if(!i->isPlayer() && i->ID != unit->ID)
            {
                i->HP -= TICK_SPEED * 0.1 * i->maxHP;
            }
        }
    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        for(auto &i: game_state.game_map.units)
        {
            if(!i->isPlayer() && i->ID != unit->ID)
            {
                i->HP = 0;
            }
        }
        if(game_state.currentFloor == 19)
        {
            game_state.game_map.gravity = 0;
            game_state.getPlayer()->moveX = game_state.getPlayer()->moveY = 0;
            game_state.game_map.units.push_back(std::make_shared<Unit>(57, Affiliation::generic_enemy, unit->getX(), unit->getY()));
            for(int i=1; i<game_state.game_map.getNumColumns()-1; i++)
                for(int j=1; j<game_state.game_map.getNumRows()-1; j++)
                    if((i<=4 || i>=16) && game_state.game_map.tiles[i][j] == MapTile::wall)
                    {
                        aestheticEffects.push_back(std::make_shared<DisappearingObject>(game_state.game_map.current_wall_sprite,
                                                    0.5, i, j, 0, false));
                        game_state.game_map.tiles[i][j] = MapTile::dirt;
                    }
        }
    };
    //
    cur = 56;
    BASE_NAME[cur] = "white circle";
    BASE_HP[cur] = 50;
    BASE_MOVEMENT_SPEED[cur] = 7;
    BASE_SHAPE[cur] = GeometricObject::Type::circle;
    BASE_SIZE1[cur] = 0.4;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_VISION_DISTANCE[cur] = INF;
    BASE_DAMAGE_ON_COLLISION[cur] = 50;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {

    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double ang = unit->getAngle();
        for(int i=0; i<12; i++)
        {
            Projectile::create_no_relative_velocity(game_state, unit, 12, 8, 8, ang + i * PI/6, 1);
        }
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {

    };
    //
    cur = 57;
    BASE_NAME[cur] = "randomly colored square";
    BASE_HP[cur] = 1500;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 1.6;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::pure_custom;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_DAMAGE_ON_COLLISION[cur] = 300;
    GEN_CUSTOM_TEXTURE[cur] = []() -> SDL_Texture*
    {
        static SDL_Texture *t;
        bool alreadyInit = false;
        if(!alreadyInit)
        {
            alreadyInit = true;
            SDL_Surface *s = SDL_CreateRGBSurface(0, 160, 160, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
            for(int i=0; i<160; i++)
            {
                for(int j=0; j<160; j++)
                {
                    Uint32 *pixel = (Uint32*)((uint8_t*)s->pixels + i * s->pitch + j * 4);
                    *pixel = (((Uint32)(randz(0, (1<<24)-1))) << 8) | 0xff;
                }
            }
            t = SDL_CreateTextureFromSurface(getRenderer(), s);
            SDL_FreeSurface(s);
        }
        return t;
    };
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        if((unit->moveX==0 && unit->moveY==0 && 0.4 * randf() < TICK_SPEED) || randf() < TICK_SPEED)
        {
            if(randf() < 0.5)
                unit->moveX = (2 + 5 * randf());
            else unit->moveX = -(2 + 5 * randf());
            if(randf() < 0.5)
                unit->moveY = (3 + 6 * randf());
            else unit->moveY = -(3 + 6 * randf());
            double diffX = game_state.getPlayer()->getX() - unit->getX();
            double diffY = game_state.getPlayer()->getY() - unit->getY();
            unit->moveX += sign(diffX) * std::sqrt(std::fabs(diffX));
            unit->moveY += sign(diffY) * std::sqrt(std::fabs(diffY));

        }
        if(2 * randf() < TICK_SPEED)
        {
            unit->moveX = 0;
            unit->moveY = 0;
        }
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        double HPpct = unit->HP / unit->maxHP;
        if(HPpct < 0.6)
        {
            if((1 + HPpct) * (0.3 + 0.15 * game_state.game_map.units.size()) * randf() < TICK_SPEED * DIFFICULTY)
            {
                int utype = 54;
                for(int tries=0; tries<20; tries++)
                {
                    std::shared_ptr<Unit> u = std::make_shared<Unit>(utype, Affiliation::generic_enemy, unit->getX() + 4 * (randf() - 0.5),
                                                                     unit->getY() + 4 * (randf() - 0.5));
                    if(!u->collidesWithOtherUnit_checkToValidateMap(game_state.game_map.units) &&
                       !u->collidesWithTerrain(game_state.game_map))
                    {
                        game_state.game_map.units.push_back(u);
                        break;
                    }
                }
            }
            if(HPpct < 0.3)
            {
                if((2 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
                {
                    unit->misc_var[0] = 4;
                    unit->misc_var[1] = unit->age;
                }
                if(unit->misc_var[0]>0 && unit->age - 0.1 > unit->misc_var[1])
                {
                    unit->misc_var[0]--;
                    unit->misc_var[1] = unit->age;
                    for(int i=-1; i<=1; i++)
                        for(int j=-1; j<=1; j++)
                            if(i!=0 || j!=0)
                                Projectile::create(game_state, unit, 9 + 7 * randz(0, 1), 9, 8, std::atan2(i, j), 5, 0);
                }
            }
            if((1 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
            {
                for(int i=0; i<20; i++)
                {
                    Projectile::create(game_state, unit, 9 + 7 * randz(0, 1), 9, 0, i * PI/10, 5, 0,
                        [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
                        {
                            double r = proj.age * 4;
                            double angle = proj.misc_var[0] + proj.age * 1.1;
                            proj.setX(proj.misc_var[1] + r * std::cos(angle));
                            proj.setY(proj.misc_var[2] + r * std::sin(angle));
                        });
                    auto &pr = game_state.game_map.projectiles.back();
                    pr.misc_var[0] = i * PI/10;
                    pr.misc_var[1] = unit->getX();
                    pr.misc_var[2] = unit->getY();
                }
            }
        }
        else
        {
            if((0.5 + HPpct) * randf() < TICK_SPEED * DIFFICULTY)
            {
                for(int i=0; i<22; i++)
                    Projectile::create(game_state, unit, 9 + 7 * randz(0, 1), 9, 7.5, i * PI/12, 5);
            }
        }
        if(unit->moveX==0 && unit->moveY==0 && 0.1 * randf() < TICK_SPEED)
        {
            double ang = std::atan2(game_state.getPlayer()->getY() - unit->getY(), game_state.getPlayer()->getX() - unit->getX());
            for(int i=-1; i<=1; i++)
                Projectile::create(game_state, unit, 9 + 7 * randz(0, 1), 9, 10, ang + i * PI/6, 5);
        }
        for(auto &i: game_state.game_map.units)
        {
            if(!i->isPlayer() && i->ID != unit->ID)
            {
                i->HP -= TICK_SPEED * 0.3 * i->maxHP;
            }
        }
    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        for(auto &i: game_state.game_map.units)
        {
            if(!i->isPlayer() && i->ID != unit->ID)
            {
                i->HP = 0;
            }
        }
    };
    //
    cur = 58;
    BASE_NAME[cur] = "small rainbow circle 2";
    BASE_HP[cur] = 50;
    BASE_MOVEMENT_SPEED[cur] = 7;
    BASE_SHAPE[cur] = GeometricObject::Type::circle;
    BASE_SIZE1[cur] = 0.2;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::pure_custom;
    BASE_ROTATION_BEHAVIOR[cur] = RotationBehavior::none;
    BASE_DAMAGE_ON_COLLISION[cur] = 50;
    BASE_VISION_DISTANCE[cur] = INF;
    BASE_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        unit->setX(unit->getX() + unit->moveX * TICK_SPEED);
        unit->setY(unit->getY() + unit->moveY * TICK_SPEED);
    };
    BASE_OTHER_BEHAVIOR[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle) -> void
    {
        unit->HP -= unit->maxHP * TICK_SPEED;
    };
    GEN_CUSTOM_TEXTURE[cur] = []() -> SDL_Texture*
    {
        static static_array<SDL_Texture*> textures(20);
        bool alreadyInit = false;
        if(!alreadyInit)
        {
            alreadyInit = true;
            for(int i=0; i<(int)textures.size(); i++)
            {
                SDL_Surface *s = SDL_CreateRGBSurface(0, 50, 50, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
                for(int i=0; i<50; i++)
                {
                    for(int j=0; j<50; j++)
                    {
                        Uint32 *pixel = (Uint32*)((uint8_t*)s->pixels + i * s->pitch + j * 4);
                        if(square(i - 24.5) + square(j - 24.5) < square(24.5))
                            *pixel = (((Uint32)(randz(0, (1<<24)-1))) << 8) | 0xff;
                        else *pixel = 0xffffffffu;
                    }
                }
                SDL_SetColorKey(s, SDL_TRUE, SDL_MapRGB(s->format, 255, 255, 255));
                SDL_Texture *t = SDL_CreateTextureFromSurface(getRenderer(), s);
                SDL_FreeSurface(s);
                textures[i] = t;
            }
        }
        return textures.get(randz(0, textures.size()-1));
    };
    BASE_ON_DEATH_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit) -> void
    {
        double ang = unit->getAngle();
        for(int i=0; i<12; i++)
        {
            Projectile::create_no_relative_velocity(game_state, unit, 9 + 7 * randz(0, 1), 9, 8, ang + i * PI/6, 1);
        }
    };
    //
    for(int i=0; i<NUM_UNITS; i++)
    {
        if(BASE_NAME[i]!="" && GEN_CUSTOM_TEXTURE[i]==nullptr)
        {
            sprites[i] = loadTexture(("data/unit/" + to_str(i) + ".png").c_str(), 255, 255, 255);
            BASE_MONEY_REWARD[i] = 0.1*BASE_HP[i] + 0.3*BASE_HP_REGEN[i];
        }
    }
}
void Weapon::init()
{
    for(auto &i: BASE_AMMO) //if it's not set by at the end of this function just assume that BASE_AMMO[i] = BASE_AMMO_PER_PURCHASE[i]
        i = NOT_SET;
    for(auto &i: BASE_PROJ_PER_SHOT) //things fire 1 projectile per shot by default
        i = 1;
    for(auto &i: BASE_FIRERATE_IRREGULARITY) //by default everything has a perfectly consistent fire rate
        i = 0;
    for(auto &i: BASE_SPREAD) //all weapons are perfectly accurate by default
        i = 1;
    for(auto &i: HAS_SPRITE) //all weapons have sprites by default
        i = true;
    for(auto &i: BASE_SPELL_COST) //by default, weapons don't have spells (so they effectively cost NOT_SET ~= INF mana to cast)
        i = NOT_SET;
    for(auto &i: BASE_MAXHP_PCT_DAMAGE) //no %maxHP damage by default
        i = 0;
    for(auto &i: BASE_PROJ_MOVEMENT_FUNC) //no custom movement by default
        i = nullptr;
    for(auto &i: BASE_PIERCE_COUNT) //weapons fire projectiles that only hit one enemy by default
        i = 1;
    for(auto &i: BASE_IS_SPECTRAL) //projectiles aren't spectral by default
        i = false;
    for(auto &i: BASE_FIRE_FUNC) //no custom fire function by default
        i = nullptr;
    //
    int cur = 0;
    BASE_NAME[cur] = "Poofy Pistol";
    BASE_DESC[cur] = "You shouldn't be seeing this text that describes the Poofy Pistol because "
    "the Poofy Pistol isn't supposed to appear in the shop.";
    BASE_PROJ_TYPE[cur] = 0;
    BASE_AMMO[cur] = INF;
    BASE_DAMAGE[cur] = 4;
    BASE_FIRE_RATE[cur] = 0.55;
    BASE_SHOT_SPEED[cur] = 11;
    BASE_PROJ_LIFESPAN[cur] = 0.65;
    BASE_SPREAD[cur] = 0.15;
    BASE_SPELL_COST[cur] = 8;
    BASE_SPELL_COOLDOWN[cur] = 8;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_poofy_pistol.wav", 0.7);
    //
    cur = 1;
    BASE_NAME[cur] = "Bromine Bow";
    BASE_DESC[cur] = "A cheap bow that sells for $0 on eBay after transaction fees. 0% real halogens.";
    BASE_COST[cur] = 0;
    BASE_PROJ_TYPE[cur] = 1;
    BASE_DAMAGE[cur] = 6;
    BASE_FIRE_RATE[cur] = 0.61;
    BASE_SHOT_SPEED[cur] = 13;
    BASE_PROJ_LIFESPAN[cur] = 0.67;
    BASE_AMMO_PER_PURCHASE[cur] = 30;
    BASE_AMMO_COST[cur] = 10;
    BASE_SPREAD[cur] = 0.1;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_bromine_bow.wav");
    //
    cur = 2;
    BASE_NAME[cur] = "Diamond Crossbow"; //former name: Cadmium Crossbow
    BASE_DESC[cur] = "There is extremely small text printed on the weapon box that says, \"Disclaimer: Diamond is the company name, not an adjective.\"";
    BASE_COST[cur] = 80;
    BASE_PROJ_TYPE[cur] = 1;
    BASE_DAMAGE[cur] = 8;
    BASE_FIRE_RATE[cur] = 0.71;
    BASE_SHOT_SPEED[cur] = 16.5;
    BASE_PROJ_LIFESPAN[cur] = 0.64;
    BASE_AMMO_PER_PURCHASE[cur] = 25;
    BASE_AMMO_COST[cur] = 15;
    BASE_SPREAD[cur] = 0.08;
    BASE_SPELL_COOLDOWN[cur] = 6;
    BASE_SPELL_COST[cur] = 6;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_cadmium_crossbow.wav");
    //
    cur = 3;
    BASE_NAME[cur] = "Device WH-722";
    BASE_DESC[cur] = "The name obviously tells you all you need to know.";
    BASE_COST[cur] = 30;
    BASE_PROJ_TYPE[cur] = 2;
    BASE_DAMAGE[cur] = 0.8;
    BASE_FIRE_RATE[cur] = 0.91;
    BASE_SHOT_SPEED[cur] = 9.2;
    BASE_PROJ_LIFESPAN[cur] = 0.58;
    BASE_AMMO_PER_PURCHASE[cur] = 20;
    BASE_AMMO_COST[cur] = 10;
    BASE_SPREAD[cur] = PI / 4;
    BASE_PROJ_PER_SHOT[cur] = 20;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_device_wh722.wav");
    //
    cur = 4;
    BASE_NAME[cur] = "Unbranded Rifle";
    BASE_DESC[cur] = "It was probably made in an obscure Chinese sweatshop. It's probably best to either purchase five of them or get a warranty plan.";
    BASE_COST[cur] = 76;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 22;
    BASE_FIRE_RATE[cur] = 2.15;
    BASE_SHOT_SPEED[cur] = 18.6;
    BASE_PROJ_LIFESPAN[cur] = 0.65;
    BASE_AMMO_PER_PURCHASE[cur] = 20;
    BASE_AMMO_COST[cur] = 20;
    BASE_SPREAD[cur] = 0.135;
    BASE_SPELL_COST[cur] = 9;
    BASE_SPELL_COOLDOWN[cur] = 7;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_unbranded_sniper_rifle.wav");
    //
    cur = 5;
    BASE_NAME[cur] = "Paintball Gun";
    BASE_DESC[cur] = "Shoots lots of paintballs. Slightly broken and fires irregularly. Probably not intended to be used as a weapon.";
    BASE_COST[cur] = 45;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 3.5;
    BASE_FIRE_RATE[cur] = 0.23;
    BASE_SHOT_SPEED[cur] = 9.8;
    BASE_PROJ_LIFESPAN[cur] = 0.36;
    BASE_AMMO_PER_PURCHASE[cur] = 60;
    BASE_AMMO_COST[cur] = 7;
    BASE_SPREAD[cur] = 0.27;
    BASE_FIRERATE_IRREGULARITY[cur] = 1; //fire interval ranges from fireRate to 1-2x fireRate
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_paintball_gun.wav");
    //
    cur = 200;
    BASE_NAME[cur] = "Dart Gun";
    BASE_DESC[cur] = "It's an obsolete S-tier dart shooter that only shoots professionally made darts. Unfortunately, it's probably not allowed in most games of darts and it's not S-tier unless your target is a dartboard. However, it's a semi-decent object to repurpose as a weapon when on a budget.";
    BASE_COST[cur] = 20;
    BASE_PROJ_TYPE[cur] = 1;
    BASE_DAMAGE[cur] = 13;
    BASE_FIRE_RATE[cur] = 0.72;
    BASE_SHOT_SPEED[cur] = 14.5;
    BASE_PROJ_LIFESPAN[cur] = 0.8;
    BASE_AMMO_PER_PURCHASE[cur] = 15;
    BASE_AMMO_COST[cur] = 30;
    BASE_SPREAD[cur] = 0.07;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_paintball_gun.wav");
    //
    cur = 201;
    BASE_NAME[cur] = "Smol Stick";
    BASE_DESC[cur] = "It looks like it was collected from the pile of fallen twigs outside and then marked up from $0 to its retail price. Like its bigger cousins, it needs ammo to operate (what kind of stick doesn't need ammo to operate?). Special: Smol Stick Energy, predictably, doesn't do anything except waste mana.";
    BASE_COST[cur] = 37;
    BASE_PROJ_TYPE[cur] = 6;
    BASE_DAMAGE[cur] = 0.6;
    BASE_FIRE_RATE[cur] = 0.02;
    BASE_SHOT_SPEED[cur] = 20;
    BASE_PROJ_LIFESPAN[cur] = 0.012;
    BASE_AMMO_PER_PURCHASE[cur] = 2000;
    BASE_AMMO_COST[cur] = 10;
    BASE_SPREAD[cur] = 0.00;
    BASE_SPELL_COST[cur] = 1;
    BASE_SPELL_COOLDOWN[cur] = 2;
    //
    cur = 202;
    BASE_NAME[cur] = "AWBP";
    BASE_DESC[cur] = "Stands for Automated Water Balloon Popper. Why this was \"invented\" and manufactured in the hopes of making a profit is a mystery. The producing company's CFO is probably your economics professor. Shoots 60 projectiles.";
    BASE_COST[cur] = 95;
    BASE_PROJ_TYPE[cur] = 2;
    BASE_DAMAGE[cur] = 0.9;
    BASE_FIRE_RATE[cur] = 1.7;
    BASE_SHOT_SPEED[cur] = 22;
    BASE_PROJ_LIFESPAN[cur] = 0.2;
    BASE_AMMO_PER_PURCHASE[cur] = 30;
    BASE_AMMO_COST[cur] = 30;
    BASE_SPREAD[cur] = PI;
    BASE_PROJ_PER_SHOT[cur] = 100;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_paintball_gun.wav");
    //
    cur = 203;
    BASE_NAME[cur] = "English Textbook";
    BASE_DESC[cur] = "The deadliest weapon is ironically the one that passes a security check. Read from it and watch all in proximity suffer! As with all English textbooks, the marginal monetary of reading from it is $0, not including medical expenses for mental damage. Note: you suffer too from reading this.";
    BASE_COST[cur] = 110;
    BASE_DAMAGE[cur] = 0.8;
    BASE_FIRE_RATE[cur] = 0.01;
    BASE_AMMO_PER_PURCHASE[cur] = INF;
    BASE_AMMO_COST[cur] = 0;
    BASE_FIRE_FUNC[cur] = [](GameState& game_state, const std::shared_ptr<Unit>& unit, const Weapon &weapon) -> void
    {
        for(auto &i: game_state.game_map.units)
        {
            if(areEnemies(unit->affiliation, i->affiliation))
            {
                double dmg = weapon.getProjDamage(unit.get());
                dmg = dmg / (square(unit->dist(i.get())) + 1) - 0.1 * dmg;
                if(dmg >= 0)
                    Unit::dealDamage(unit.get(), i.get(), dmg);
            }
        }
        Unit::dealDamage(nullptr, unit.get(), 0.05);
    };
    //
    cur = 400;
    BASE_NAME[cur] = "Generic shotgun";
    BASE_DESC[cur] = "Shoots four bullets. You can't get a more generic video game shotgun than that.";
    BASE_COST[cur] = 100;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 5;
    BASE_FIRE_RATE[cur] = 1.31;
    BASE_SHOT_SPEED[cur] = 11.5;
    BASE_PROJ_LIFESPAN[cur] = 0.3;
    BASE_AMMO_PER_PURCHASE[cur] = 20;
    BASE_AMMO_COST[cur] = 25;
    BASE_SPREAD[cur] = 0.45;
    BASE_PROJ_PER_SHOT[cur] = 4;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_generic_shotgun.wav");
    //
    cur = 401;
    BASE_NAME[cur] = "Hunting Rifle";
    BASE_DESC[cur] = "Looks suspiciously familiar, as if someone just repainted a different weapon and relabeled it.";
    BASE_COST[cur] = 105;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 31;
    BASE_FIRE_RATE[cur] = 2.16;
    BASE_SHOT_SPEED[cur] = 19.6;
    BASE_PROJ_LIFESPAN[cur] = 0.68;
    BASE_AMMO_PER_PURCHASE[cur] = 20;
    BASE_AMMO_COST[cur] = 20;
    BASE_SPREAD[cur] = 0.122;
    BASE_SPELL_COST[cur] = 10;
    BASE_SPELL_COOLDOWN[cur] = 9;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_hunting_rifle.wav");
    //
    cur = 402;
    BASE_NAME[cur] = "Flamethrower";
    BASE_DESC[cur] = "Actually shoots red bullets, not flames.";
    BASE_COST[cur] = 170;
    BASE_PROJ_TYPE[cur] = 3;
    BASE_DAMAGE[cur] = 3;
    BASE_FIRE_RATE[cur] = 0.08;
    BASE_SHOT_SPEED[cur] = 9.7;
    BASE_PROJ_LIFESPAN[cur] = 0.22;
    BASE_AMMO_PER_PURCHASE[cur] = 400;
    BASE_AMMO_COST[cur] = 15;
    BASE_SPREAD[cur] = 0.85;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_flamethrower.wav", 0.7);
    //
    cur = 403;
    BASE_NAME[cur] = "Ping Pong Machine";
    BASE_DESC[cur] = "It's a deluxe version, much more expensive than your typical automated ping pong serving machine, but true to its price "
        "tag, it packs quite a punch with the right type of ping pong ball. It's also very accurate. Disclaimer: Do not use without "
        "an expensive health insurance plan.";
    BASE_COST[cur] = 110;
    BASE_PROJ_TYPE[cur] = 5;
    BASE_DAMAGE[cur] = 4.5;
    BASE_FIRE_RATE[cur] = 0.36;
    BASE_SHOT_SPEED[cur] = 10;
    BASE_PROJ_LIFESPAN[cur] = 0.3;
    BASE_AMMO_PER_PURCHASE[cur] = 70;
    BASE_AMMO_COST[cur] = 5;
    BASE_SPREAD[cur] = 0.05;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_ping_pong_machine.wav");
    //
    cur = 600;
    BASE_NAME[cur] = "Magnet Shooter";
    BASE_DESC[cur] = "It shoots little orbiting magnets that kind of obey Coulomb's Law! Unlike most magnets, you'll be very screwed even if you swallow just one of these. Magnets orbit at 15 tiles/s.";
    BASE_COST[cur] = 141;
    BASE_PROJ_TYPE[cur] = 15;
    BASE_DAMAGE[cur] = 9.9;
    BASE_FIRE_RATE[cur] = 0.103;
    BASE_SHOT_SPEED[cur] = 15;
    BASE_PROJ_LIFESPAN[cur] = 2;
    BASE_AMMO_PER_PURCHASE[cur] = 100;
    BASE_AMMO_COST[cur] = 41;
    BASE_SPREAD[cur] = 0.24;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_ping_pong_machine.wav");
    BASE_PROJ_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
    {
        double ang = std::atan2(proj.getY() - unit->getY(), proj.getX() - unit->getX());
        double dist = square(unit->shape->centerDist(proj.shape.get()));
        proj.dX -= TICK_SPEED * 120 * std::cos(ang) / (dist + 0.1);
        proj.dY -= TICK_SPEED * 120 * std::sin(ang) / (dist + 0.1);
        proj.setX(proj.getX() + proj.dX * TICK_SPEED);
        proj.setY(proj.getY() + proj.dY * TICK_SPEED);
    };
    //
    cur = 601;
    BASE_NAME[cur] = "Flashlight";
    BASE_DESC[cur] = "It's definitely more like a laser than a commercial flashlight. Note: \"IMPROPER USE MAY CAUSE SERIOUS INJURY OR DEATH.\"";
    BASE_COST[cur] = 210;
    BASE_PROJ_TYPE[cur] = 14;
    BASE_DAMAGE[cur] = 0.1;
    BASE_FIRE_RATE[cur] = 0.0025;
    BASE_SHOT_SPEED[cur] = 40;
    BASE_PROJ_LIFESPAN[cur] = INF;
    BASE_AMMO_PER_PURCHASE[cur] = 3600;
    BASE_AMMO_COST[cur] = 45;
    BASE_SPREAD[cur] = 0;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_laser.wav", 0.1);
    //
    cur = 800;
    BASE_NAME[cur] = "Big Stick";
    BASE_DESC[cur] = "Pretty expensive for a stick. Also, for some reason it still needs ammo to operate and seems more like a gun that shoots invisible bullets than a stick. Deceptive advertising much? Special attack: gain Big Stick Energy for 2 seconds, which makes you move faster and deal more damage with the Big Stick.";
    BASE_COST[cur] = 135;
    BASE_PROJ_TYPE[cur] = 6;
    BASE_DAMAGE[cur] = 0.95;
    BASE_FIRE_RATE[cur] = 0.02;
    BASE_SHOT_SPEED[cur] = 20;
    BASE_PROJ_LIFESPAN[cur] = 0.015;
    BASE_AMMO_PER_PURCHASE[cur] = 2000;
    BASE_AMMO_COST[cur] = 30;
    BASE_SPREAD[cur] = 0.00;
    BASE_SPELL_COST[cur] = 5;
    BASE_SPELL_COOLDOWN[cur] = 3;
    //
    cur = 801;
    BASE_NAME[cur] = "Water Gun";
    BASE_DESC[cur] = "Its full advertised name is Ultimate Water Gun Pro (TM) Gold Platinum Plus+ Diamond Deluxe Limited Edition. "
        "Despite having space to put all the adjectives on the box label, the manufacturers conveniently purportedly lacked enough space to warn "
        "customers that it is not actually a water gun in the traditional sense of the word.";
    BASE_COST[cur] = 160;
    BASE_PROJ_TYPE[cur] = 2;
    BASE_DAMAGE[cur] = 1.7;
    BASE_FIRE_RATE[cur] = 0.06;
    BASE_SHOT_SPEED[cur] = 8.3;
    BASE_PROJ_LIFESPAN[cur] = 0.5;
    BASE_AMMO_PER_PURCHASE[cur] = 500;
    BASE_AMMO_COST[cur] = 15;
    BASE_SPREAD[cur] = 0.7;
    BASE_SPELL_COOLDOWN[cur] = 10;
    BASE_SPELL_COST[cur] = 14;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_water_gun.wav", 0.4);
    //
    cur = 802;
    BASE_NAME[cur] = "Shotgun (TM)";
    BASE_DESC[cur] = "You are viewing a Shotgun (TM), which is superior to a shotgun.";
    BASE_COST[cur] = 260;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 8.5;
    BASE_FIRE_RATE[cur] = 1.1;
    BASE_SHOT_SPEED[cur] = 13.5;
    BASE_PROJ_LIFESPAN[cur] = 0.25;
    BASE_AMMO_PER_PURCHASE[cur] = 15;
    BASE_AMMO_COST[cur] = 25;
    BASE_SPREAD[cur] = 0.55;
    BASE_PROJ_PER_SHOT[cur] = 6;
    BASE_FIRERATE_IRREGULARITY[cur] = 0.6;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_shotgun.wav", 0.4);
    //
    cur = 803;
    BASE_NAME[cur] = "P89";
    BASE_DESC[cur] = "The logical deduction here is P2000 < P250 < P90 < P89. In other words, don't buy you ****ing retarded braindead autistic "       "useless silver trash, you're legit worse than a bot. Vote initiated by CalmFriendlyNicePlayer27. Kick player: (you)?";
    BASE_COST[cur] = 250;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 5.6;
    BASE_FIRE_RATE[cur] = 0.135;
    BASE_SHOT_SPEED[cur] = 12.5;
    BASE_PROJ_LIFESPAN[cur] = 0.55;
    BASE_AMMO_PER_PURCHASE[cur] = 200;
    BASE_AMMO_COST[cur] = 25;
    BASE_SPREAD[cur] = 0.25;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_p89.mp3", 0.4);
    //
    cur = 804;
    BASE_NAME[cur] = "Gamma Ray Gun";
    BASE_DESC[cur] = "Shoots waves, which can pass through infinitely many enemies. Also shoots particles, which look like normal projectiles and die after some time. Also shoots wave-particles.";
    BASE_COST[cur] = 273;
    BASE_PROJ_TYPE[cur] = 2;
    BASE_DAMAGE[cur] = 1.5;
    BASE_FIRE_RATE[cur] = 0.45;
    BASE_SHOT_SPEED[cur] = 11;
    BASE_PROJ_LIFESPAN[cur] = 0.35;
    BASE_AMMO_PER_PURCHASE[cur] = 40;
    BASE_AMMO_COST[cur] = 50;
    BASE_SPREAD[cur] = PI / 3;
    BASE_PROJ_PER_SHOT[cur] = 30;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_gamma_ray_gun.wav");
    //
    cur = 1000;
    BASE_NAME[cur] = "Bigger Stick";
    BASE_DESC[cur] = "Like a big stick, but even bigger. Also requires ammo to operate. Special attack: Gain Bigger Stick Energy for 2s, which negates damage from standing on X tiles.";
    BASE_COST[cur] = 200;
    BASE_PROJ_TYPE[cur] = 6;
    BASE_DAMAGE[cur] = 1.7;
    BASE_FIRE_RATE[cur] = 0.02;
    BASE_SHOT_SPEED[cur] = 20;
    BASE_PROJ_LIFESPAN[cur] = 0.017;
    BASE_AMMO_PER_PURCHASE[cur] = 2000;
    BASE_AMMO_COST[cur] = 40;
    BASE_SPREAD[cur] = 0.00;
    BASE_SPELL_COST[cur] = 5;
    BASE_SPELL_COOLDOWN[cur] = 5;
    //
    cur = 1001;
    BASE_NAME[cur] = "Sawed-on Shotgun";
    BASE_DESC[cur] = "You've probably heard of a sawed-off shotgun, but have you ever heard of a sawed-on shotgun? Well it can't be too different, because it's 67% similar to a sawed off shotgun name-wise.";
    BASE_COST[cur] = 375;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 7.5;
    BASE_FIRE_RATE[cur] = 1.15;
    BASE_SHOT_SPEED[cur] = 16;
    BASE_PROJ_LIFESPAN[cur] = 0.85;
    BASE_AMMO_PER_PURCHASE[cur] = 20;
    BASE_AMMO_COST[cur] = 35;
    BASE_SPREAD[cur] = 0.08;
    BASE_PROJ_PER_SHOT[cur] = 5;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_sawedon.wav", 0.6);
    //
    cur = 1002;
    BASE_NAME[cur] = "P0";
    BASE_DESC[cur] = "P2000 < P250 < P90 < P89 < P0";
    BASE_COST[cur] = 409;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 9.5;
    BASE_FIRE_RATE[cur] = 0.31;
    BASE_SHOT_SPEED[cur] = 12.6;
    BASE_PROJ_LIFESPAN[cur] = 0.5;
    BASE_AMMO_PER_PURCHASE[cur] = 85;
    BASE_AMMO_COST[cur] = 50;
    BASE_SPREAD[cur] = 0.18;
    BASE_PROJ_PER_SHOT[cur] = 2;
    //BASE_SPELL_COST[cur] = 16;
    //BASE_SPELL_COOLDOWN[cur] = 11;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_pi.wav", 0.35);
    //
    cur = 1004;
    BASE_NAME[cur] = "1 GPa Sprinkler Head";
    BASE_DESC[cur] = "Definitely not a traditional weapon (and not your typical sprinkler, or at least probably not a legal sprinkler), but it has its uses. And no, you can't use water gun ammo for this because this uses \"special\" water.";
    BASE_COST[cur] = 270;
    BASE_PROJ_TYPE[cur] = 2;
    BASE_DAMAGE[cur] = 5;
    BASE_FIRE_RATE[cur] = 0.03;
    BASE_SHOT_SPEED[cur] = 15;
    BASE_PROJ_LIFESPAN[cur] = 0.35;
    BASE_AMMO_PER_PURCHASE[cur] = 700;
    BASE_AMMO_COST[cur] = 30;
    BASE_SPREAD[cur] = INF;
    //BASE_SPELL_COST[cur] = 16;
    //BASE_SPELL_COOLDOWN[cur] = 11;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_water_gun.wav", 0.35);
    //
    cur = 1200;
    BASE_NAME[cur] = "Biggest Stick";
    BASE_DESC[cur] = "The biggest stick ever! Except it's definitely not a stick. Special attack: All enemy shapes in the room tremble at your biggest stick energy aura and lose 10% of their max health.";
    BASE_COST[cur] = 250;
    BASE_PROJ_TYPE[cur] = 6;
    BASE_DAMAGE[cur] = 2.7;
    BASE_FIRE_RATE[cur] = 0.02;
    BASE_SHOT_SPEED[cur] = 20;
    BASE_PROJ_LIFESPAN[cur] = 0.019;
    BASE_AMMO_PER_PURCHASE[cur] = 2000;
    BASE_AMMO_COST[cur] = 50;
    BASE_SPREAD[cur] = 0.00;
    BASE_SPELL_COST[cur] = 15;
    BASE_SPELL_COOLDOWN[cur] = 20;
    //
    cur = 1201;
    BASE_NAME[cur] = "Pi";
    BASE_DESC[cur] = "P2000 < P250 < P90 < P89 < P0, but ever heard of a Pi? For what it's worth, if you take four Pis, multiply them and, and divide by P three times, you get a P1. Of course, it's up to you to figure out how gun arithmetic works.";
    BASE_COST[cur] = 350;
    BASE_PROJ_TYPE[cur] = 12;
    BASE_DAMAGE[cur] = 7.5;
    BASE_FIRE_RATE[cur] = 0.35;
    BASE_SHOT_SPEED[cur] = 12;
    BASE_PROJ_LIFESPAN[cur] = 0.5;
    BASE_AMMO_PER_PURCHASE[cur] = 80;
    BASE_AMMO_COST[cur] = 70;
    BASE_SPREAD[cur] = 0.21;
    BASE_PROJ_PER_SHOT[cur] = 3;
    BASE_SPELL_COST[cur] = 16;
    BASE_SPELL_COOLDOWN[cur] = 11;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_pi.wav", 0.35);
    //
    cur = 1202;
    BASE_NAME[cur] = "S^3";
    BASE_DESC[cur] = "Short for Shape Seeking Sniper. It’s automatic, a sniper, and homing, sure to provide a quick and not-too-painful death. Definitely sounds super OP.";
    BASE_COST[cur] = 300;
    BASE_PROJ_TYPE[cur] = 12;
    BASE_DAMAGE[cur] = 21;
    BASE_FIRE_RATE[cur] = 0.25;
    BASE_SHOT_SPEED[cur] = 15;
    BASE_PROJ_LIFESPAN[cur] = 1.5;
    BASE_AMMO_PER_PURCHASE[cur] = 50;
    BASE_AMMO_COST[cur] = 50;
    BASE_SPREAD[cur] = 0.02;
    BASE_SPELL_COST[cur] = 16;
    BASE_SPELL_COOLDOWN[cur] = 16;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_sss.wav", 0.2);
    BASE_PROJ_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
    {
        double ms = std::hypot(proj.dX, proj.dY);
        Unit *closest;
        double closestd = 1e9;
        for(auto &i: game_state.game_map.units)
        {
            if(!areEnemies(unit->affiliation, i->affiliation))
                continue;
            double d = i->shape->centerDist(proj.shape.get());
            if(d < closestd)
            {
                closestd = d;
                closest = i.get();
            }
        }
        if(closestd != 1e9)
        {
            closestd += 0.01;
            closestd = square(closestd);
            double xdiff = closest->getX() - proj.getX();
            double ydiff = closest->getY() - proj.getY();
            proj.dX = (proj.dX + 100 * xdiff / closestd * TICK_SPEED);
            proj.dY = (proj.dY + 100 * ydiff / closestd * TICK_SPEED);
            normToHypot(proj.dX, proj.dY, ms);
        }
        proj.setX(proj.getX() + proj.dX * TICK_SPEED);
        proj.setY(proj.getY() + proj.dY * TICK_SPEED);
    };
    //
    cur = 1203;
    BASE_NAME[cur] = "Unscientific Railgun";
    BASE_DESC[cur] = "Level 6 tech means infinite range, infinite piercing, and travelling through walls!";
    BASE_COST[cur] = 370;
    BASE_PROJ_TYPE[cur] = 12;
    BASE_DAMAGE[cur] = 40;
    BASE_FIRE_RATE[cur] = 0.75;
    BASE_SHOT_SPEED[cur] = 28;
    BASE_PROJ_LIFESPAN[cur] = INF;
    BASE_AMMO_PER_PURCHASE[cur] = 35;
    BASE_AMMO_COST[cur] = 40;
    BASE_SPREAD[cur] = 0.04;
    BASE_PIERCE_COUNT[cur] = INF;
    BASE_IS_SPECTRAL[cur] = true;
    BASE_SPELL_COST[cur] = 24;
    BASE_SPELL_COOLDOWN[cur] = 10;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_railgun.wav", 0.7);
    //
    cur = 1204;
    BASE_NAME[cur] = "Blowgun";
    BASE_DESC[cur] = "The most technologically advanced weapon out there, perfect for the later levels of roguelike games and sure to kill enemies in one shot.";
    BASE_COST[cur] = 310;
    BASE_PROJ_TYPE[cur] = 1;
    BASE_DAMAGE[cur] = 100;
    BASE_FIRE_RATE[cur] = 2.45;
    BASE_SHOT_SPEED[cur] = 24;
    BASE_PROJ_LIFESPAN[cur] = INF;
    BASE_AMMO_PER_PURCHASE[cur] = 20;
    BASE_AMMO_COST[cur] = 45;
    BASE_SPREAD[cur] = 0.01;
    //BASE_SPELL_COST[cur] = 24;
    //BASE_SPELL_COOLDOWN[cur] = 10;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_blowgun.wav");
    //
    cur = 1205;
    BASE_NAME[cur] = "1 ZW laser";
    BASE_DESC[cur] = "The \"Z\" in front of the \"W\" means it's a very big number. That's all you need to know.";
    BASE_COST[cur] = 434;
    BASE_PROJ_TYPE[cur] = 13;
    BASE_DAMAGE[cur] = 0.7;
    BASE_FIRE_RATE[cur] = 0.0025;
    BASE_SHOT_SPEED[cur] = 40;
    BASE_PROJ_LIFESPAN[cur] = INF;
    BASE_AMMO_PER_PURCHASE[cur] = 1500;
    BASE_AMMO_COST[cur] = 160;
    BASE_SPREAD[cur] = 0;
    BASE_PIERCE_COUNT[cur] = INF;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_laser.wav", 0.1);
    //
    cur = 1206;
    BASE_NAME[cur] = "Mini Quasar Gun";
    BASE_DESC[cur] = "It shoots mini quasars. VERY mini quasars. Like, we're talking on the scale of a trillionth of a trillionth of a trillionth of the usual size. Again, better read the fine print. I definitely would've expected a quasar to appear as soon as I hit the trigger and would've been sorely disappointed.";
    BASE_COST[cur] = 400;
    BASE_PROJ_TYPE[cur] = 13;
    BASE_DAMAGE[cur] = 30;
    BASE_FIRE_RATE[cur] = 0.15;
    BASE_SHOT_SPEED[cur] = 10;
    BASE_PROJ_LIFESPAN[cur] = 2;
    BASE_AMMO_PER_PURCHASE[cur] = 65;
    BASE_AMMO_COST[cur] = 92;
    BASE_SPREAD[cur] = 0.05;
    //BASE_SPELL_COST[cur] = 16;
    //BASE_SPELL_COOLDOWN[cur] = 16;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_laser.wav", 0.35);
    BASE_PROJ_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
    {
        //parametric movement is unaffected by projectile speed bonuses
        double v2 = 1000 * std::fmod(proj.rand_var, 0.001);
        double dX = (4 * proj.rand_var + 3) * std::sin(v2 + (1.5 * proj.rand_var + 3) * proj.age);
        double dY = (4 * v2 + 3) * std::cos(proj.rand_var + (1.2 * v2 + 5) * proj.age);
        proj.setX(proj.getX() + (dX + proj.dX) * TICK_SPEED);
        proj.setY(proj.getY() + (dY + proj.dY) * TICK_SPEED);
    };
    //
    cur = 1207;
    BASE_NAME[cur] = "0x812FBCC5";
    BASE_DESC[cur] = "This is a bit broken right now. Trying to fix. -kworker.";
    BASE_COST[cur] = 450;
    BASE_PROJ_TYPE[cur] = 13;
    BASE_DAMAGE[cur] = 3;
    BASE_FIRE_RATE[cur] = 0.7;
    BASE_SHOT_SPEED[cur] = 15;
    BASE_PROJ_LIFESPAN[cur] = 0.4;
    BASE_AMMO_PER_PURCHASE[cur] = 8;
    BASE_AMMO_COST[cur] = 100;
    BASE_SPREAD[cur] = PI/3;
    BASE_PROJ_PER_SHOT[cur] = 70;
    //BASE_SPELL_COST[cur] = 16;
    //BASE_SPELL_COOLDOWN[cur] = 16;
    FIRE_AUDIO[cur] = audio_chunk("data/audio/shot_laser.wav", 0.35);
    BASE_PROJ_MOVEMENT_FUNC[cur] = [](GameState &game_state, const std::shared_ptr<Unit> &unit, Projectile &proj)-> void
    {
        //parametric movement is unaffected by projectile speed bonuses
        double v2 = 1000 * std::fmod(proj.rand_var, 0.01);
        double dX = (2 * proj.rand_var + 1) * std::sin(v2 + (1.5 * proj.rand_var + 3) * proj.age);
        double dY = (2 * v2 + 1) * std::cos(proj.rand_var + (1.2 * v2 + 5) * proj.age);
        proj.setX(proj.getX() + (dX + proj.dX) * TICK_SPEED);
        proj.setY(proj.getY() + (dY + proj.dY) * TICK_SPEED);
    };
    //
    for(int i=0; i<NUM_WEAPONS; i++) //by default, weapons come with one purchase of ammo
        if(BASE_AMMO[i] == NOT_SET)
            BASE_AMMO[i] = BASE_AMMO_PER_PURCHASE[i];
    for(int i=0; i<NUM_WEAPONS; i++)
        if(HAS_SPRITE[i] && BASE_NAME[i] != "")
            sprites[i] = loadTexture(("data/weapon/" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(int i=0; i<NUM_WEAPONS; i++)
        if(HAS_SPRITE[i] && BASE_NAME[i] != "")
            wield_sprites[i] = loadTexture(("data/weapon/" + to_str(i) + "_wield.png").c_str(), 255, 255, 255);
}
void Projectile::init()
{
    for(auto &i: GEN_CUSTOM_TEXTURE) //no custom texture by default
        i = nullptr;
    //bullet
    int cur = 0;
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.08;
    //arrow
    cur = 1;
    BASE_SHAPE[cur] = "square";
    BASE_SIZE1[cur] = 0.32;
    //colorful bullet
    cur = 2;
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.08;
    //red bullet (mainly for AI)
    cur = 3;
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.08;
    //green bullet (mainly for AI)
    cur = 4;
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.08;
    //yellow bullet (mainly for AI)
    cur = 5;
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.08;
    //NOTHING!!!! (for things like Big Stick)
    cur = 6;
    GEN_CUSTOM_TEXTURE[cur] = []() -> SDL_Texture*
    {
        static SDL_Texture *t = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 0, 0);
        return t;
    };
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.10;
    //purple bullet (mainly for AI)
    cur = 7;
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.08;
    //orange bullet (mainly for AI)
    cur = 8;
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.08;
    //randomly colored circle
    cur = 9;
    BASE_SHAPE[cur] = "circle";
    BASE_SIZE1[cur] = 0.08;
    GEN_CUSTOM_TEXTURE[cur] = []() -> SDL_Texture*
    {
        static static_array<SDL_Texture*> textures(20);
        bool alreadyInit = false;
        if(!alreadyInit)
        {
            alreadyInit = true;
            for(int i=0; i<(int)textures.size(); i++)
            {
                SDL_Surface *s = SDL_CreateRGBSurface(0, 17, 17, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
                for(int i=0; i<17; i++)
                {
                    for(int j=0; j<17; j++)
                    {
                        Uint32 *pixel = (Uint32*)((uint8_t*)s->pixels + i * s->pitch + j * 4);
                        if(square(i - 8) + square(j - 8) < square(8))
                            *pixel = (((Uint32)(randz(0, (1<<24)-1))) << 8) | 0xff;
                        else *pixel = 0xffffffffu;
                    }
                }
                SDL_SetColorKey(s, SDL_TRUE, SDL_MapRGB(s->format, 255, 255, 255));
                SDL_Texture *t = SDL_CreateTextureFromSurface(getRenderer(), s);
                SDL_FreeSurface(s);
                textures[i] = t;
            }
        }
        return textures.get(randz(0, textures.size()-1));
    };
    //"0" digit (for kworker)
    cur = 10;
    BASE_SHAPE[cur] = "polygon";
    BASE_VERTICES[cur] = static_array<Point>(8);
    BASE_VERTICES[cur][0] = Point(-0.06, -0.21);
    BASE_VERTICES[cur][1] = Point(-0.15, -0.11);
    BASE_VERTICES[cur][2] = Point(-0.15, 0.16);
    BASE_VERTICES[cur][3] = Point(-0.06, 0.26);
    BASE_VERTICES[cur][4] = Point(0.07, 0.26);
    BASE_VERTICES[cur][5] = Point(0.16, 0.16);
    BASE_VERTICES[cur][6] = Point(0.16, -0.11);
    BASE_VERTICES[cur][7] = Point(0.07, -0.21);
    GEN_CUSTOM_TEXTURE[cur] = []() -> SDL_Texture*
    {
        static SDL_Texture *t = createText("0", 54, 255, 255, 254, 255);
        return t;
    };
    //"1" digit (for kworker)
    cur = 11;
    BASE_SHAPE[cur] = "polygon";
    BASE_VERTICES[cur] = static_array<Point>(7);
    BASE_VERTICES[cur][0] = Point(-0.14, -0.15);
    BASE_VERTICES[cur][1] = Point(0.03, -0.21);
    BASE_VERTICES[cur][2] = Point(0.06, -0.21);
    BASE_VERTICES[cur][3] = Point(0.06, 0.25);
    BASE_VERTICES[cur][4] = Point(-0.03, 0.25);
    BASE_VERTICES[cur][5] = Point(-0.03, -0.12);
    BASE_VERTICES[cur][6] = Point(-0.13, -0.07);
    GEN_CUSTOM_TEXTURE[cur] = []() -> SDL_Texture*
    {
        static SDL_Texture *t = createText("1", 54, 255, 255, 254, 255);
        return t;
    };
    //white square
    cur = 12;
    BASE_SHAPE[cur] = "square";
    BASE_SIZE1[cur] = 0.16;
    //red square
    cur = 13;
    BASE_SHAPE[cur] = "square";
    BASE_SIZE1[cur] = 0.16;
    //yellow square
    cur = 14;
    BASE_SHAPE[cur] = "square";
    BASE_SIZE1[cur] = 0.16;
    //gray square
    cur = 15;
    BASE_SHAPE[cur] = "square";
    BASE_SIZE1[cur] = 0.16;
    //randomly colored square
    cur = 16;
    BASE_SHAPE[cur] = "square";
    BASE_SIZE1[cur] = 0.16;
    GEN_CUSTOM_TEXTURE[cur] = []() -> SDL_Texture*
    {
        static static_array<SDL_Texture*> textures(20);
        bool alreadyInit = false;
        if(!alreadyInit)
        {
            alreadyInit = true;
            for(int i=0; i<(int)textures.size(); i++)
            {
                SDL_Surface *s = SDL_CreateRGBSurface(0, 16, 16, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
                for(int i=0; i<16; i++)
                {
                    for(int j=0; j<16; j++)
                    {
                        Uint32 *pixel = (Uint32*)((uint8_t*)s->pixels + i * s->pitch + j * 4);
                        *pixel = (((Uint32)(randz(0, (1<<24)-1))) << 8) | 0xff;
                    }
                }
                SDL_SetColorKey(s, SDL_TRUE, SDL_MapRGB(s->format, 255, 255, 255));
                SDL_Texture *t = SDL_CreateTextureFromSurface(getRenderer(), s);
                SDL_FreeSurface(s);
                textures[i] = t;
            }
        }
        return textures.get(randz(0, textures.size()-1));
    };
    //
    for(int i=0; i<NUM_PROJECTILES; i++)
        if(GEN_CUSTOM_TEXTURE[i] == nullptr)
            sprites[i] = loadTexture(("data/projectile/" + to_str(i) + ".png").c_str(), 255, 255, 255);
}
void Item::init()
{
    for(auto &i: IS_SHOWN) //items are shown by default
        i = true;
    //Spiked vest
    int cur = 0;
    BASE_NAME[cur] = "Spiked Vest";
    BASE_COST[cur] = 80;
    BASE_DESC[cur] = "A cheap vest that someone stuck a lot of spikes on and rebranded.";
    //
    cur = 1;
    BASE_NAME[cur] = "Bullseye";
    BASE_COST[cur] = 65;
    BASE_DESC[cur] = "Expensive for a bullseye tattoo. Worth?";
    //
    cur = 2;
    BASE_NAME[cur] = "Dirty Serrated Shirt";
    BASE_COST[cur] = 50;
    BASE_DESC[cur] = "A dirty shirt that is somehow serrated. Probably made in China.";
    //
    cur = 3;
    BASE_NAME[cur] = "Blue Boots";
    BASE_COST[cur] = 75;
    BASE_DESC[cur] = "Boots with blue coloring.";
    //
    cur = 4;
    BASE_NAME[cur] = "Red Boots";
    BASE_COST[cur] = 75;
    BASE_DESC[cur] = "Boots with red coloring, which gives you sadistic urges.";
    //
    cur = 5;
    BASE_NAME[cur] = "Sadist's Machete";
    BASE_COST[cur] = 85;
    BASE_DESC[cur] = "This toy seems like lots of fun! That is, you'll have fun if you don't use it for its likely intended purpose.";
    //
    cur = 6;
    BASE_NAME[cur] = "Green Boots";
    BASE_COST[cur] = 85;
    BASE_DESC[cur] = "Like any other boots, but colored green.";
    //
    cur = 7;
    BASE_NAME[cur] = "Amulet of Air";
    BASE_COST[cur] = 50;
    BASE_DESC[cur] = "An artifact claimed to have supernatural powers. Probably a scam.";
    //
    cur = 8;
    BASE_NAME[cur] = "Dalton's Atom";
    BASE_COST[cur] = 45;
    BASE_DESC[cur] = "Isn't this just a liiiitle outdated?";
    //
    cur = 9;
    BASE_NAME[cur] = "Toothbrush";
    BASE_COST[cur] = 77;
    BASE_DESC[cur] = "It makes you excited because it reminds you of all the anime you plan to watch after finishing this floor.";
    //
    cur = 10;
    BASE_NAME[cur] = "Red Gloves";
    BASE_COST[cur] = 45;
    BASE_DESC[cur] = "Your hands feel warm and happy because they are coated in fuzzy red material.";
    //
    cur = 11;
    BASE_NAME[cur] = "Red Pillow";
    BASE_COST[cur] = 54;
    BASE_DESC[cur] = "It' so fluffy! But it's also kind of overpriced. Is it Black Friday perchance?";
    //
    cur = 12;
    BASE_NAME[cur] = "Mysterious Vial";
    BASE_COST[cur] = 87;
    BASE_DESC[cur] = "Not FDA approved.";
    //
    cur = 13;
    BASE_NAME[cur] = "Mysterious Phial";
    BASE_COST[cur] = 87;
    BASE_DESC[cur] = "Like a vial, but a phial. Rejected by the FDA due to the strange spelling.";
    //
    cur = 14;
    BASE_NAME[cur] = "The Art of Sadism";
    BASE_COST[cur] = 71;
    BASE_DESC[cur] = "0/15 copies available at your local library. Fortunately, it's the #1 selling book on Amazon so it's easy to find.";
    //
    cur = 15;
    BASE_NAME[cur] = "Pistol Enhancement I";
    BASE_COST[cur] = 96;
    BASE_DESC[cur] = "What's this?";
    //
    cur = 400;
    BASE_NAME[cur] = "Blue Tome";
    BASE_COST[cur] = 66;
    BASE_DESC[cur] = "Merely having it by your side makes you feel scholarly.";
    //
    cur = 401;
    BASE_NAME[cur] = "Pistol Enhancement II";
    BASE_COST[cur] = 65;
    BASE_DESC[cur] = "It somehow enhances the Poofy Pistol, but how it does so seems unclear.";
    //
    cur = 402;
    BASE_NAME[cur] = "Philosopher's Book";
    BASE_COST[cur] = 67;
    BASE_DESC[cur] = "10/10 would philosophize about whether buying this book is a good choice or not.";
    //
    cur = 403;
    BASE_NAME[cur] = "Purple Boots";
    BASE_COST[cur] = 155;
    BASE_DESC[cur] = "Purple makes your feet feel satisfied. Your projectiles also feel satisfied.";
    //
    cur = 404;
    BASE_NAME[cur] = "The Sadist Manifesto";
    BASE_COST[cur] = 180;
    BASE_DESC[cur] = "Written by \"A Sadist.\"";
    //
    cur = 405;
    BASE_NAME[cur] = "Projektyles";
    BASE_COST[cur] = 150;
    BASE_DESC[cur] = "A foreign technical book that was evidently translated by a computer instead of an actual person.";
    //
    cur = 406;
    BASE_NAME[cur] = "Illegal Fur Coat";
    BASE_COST[cur] = 120;
    BASE_DESC[cur] = "Disclaimer: not legal to buy or sell.";
    //
    cur = 407;
    BASE_NAME[cur] = "Professional Protester";
    BASE_COST[cur] = 150;
    BASE_DESC[cur] = "Hire a professional protester to harass the item and services shop! Doesn't affect the weapons shop.";
    IS_SHOWN[cur] = false;
    //
    cur = 408;
    BASE_NAME[cur] = "Waifu Body Pillow";
    BASE_COST[cur] = 200;
    BASE_DESC[cur] = "Definitely the most underrated commodity available. Unfortunately, the art on this one is crappy.";
    //
    cur = 600;
    BASE_NAME[cur] = "Angry Gamer's Manifesto";
    BASE_COST[cur] = 138;
    BASE_DESC[cur] = "To be written by you after playing this game. One of your primary complaints is the lack of the word \"The\" before the title of this book, which the developer purposefully excluded to prevent text overflow. You can spend some money to publish this book.";
    //
    cur = 601;
    BASE_NAME[cur] = "Physics for Bots";
    BASE_COST[cur] = 386;
    BASE_DESC[cur] = "There's decelerated physics and remedial physics, but have you ever heard of Physics for Bots?";
    //
    cur = 602;
    BASE_NAME[cur] = "Floppy Disk";
    BASE_COST[cur] = 170;
    BASE_DESC[cur] = "Hmm... do they even sell floppy disk readers these days?";
    //
    cur = 603;
    BASE_NAME[cur] = "Book of Words";
    BASE_COST[cur] = 170;
    BASE_DESC[cur] = "10/10 the most descriptive title for a book you have seen.";
    //
    cur = 604;
    BASE_NAME[cur] = "Heart Pendant";
    BASE_COST[cur] = 380;
    BASE_DESC[cur] = "It has an eerie inscription that reads: \"The only thing worth loving is hate.\"";
    //
    cur = 605;
    BASE_NAME[cur] = "S^4";
    BASE_COST[cur] = 380;
    BASE_DESC[cur] = "S^4 = Super Specialized Sniper Scope, the name for a toy to make bullets from sniper-like weapons count. The description on the packaging reads, \"One bullet to kill them all. One bullet to behead them. One bullet to shred their insides and spread them to paint the world red.\"";
    //
    cur = 606;
    BASE_NAME[cur] = "Silver Tongue";
    BASE_COST[cur] = 335;
    BASE_DESC[cur] = "It's literally a detached tongue that's been coated with a thick layer of silver.";
    //
    cur = 800;
    BASE_NAME[cur] = "Professional Rioter";
    BASE_COST[cur] = 190;
    BASE_DESC[cur] = "Like a protester, but also riots. This person specializes in harassing weapon shop owners.";
    IS_SHOWN[cur] = false;
    //
    cur = 801;
    BASE_NAME[cur] = "Attorney";
    BASE_COST[cur] = 230;
    BASE_DESC[cur] = "File a lawsuit and get a lot of money if your attorney is good!";
    IS_SHOWN[cur] = false;
    //
    cur = 802;
    BASE_NAME[cur] = "Pistol Enhancement III";
    BASE_COST[cur] = 100;
    BASE_DESC[cur] = "Probably not worth it.";
    //
    cur = 803;
    BASE_NAME[cur] = "Barber";
    BASE_COST[cur] = 130;
    BASE_DESC[cur] = "This is either going to go really well or really poorly...";
    IS_SHOWN[cur] = false;
    //
    cur = 804;
    BASE_NAME[cur] = "Neighbor'sw Hose";
    BASE_COST[cur] = 50;
    BASE_DESC[cur] = "Infinite free water! Well, free for you at least. Water, of course, refers to blue projectiles, which is what your "
            "eccentric neighbor's even more eccentric hose outputs.";
    //
    cur = 805;
    BASE_NAME[cur] = "Mahou Shoujo Tshirt";
    BASE_COST[cur] = 250;
    BASE_DESC[cur] = "It has protective magical properties against most shapes. The entropy of the Universe was not significantly changed "
            "in the making of this.";
    //
    cur = 806;
    BASE_NAME[cur] = "Aspirin";
    BASE_COST[cur] = 200;
    BASE_DESC[cur] = "Pain only hurts if you can feel it. This family-sized aspirin pill will probably last until you respawn.";
    //
    cur = 807;
    BASE_NAME[cur] = "Yellow Pill";
    BASE_COST[cur] = 300;
    BASE_DESC[cur] = "A pill solely marketed by advertising its unique color. The only text on the box says, \"We created the PERFECT pill for YOU. Created by our team of expert researchers specifically to be a unique color and marketed by our world-class marketing team, it is sure to satisfy you!\" Perhaps not surprisingly, the number of boxes of this medicine on this shelf at the local pharmacy has stayed constant for the past several weeks.";
    //
    cur = 808;
    BASE_NAME[cur] = "Shower Pass";
    BASE_COST[cur] = 190;
    BASE_DESC[cur] = "It's a pass that allows you to shower! It seems like you haven't been showering, or else this wouldn't have appeared in the shop.";
    //
    cur = 809;
    BASE_NAME[cur] = "Archery Lesson";
    BASE_COST[cur] = 140;
    BASE_DESC[cur] = "Become a true master of bows - CEO, Ph.D. in Medieval Archery, USU (Useless Skills University)";
    //
    cur = 810;
    BASE_NAME[cur] = "The Laissez Faire Manifesto";
    BASE_COST[cur] = 240;
    BASE_DESC[cur] = "You haven't heard about it? It was all the rage back then. Even the great Karl Marx himself time to publicly burn a copy after it declaimed itself to be \"more equal than its counterpart.\" Of course, he then burned a copy of his own book too.";
    //
    cur = 811;
    BASE_NAME[cur] = "Fentanyl";
    BASE_COST[cur] = 240;
    BASE_DESC[cur] = "Sourced from either somewhere you don't want to know or from your local hospital's unwitting anesthesia department. Either way, it'd be legally advantageous to not know.";
    //
    cur = 812;
    BASE_NAME[cur] = "IndecisivePersonsManifesto";
    BASE_COST[cur] = 340;
    BASE_DESC[cur] = "This widely acclaimed book describes how the author was conflicted in decided about whether the include the world \"The\" in the title or not and whether to eat a peanut butter sandwich only an hour after dinner or not. Not surprisingly, it was poorly received by everyone who didn't have an English Ph.D.";
    //
    cur = 1003;
    BASE_NAME[cur] = "X-ray glasses";
    BASE_COST[cur] = 650;
    BASE_DESC[cur] = "They neither emit X-rays nor function as glasses. Almost as bad as the Holy Roman Empire.";
    //
    cur = 1004;
    BASE_NAME[cur] = "Wizard hat";
    BASE_COST[cur] = 613;
    BASE_DESC[cur] = "The act of simply putting this hat on your head imbues you with wizard power.";
    //
    cur = 1005;
    BASE_NAME[cur] = "The Communist Manifesto";
    BASE_COST[cur] = 575;
    BASE_DESC[cur] = "Sample text: \"Let the ruling bosses tremble at a Communistic revolution. The proletarian shapes have nothing to lose but their chains. They have a world to win.\"";
    //
    cur = 1006;
    BASE_NAME[cur] = "Relativity";
    BASE_COST[cur] = 475;
    BASE_DESC[cur] = "Einstein's ghost teaches you 300 IQ projectile firing.";
    //
    cur = 1007;
    BASE_NAME[cur] = "Laggy Router";
    BASE_COST[cur] = 399;
    BASE_DESC[cur] = "Probably from your teammate from last game. Looks like they forgot a \".\" in the price tag.";
    //
    cur = 1008;
    BASE_NAME[cur] = "Physics I: Mechanics";
    BASE_COST[cur] = 455;
    BASE_DESC[cur] = "An introductory physics textbook, meant for remedial classes. Ironically, it looks like the cover designer needs remedial handwriting classes.";
    //
    cur = 1009;
    BASE_NAME[cur] = "CS:GO Steam Code";
    BASE_COST[cur] = 299;
    BASE_DESC[cur] = "Enlighten yourself by buying a CS:GO code for the mere price of $499.00, which is over a 99% Cyber Monday discount! You note that the \"regular\" price jumped from $14.99 to $42949672.95 yesterday. You note the useless fact that the price management software is still probably running on a 32 bit OS.";
    //
    cur = 1700;
    BASE_NAME[cur] = "Kworker Mod";
    BASE_COST[cur] = 999999;
    BASE_DESC[cur] = "You're not supposed to see this. This is an actual bug, but it's probably not game-breaking because the devs had the "
        "foresight to set the price to a high value, so you can't buy it anyway.";
    //
    for(int i=0; i<NUM_ITEMS; i++)
        if(BASE_NAME[i] != "") //it's not an empty slot
            sprites[i] = loadTexture(("data/item/" + to_str(i) + ".png").c_str(), 255, 255, 255);
}
