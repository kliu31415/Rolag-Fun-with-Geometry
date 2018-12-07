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
//can't forward declare enum classes so I have to keep it here
enum class MapTile{not_set, dirt, blank, wall, hurt, portal, slip, connection};
class GenericMapRoom
{
protected:
    void dfsValidate(std::vector<std::vector<bool> > &visited, int x, int y) const;
    bool isValidRoom() const;
public:
    virtual void setMapSize(int columns, int rows);
    int moneyBonusOnCompletion;
    std::vector<std::vector<MapTile> > tiles;
    std::vector<std::vector<double> > timeTillCollapse;
    std::vector<std::shared_ptr<Unit> > units;
    void surroundWithTile(MapTile t);
    void fillWithTile(MapTile t);
    inline bool isPassableTile(int x, int y) const
    {
        return x>=0 && y>=0 && x<getNumColumns() && y<getNumRows() && (tiles[x][y] != MapTile::wall && tiles[x][y] != MapTile::blank &&
                                                                       tiles[x][y] != MapTile::not_set);
    }
    inline bool isFlyableTile(int x, int y) const
    {
        return x>=0 && y>=0 && x<getNumColumns() && y<getNumRows() && (tiles[x][y] != MapTile::wall && tiles[x][y] != MapTile::not_set);
    }
    inline int getNumRows() const
    {
        if(tiles.size() == 0)
        {
            print_warning("trying to fetch number of rows, but there are 0 columns");
            return 0;
        }
        return tiles[0].size();
    }
    __attribute__((always_inline)) int getNumColumns() const
    {
        return tiles.size();
    }
    __attribute__((always_inline)) int getArea() const
    {
        return getNumRows() * getNumColumns();
    }
    inline bool isInMap(int x, int y) const
    {
        return x>=0 && y>=0 && x<getNumColumns() && y<getNumRows();
    }
};
class Room: public GenericMapRoom //preset room
{
    void generateEnemies(double spawnChance, fast_map<int, double> &spawn_weight);
    void generateGenericRoom(int columns, int rows, int numClusters, double fillFactor, double collapse_time, double slip_tile_pct);
public:
    void (*custom_func)(Room&);
    double weight;
    Room(int numColumns, int numRows, double weight = 1);
    Room();
    static void init();
    static void init_floor_1();
    static void init_floor_3();
    static void init_floor_5();
    static void init_floor_7();
};
class Player;
class GameMap: public GenericMapRoom
{
public:
    static constexpr int NUM_FLOORS = 25; //in reality, the 0th floor isn't used, so actual floors = NUM_FLOORS - 1
private:
    static constexpr int NUM_DIRT_SPRITES = 8, NUM_WALL_SPRITES = 9, NUM_HURT_SPRITES = 2, NUM_PORTAL_SPRITES = 2, NUM_COLLAPSE_SPRITES = 2;
    static constexpr int NUM_SLIP_SPRITES = 2;
    static double UNIT_SPAWN_WEIGHT[NUM_FLOORS][Unit::NUM_UNITS];
    static double TOTAL_UNIT_SPAWN_WEIGHT[NUM_FLOORS];
    static int BASE_FLOOR_TIME[NUM_FLOORS]; //the number of seconds there are to finish the floor before you start losing health
    static SDL_Texture *dirt_sprites[NUM_DIRT_SPRITES], *wall_sprites[NUM_WALL_SPRITES], *hurt_sprites[NUM_HURT_SPRITES];
    static SDL_Texture *portal_closed_sprites[NUM_PORTAL_SPRITES], *portal_open_sprites[NUM_PORTAL_SPRITES];
    static SDL_Texture *collapse_sprites[NUM_COLLAPSE_SPRITES], *slip_sprites[NUM_SLIP_SPRITES];
    static SDL_Texture *blank_sprite;
    SDL_Texture *current_dirt_sprite, *current_blank_sprite, *current_hurt_sprite;
    SDL_Texture *current_portal_closed_sprite, *current_portal_open_sprite, *current_collapse_sprite;
    SDL_Texture *current_slip_sprite;
    int floor;
    double startPositionX, startPositionY;
    double cameraX, cameraY;
    void create_background_texture(uint8_t r, uint8_t g, uint8_t b);
    void setMapSize(int columns, int rows);
    bool paste_room(Room &room, int x, int y);
    Room getRandomRoom();
    void generateRandomRoomArrangement(int c, int r, double fillPct);
    void generateMapFloor1();
    void generateMapFloor2();
    void generateMapFloor3();
    void generateMapFloor4();
    void generateMapFloor5();
    void generateMapFloor6();
    void generateMapFloor7();
    void generateMapFloor8();
    void generateMapFloor9();
    void generateMapFloor10();
    void generateMapFloor11();
    void generateMapFloor12();
    void generateMapFloor13();
    void generateMapFloor14();
    void generateMapFloor15();
    void generateMapFloor16();
    void generateMapFloor17();
    void generateMapFloor18();
    void generateMapFloor19();
    void generateMapFloor20();
    void generateFloorEnemies(double spawnChance);
    void generateFloorCollapsing(double time, double slip_tile_pct = 0);
    void generateFloorMaze(double collapse_time = NOT_SET, int numExtraConnections = 0, double slip_tile_pct = 0, double wall_pct = 0.25);
    void generateFloorGeneric(int numClusters, double fillFactor, double collapse_time = NOT_SET, double slip_tile_pct = 0);
    void getMinimapRect(const GameState &game_state, int *x, int *y, int *w, int *h) const;
    void generateFloor(GameState &game_state);
    bool ok_connection_tile(int x, int y, bool isFirst = false);
    void draw_tile(MapTile tile, int i, int j, int x, int y, int w, int h);
    //void generateShortestPaths();
    SDL_Texture *minimap_texture;
public:
    static std::vector<Room> PRESET_ROOMS[NUM_FLOORS];
    static std::string BASE_FLOOR_NAME[NUM_FLOORS];
    static double TOTAL_ITEM_SHOP_SPAWN_WEIGHT[NUM_FLOORS], ITEM_SHOP_SPAWN_WEIGHT[NUM_FLOORS][Item::NUM_ITEMS];
    static double TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[NUM_FLOORS], WEAPON_SHOP_SPAWN_WEIGHT[NUM_FLOORS][Weapon::NUM_WEAPONS];
    //SDL_Texture *explored_texture
    std::vector<std::vector<double> > initialTimeTillCollapse;
    std::vector<std::vector<double> > time_since_explored;
    int num_rooms;
    std::vector<std::vector<int> > belongs_to_room;
    std::vector<bool> found_rooms;
    //std::vector<std::vector<bool> > explored;
    std::vector<Projectile> projectiles;
    fast_set<pii> alreadyCollided;
    SDL_Texture *current_wall_sprite;
    double floorTimeLeft;
    double gravity;
    GameMap();
    GameMap(GameState &game_state);
    static void init();
    void setCameraPosition(const GameState &game_state, double x, double y);
    void update_explored(const std::shared_ptr<Player> &player, double vision_distance);
    double getCameraX() const;
    double getCameraY() const;
    double getStartPositionX() const;
    double getStartPositionY() const;
    double getMinimapPixelsPerTile() const;
    void getMinimapSize(int *w, int *h) const;
    void draw(const GameState &game_state);
    bool isFloorFinished() const;
    bool isOutOfMap(const GeometricObject *x) const;
    void generateMinimapTexture(const GameState &game_state);
    //void generateExploredTexture(const GameState &game_state);
    void drawMinimap(const GameState &game_state);
    void endFloor();
    std::shared_ptr<Unit> getUnitByID(int id) const;
};
