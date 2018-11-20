#include "game_map.h"
#include "unit.h"
#include "weapon.h"
#include "item.h"
#include "math_funcs.h"
void GameMap::init()
{
    for(int i=0; i<NUM_DIRT_SPRITES; i++)
        dirt_sprites[i] = loadTexture(("data/dirt_" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(int i=0; i<NUM_WALL_SPRITES; i++)
        wall_sprites[i] = loadTexture(("data/wall_" + to_str(i) + ".png").c_str(), 255, 255, 255);
    //units
    for(auto &i: UNIT_SPAWN_WEIGHT)
        for(auto &j: i)
            j = 0;
    UNIT_SPAWN_CHANCE[1] = 0.06;
    UNIT_SPAWN_WEIGHT[1][1] = 1;
    UNIT_SPAWN_WEIGHT[1][2] = 0.3;
    UNIT_SPAWN_WEIGHT[1][3] = 0.2;
    UNIT_SPAWN_WEIGHT[1][4] = 0.1;
    UNIT_SPAWN_WEIGHT[1][5] = 0.8;
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
    ITEM_SHOP_SPAWN_WEIGHT[1][0] = 1;
    ITEM_SHOP_SPAWN_WEIGHT[1][1] = 1;
    ITEM_SHOP_SPAWN_WEIGHT[1][2] = 1;
    ITEM_SHOP_SPAWN_WEIGHT[1][3] = 1;
    ITEM_SHOP_SPAWN_WEIGHT[1][4] = 1;
    ITEM_SHOP_SPAWN_WEIGHT[1][5] = 1;
    ITEM_SHOP_SPAWN_WEIGHT[1][6] = 1;
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
    WEAPON_SHOP_SPAWN_WEIGHT[1][2] = 1;
    WEAPON_SHOP_SPAWN_WEIGHT[1][3] = 1;
    WEAPON_SHOP_SPAWN_WEIGHT[1][4] = 1;
    WEAPON_SHOP_SPAWN_WEIGHT[1][5] = 1;
    for(int i=0; i<NUM_FLOORS; i++)
    {
        TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[i] = 0;
        for(int j=0; j<Weapon::NUM_WEAPONS; j++)
            TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[i] += WEAPON_SHOP_SPAWN_WEIGHT[i][j];
    }
}

void Unit::init()
{
    for(int i=0; i<NUM_UNITS; i++)
        sprites[i] = loadTexture(("data/unit/" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(auto &i: BASE_FIRE_BEHAVIOR)
        i = FireBehavior::none;
    //player
    int cur = 0;
    BASE_FIRE_BEHAVIOR[cur] = FireBehavior::player;
    BASE_HP[cur] = 50;
    BASE_MOVEMENT_SPEED[cur] = 0.1;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_VISION_DISTANCE[cur] = 7;
    BASE_IS_SPECTRAL[cur] = false;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::player;
    BASE_DAMAGE_ON_COLLISION[cur] = 0.3;
    //red square
    cur = 1;
    BASE_HP[cur] = 10;
    BASE_MOVEMENT_SPEED[cur] = 0.04;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::random;
    BASE_IS_SPECTRAL[cur] = false;
    BASE_DAMAGE_ON_COLLISION[cur] = 0.3;
    //yellow square
    cur = 2;
    BASE_HP[cur] = 7;
    BASE_MOVEMENT_SPEED[cur] = 0.05;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_VISION_DISTANCE[cur] = 8;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_simple;
    BASE_IS_SPECTRAL[cur] = false;
    BASE_DAMAGE_ON_COLLISION[cur] = 0.2;
    //green square
    cur = 3;
    BASE_HP[cur] = 12;
    BASE_MOVEMENT_SPEED[cur] = 0.03;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_VISION_DISTANCE[cur] = 12;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::homing_erratic;
    BASE_IS_SPECTRAL[cur] = false;
    BASE_DAMAGE_ON_COLLISION[cur] = 0.25;
    //orange square
    cur = 4;
    BASE_HP[cur] = 5;
    BASE_MOVEMENT_SPEED[cur] = 0.08;
    BASE_SHAPE[cur] = GeometricObject::Type::square;
    BASE_SIZE1[cur] = 0.8;
    BASE_VISION_DISTANCE[cur] = 6;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::cardinal;
    BASE_IS_SPECTRAL[cur] = false;
    BASE_DAMAGE_ON_COLLISION[cur] = 0.25;
    //red T tetromino
    cur = 5;
    BASE_HP[cur] = 5;
    BASE_MOVEMENT_SPEED[cur] = 0.08;
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
    BASE_VISION_DISTANCE[cur] = 6;
    BASE_MOVEMENT_BEHAVIOR[cur] = MovementBehavior::cardinal;
    BASE_IS_SPECTRAL[cur] = false;
    BASE_DAMAGE_ON_COLLISION[cur] = 0.25;
}
void Weapon::init()
{
    for(int i=0; i<NUM_WEAPONS; i++)
        sprites[i] = loadTexture(("data/weapon/" + to_str(i) + ".png").c_str(), 255, 255, 255);
    for(int i=0; i<NUM_WEAPONS; i++)
        wield_sprites[i] = loadTexture(("data/weapon/" + to_str(i) + "_wield.png").c_str(), 255, 255, 255);
    for(auto &i: BASE_AMMO) //if it's not set by at the end of this function just assume that BASE_AMMO[i] = BASE_AMMO_PER_PURCHASE[i]
        i = NOT_SET;
    for(auto &i: BASE_PROJ_PER_SHOT) //things fire 1 projectile per shot by default
        i = 1;
    for(auto &i: BASE_FIRERATE_IRREGULARITY) //by default everything has a perfectly consistent fire rate
        i = 0;
    //
    int cur = 0;
    BASE_NAME[cur] = "Poofy Pistol";
    BASE_DESC[cur] = "You shouldn't be seeing this text that describes the Poofy Pistol because "
    "the Poofy Pistol isn't supposed to appear in the shop.";
    BASE_PROJ_TYPE[cur] = 0;
    BASE_AMMO[cur] = INF;
    BASE_DAMAGE[cur] = 4;
    BASE_FIRE_RATE[cur] = 35;
    BASE_SHOT_SPEED[cur] = 0.16;
    BASE_PROJ_LIFESPAN[cur] = 30;
    BASE_SPREAD[cur] = 0.25;
    //
    cur = 1;
    BASE_NAME[cur] = "Bromine Bow";
    BASE_DESC[cur] = "A cheap bow that sells for $0 on eBay after transaction fees.";
    BASE_COST[cur] = 0;
    BASE_PROJ_TYPE[cur] = 1;
    BASE_DAMAGE[cur] = 6;
    BASE_FIRE_RATE[cur] = 31;
    BASE_SHOT_SPEED[cur] = 0.18;
    BASE_PROJ_LIFESPAN[cur] = 26;
    BASE_AMMO_PER_PURCHASE[cur] = 30;
    BASE_AMMO_COST[cur] = 10;
    BASE_SPREAD[cur] = 0.2;
    //
    cur = 2;
    BASE_NAME[cur] = "Cadmium Crossbow";
    BASE_DESC[cur] = "It's a bit better than a normal bow.";
    BASE_COST[cur] = 80;
    BASE_PROJ_TYPE[cur] = 1;
    BASE_DAMAGE[cur] = 8;
    BASE_FIRE_RATE[cur] = 42;
    BASE_SHOT_SPEED[cur] = 0.23;
    BASE_PROJ_LIFESPAN[cur] = 32;
    BASE_AMMO_PER_PURCHASE[cur] = 25;
    BASE_AMMO_COST[cur] = 15;
    BASE_SPREAD[cur] = 0.14;
    //
    cur = 3;
    BASE_NAME[cur] = "Device WH-722";
    BASE_DESC[cur] = "The name obviously tells you all you need to know.";
    BASE_COST[cur] = 30;
    BASE_PROJ_TYPE[cur] = 2;
    BASE_DAMAGE[cur] = 0.8;
    BASE_FIRE_RATE[cur] = 55;
    BASE_SHOT_SPEED[cur] = 0.15;
    BASE_PROJ_LIFESPAN[cur] = 35;
    BASE_AMMO_PER_PURCHASE[cur] = 20;
    BASE_AMMO_COST[cur] = 20;
    BASE_SPREAD[cur] = PI / 4;
    BASE_PROJ_PER_SHOT[cur] = 20;
    //
    cur = 4;
    BASE_NAME[cur] = "Unbranded Sniper Rifle";
    BASE_DESC[cur] = "It was made in an obscure Chinese sweatshop.";
    BASE_COST[cur] = 76;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 18;
    BASE_FIRE_RATE[cur] = 127;
    BASE_SHOT_SPEED[cur] = 0.30;
    BASE_PROJ_LIFESPAN[cur] = 50;
    BASE_AMMO_PER_PURCHASE[cur] = 20;
    BASE_AMMO_COST[cur] = 20;
    BASE_SPREAD[cur] = 0.12;
    //
    cur = 5;
    BASE_NAME[cur] = "Paintball Gun";
    BASE_DESC[cur] = "Shoots lots of paintballs. Slightly broken and fires irregularly.";
    BASE_COST[cur] = 45;
    BASE_PROJ_TYPE[cur] = 0;
    BASE_DAMAGE[cur] = 3.5;
    BASE_FIRE_RATE[cur] = 14;
    BASE_SHOT_SPEED[cur] = 0.15;
    BASE_PROJ_LIFESPAN[cur] = 22;
    BASE_AMMO_PER_PURCHASE[cur] = 60;
    BASE_AMMO_COST[cur] = 10;
    BASE_SPREAD[cur] = 0.27;
    BASE_FIRERATE_IRREGULARITY[cur] = 1; //ranges from fireRate to 2*fireRate
    //
    for(int i=0; i<NUM_WEAPONS; i++) //by default, weapons come with one purchase of ammo
        if(BASE_AMMO[i] == NOT_SET)
            BASE_AMMO[i] = BASE_AMMO_PER_PURCHASE[i];
}
void Item::init()
{
    for(int i=0; i<NUM_ITEMS; i++)
        sprites[i] = loadTexture(("data/item/" + to_str(i) + ".png").c_str(), 255, 255, 255);
    //Spiked vest
    int cur = 0;
    BASE_NAME[cur] = "Spiked Vest";
    BASE_COST[cur] = 80;
    BASE_DESC[cur] = "The spikes eviscerate enemies and cause you to deal 200% contact damage.";
    //bullseye
    cur = 1;
    BASE_NAME[cur] = "Bullseye";
    BASE_COST[cur] = 65;
    BASE_DESC[cur] = "Getting a bullseye tattoo somehow helps you fire more accurately.";
    //bullseye
    cur = 2;
    BASE_NAME[cur] = "Dirty Serrated Shirt";
    BASE_COST[cur] = 50;
    BASE_DESC[cur] = "A dirty shirt that is somehow serrated. Probably made in China. "
    "Gives enemies tetanus on contact, causing them to move more slowly for a bit.";
    //blue boots
    cur = 3;
    BASE_NAME[cur] = "Blue Boots";
    BASE_COST[cur] = 75;
    BASE_DESC[cur] = "Boots with blue coloring. Blue makes you happy so you move a bit faster.";
    //blue boots
    cur = 4;
    BASE_NAME[cur] = "Red Boots";
    BASE_COST[cur] = 75;
    BASE_DESC[cur] = "Boots with red coloring, which gives you sadistic urges."
    "Gain a blood rush and move faster when damaging others with projectiles.";
    //sadist's machete
    cur = 5;
    BASE_NAME[cur] = "Sadist's Machete";
    BASE_COST[cur] = 85;
    BASE_DESC[cur] = "This toy seems like lots of fun! Deal a bit more damage and heal for a small portion of all damage dealt.";
    //green boots
    cur = 6;
    BASE_NAME[cur] = "Green Boots";
    BASE_COST[cur] = 85;
    BASE_DESC[cur] = "Like any other boots, but colored green. Move faster when damaged.";
}
