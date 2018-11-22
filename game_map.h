#pragma once
#include <SDL2/SDL_image.h>
#include <vector>
#include <set>
#define pii std::pair<int, int>
#include "fundamentals.h"
#include "unit.h"
#include "weapon.h"
#include "item.h"
class GameState;
class GameMap
{
    static constexpr int NUM_DIRT_SPRITES = 1, NUM_WALL_SPRITES = 2;
    static constexpr int NUM_FLOORS = 3; //in reality, the 0th floor isn't used, so actual floors = NUM_FLOORS - 1
    static double UNIT_SPAWN_WEIGHT[NUM_FLOORS][Unit::NUM_UNITS];
    static double UNIT_SPAWN_CHANCE[NUM_FLOORS], TOTAL_UNIT_SPAWN_WEIGHT[NUM_FLOORS];
    static int BASE_FLOOR_TIME[NUM_FLOORS]; //the number of seconds there are to finish the floor before you start losing health
    static int FLOOR_W[NUM_FLOORS], FLOOR_H[NUM_FLOORS];
    static SDL_Texture *dirt_sprites[NUM_DIRT_SPRITES], *wall_sprites[NUM_WALL_SPRITES];
    SDL_Texture *current_wall_sprite, *current_dirt_sprite, *current_blank_sprite;
    enum class MapTile{not_set, dirt, blank, wall};
    int floor;
    double startPositionX, startPositionY;
    double cameraX, cameraY;
    std::vector<std::vector<MapTile> > tiles;
    bool isInMap(int x, int y) const;
    void setMapSize(int columns, int rows);
    void generateMapFloor1();
    void generateMapFloor2();
    void generateFloorGeneric(int numClusters, double fillFactor);
    void dfsValidate(std::vector<std::vector<bool> > &visited, int x, int y) const;
    bool isValidFloor() const;
    void getMinimapRect(const GameState &game_state, int *x, int *y, int *w, int *h) const;
    void generateMinimapTexture(const GameState &game_state);
    void generateFloor();
    //void generateShortestPaths();
    SDL_Texture *minimap_texture;
public:
    static std::string BASE_FLOOR_NAME[NUM_FLOORS];
    static double TOTAL_ITEM_SHOP_SPAWN_WEIGHT[NUM_FLOORS], ITEM_SHOP_SPAWN_WEIGHT[NUM_FLOORS][Item::NUM_ITEMS];
    static double TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[NUM_FLOORS], WEAPON_SHOP_SPAWN_WEIGHT[NUM_FLOORS][Weapon::NUM_WEAPONS];
    //SDL_Texture *explored_texture;
    std::vector<std::vector<bool> > explored;
    std::vector<Projectile> projectiles;
    std::vector<std::shared_ptr<Unit> > units;
    std::vector<std::vector<std::vector<std::vector<pii> > > > shortestPathDir;
    fast_set<pii> alreadyCollided;
    double floorTimeLeft;
    GameMap();
    GameMap(GameState &game_state);
    static void init();
    void setCameraPosition(double x, double y);
    double getCameraX() const;
    double getCameraY() const;
    bool isPassableTile(int x, int y) const;
    int getNumRows() const;
    int getNumColumns() const;
    int getArea() const;
    double getStartPositionX() const;
    double getStartPositionY() const;
    double getMinimapPixelsPerTile() const;
    void getMinimapSize(int *w, int *h) const;
    void draw(const GameState &game_state) const;
    //void generateExploredTexture(const GameState &game_state);
    void drawMinimap(GameState &game_state) const;
    void endFloor();
    std::shared_ptr<Unit> getUnitByID(int id) const;
};
extern GameMap currentGameMap;
