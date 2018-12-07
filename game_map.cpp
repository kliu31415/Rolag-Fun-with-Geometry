#include "game_map.h"
#include "sdl_base.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <numeric>
#include <set>
#include <utility>
#include <queue>
#include "game_state.h"
SDL_Texture *GameMap::dirt_sprites[GameMap::NUM_DIRT_SPRITES], *GameMap::wall_sprites[GameMap::NUM_WALL_SPRITES];
SDL_Texture *GameMap::hurt_sprites[GameMap::NUM_HURT_SPRITES], *GameMap::collapse_sprites[GameMap::NUM_COLLAPSE_SPRITES];
SDL_Texture *GameMap::portal_closed_sprites[GameMap::NUM_PORTAL_SPRITES], *GameMap::portal_open_sprites[GameMap::NUM_PORTAL_SPRITES];
SDL_Texture *GameMap::slip_sprites[GameMap::NUM_SLIP_SPRITES];
SDL_Texture *GameMap::blank_sprite;
std::vector<Room> GameMap::PRESET_ROOMS[GameMap::NUM_FLOORS];
int GameMap::BASE_FLOOR_TIME[GameMap::NUM_FLOORS];
std::string GameMap::BASE_FLOOR_NAME[GameMap::NUM_FLOORS];
double GameMap::UNIT_SPAWN_WEIGHT[GameMap::NUM_FLOORS][Unit::NUM_UNITS];
double GameMap::TOTAL_UNIT_SPAWN_WEIGHT[GameMap::NUM_FLOORS];
double GameMap::TOTAL_ITEM_SHOP_SPAWN_WEIGHT[GameMap::NUM_FLOORS], GameMap::ITEM_SHOP_SPAWN_WEIGHT[GameMap::NUM_FLOORS][Item::NUM_ITEMS];
double GameMap::TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[GameMap::NUM_FLOORS], GameMap::WEAPON_SHOP_SPAWN_WEIGHT[GameMap::NUM_FLOORS][Weapon::NUM_WEAPONS];
//
GameMap::GameMap()
{
    minimap_texture = NULL;
}
GameMap::GameMap(GameState &game_state)
{
    this->floor = game_state.currentFloor;
    this->gravity = 0;
    minimap_texture = NULL;
    //explored_texture = NULL;
    generateFloor(game_state);
    generateMinimapTexture(game_state);
    double xFloorTimeLeft = 1;
    for(auto &i: game_state.getPlayer()->items)
    {
        switch((Item::Name)i.type)
        {
        case Item::Name::DaltonsAtom:
            xFloorTimeLeft += 0.1;
            break;
        default:
            break;
        }
    }
    this->floorTimeLeft = BASE_FLOOR_TIME[floor] * xFloorTimeLeft;
    //generateShortestPaths(); buggy and computationally expensive and time consuming to make a good one
}
void GameMap::endFloor()
{
    if(minimap_texture != NULL)
    {
        SDL_DestroyTexture(minimap_texture);
        minimap_texture = NULL;
    }
    /*if(explored_texture != NULL)
    {
        SDL_DestroyTexture(explored_texture);
        explored_texture = NULL;
    }*/
    moneyBonusOnCompletion = 0;
    num_rooms = 0;
    units.clear();
    projectiles.clear();
    //explored.clear();
    tiles.clear();
    timeTillCollapse.clear();
    initialTimeTillCollapse.clear();
    belongs_to_room.clear();
    time_since_explored.clear();
    found_rooms.clear();
}
double GameMap::getStartPositionX() const
{
    return startPositionX;
}
double GameMap::getStartPositionY() const
{
    return startPositionY;
}
bool GameMap::isOutOfMap(const GeometricObject *x) const
{
    //getFloor... is a bit better but whatever this works too
    int W = 0.5 * getDisplayW() / getPixelsPerTile();
    int H = 0.5 * getDisplayH() / getPixelsPerTile();
    return x->getX() < -MAX_RADIUS - W || x->getX() > getNumColumns() + MAX_RADIUS + W ||
            x->getY() < -MAX_RADIUS - H || x->getY() > getNumRows() + MAX_RADIUS + H;
}
bool GameMap::isFloorFinished() const
{
    for(auto &i: units)
    {
        if(areEnemies(Affiliation::player, i->affiliation) && i->maxHP != INF)
        {
            return false;
        }
    }
    return true;
}
double GameMap::getCameraX() const
{
    return cameraX;
}
double GameMap::getCameraY() const
{
    return cameraY;
}
std::shared_ptr<Unit> GameMap::getUnitByID(int id) const
{
    for(auto &i: units)
        if(i->ID == id)
            return i;
    return nullptr;
}
void GameMap::setCameraPosition(const GameState &game_state, double x, double y)
{
    /*int ppt = getPixelsPerTile();
    if(ppt * getNumColumns() < game_state.getFloorDisplayW())
        cameraX = getNumColumns()/2.0 - game_state.getFloorDisplayW()/(2.0*getPixelsPerTile());
    else */cameraX = x;
    cameraY = y;
}
void GameMap::setMapSize(int columns, int rows)
{
    time_since_explored.clear();
    time_since_explored.resize(columns, std::vector<double>(rows, NOT_SET));
    tiles.clear();
    tiles.resize(columns, std::vector<MapTile>(rows, MapTile::not_set));
    belongs_to_room.clear();
    belongs_to_room.resize(columns, std::vector<int>(rows, NOT_SET));
    timeTillCollapse.resize(columns, std::vector<double>(rows, NOT_SET));
    initialTimeTillCollapse.resize(columns, std::vector<double>(rows, NOT_SET));
}
void GameMap::generateFloorEnemies(double spawnChance)
{
    double curDiff = 0; //difference between # of spawned units so far and expected number of spawned units
    double curChance = spawnChance; //if we were supposed to create a unit but it didn't fit, then increase this
    for(int i=0; i<getNumColumns(); i++) //add enemies
        for(int j=0; j<getNumRows(); j++)
            if(isPassableTile(i, j))
            {
                curDiff += spawnChance;
                if(std::hypot(getStartPositionX()-i, getStartPositionY()-j) > 8 && randf() - curDiff / 10 < curChance)
                {
                    double x = randf() * TOTAL_UNIT_SPAWN_WEIGHT[floor];
                    for(int k=1; k<Unit::NUM_UNITS; k++)
                    {
                        if(x < UNIT_SPAWN_WEIGHT[floor][k])
                        {
                            std::shared_ptr<Unit> unit = std::make_shared<Unit>(k, Affiliation::generic_enemy, i, j);
                            if(unit->collidesWithTerrain(*this) || unit->collidesWithOtherUnit_checkToValidateMap(units))
                            {
                                curChance = 1;
                                break;
                            }
                            curDiff--;
                            curChance = spawnChance;
                            units.push_back(unit);
                            break;
                        }
                        x -= UNIT_SPAWN_WEIGHT[floor][k];
                    }
                }
            }
}
void GameMap::generateFloorCollapsing(double time, double slip_tile_pct)
{
    //randomized kruskal's for maze generation
    moneyBonusOnCompletion += 0.04 * floor * getArea() * (1 + 1 / (1 + time));
    int numC = getNumColumns(), numR = getNumRows();
    for(auto &i: tiles)
        for(auto &j: i)
            j = randf() < slip_tile_pct? (MapTile::slip): (MapTile::dirt);
    for(auto &i: initialTimeTillCollapse)
        for(auto &j: i)
            j = time;
    for(auto &i: timeTillCollapse)
        for(auto &j: i)
            j = time;
    startPositionX = randz(0, numC-1);
    startPositionY = randz(0, numR-1);
    tiles[startPositionX][startPositionY] = MapTile::portal;
}
/*numExtraConnections equals the max number of extra "edges" we want to add the maze graph to make it easier by adding cycles
note that the current algo might add the same connection multiple times to no effect
so numExtraConnections is the MAX number of extra edges that will be added. In reality, around 1/2 will be added if it's small.*/
void GameMap::generateFloorMaze(double collapse_time, int numExtraConnections, double slip_tile_pct, double wall_pct)
{
    //randomized kruskal's for maze generation
    int numC = getNumColumns(), numR = getNumRows();
    if(numC%3!=1 || numR%3!=1)
        print_error("both the number of columns and rows must be congruent to 1 mod 3 in order to correctly generate a maze pattern");
    moneyBonusOnCompletion += 0.04 * floor * getArea() * (1 + 1 / (1 + collapse_time));
    for(auto &i: tiles)
        for(auto &j: i)
        {
            if(randf() < wall_pct)
                j = MapTile::wall;
            else j = MapTile::hurt;
        }
    numC /= 3;
    numR /= 3;
    for(int i=0; i<numC; i++)
    {
        for(int j=0; j<numR; j++)
        {
            tiles[i*3+1][j*3+1] = randf() < slip_tile_pct? (MapTile::slip): (MapTile::dirt);
            tiles[i*3+1][j*3+2] = randf() < slip_tile_pct? (MapTile::slip): (MapTile::dirt);
            tiles[i*3+2][j*3+1] = randf() < slip_tile_pct? (MapTile::slip): (MapTile::dirt);
            tiles[i*3+2][j*3+2] = randf() < slip_tile_pct? (MapTile::slip): (MapTile::dirt);
        }
    }
    for(auto &i: initialTimeTillCollapse)
        for(auto &j: i)
            j = collapse_time;
    for(auto &i: timeTillCollapse)
        for(auto &j: i)
            j = collapse_time;
    DisjointSet dsu(numC * numR);
    for(int i=1; i<numC*numR + numExtraConnections; i++)
    {
        int x1 = randz(0, numC - 1), y1 = randz(0, numR - 1);
        int x2 = x1, y2 = y1;
        switch(randz(0, 3))
        {
        case 0:
            x2--;
            break;
        case 1:
            x2++;
            break;
        case 2:
            y2--;
            break;
        case 3:
            y2++;
            break;
        }
        if(x2<0 || y2<0 || x2>=numC || y2>=numR || (i < numC * numR && dsu.getp(y1*numC + x1)==dsu.getp(y2*numC + x2)))
        {
            i--;
            continue;
        }
        dsu.combine(y1*numC + x1, y2*numC + x2);
        for(int a=1; a<=2; a++)
            for(int b=1; b<=2; b++)
                tiles[1.5*(x1+x2)+a][1.5*(y1+y2)+b] = randf() < slip_tile_pct? (MapTile::slip): (MapTile::dirt);
    }
    startPositionX = randz(0, numC-1) * 3 + randz(1, 2);
    startPositionY = randz(0, numR-1) * 3 + randz(1, 2);
    tiles[startPositionX][startPositionY] = MapTile::portal;
}
void GameMap::generateFloorGeneric(int numClusters, double fillFactor, double collapse_time, double slip_tile_pct)
{
    if(numClusters > getArea())
        print_error("number of clusters is greater than map area");
    moneyBonusOnCompletion += 0.04 * floor * getArea() * (1 / (1 + collapse_time));
    for(auto &i: initialTimeTillCollapse)
        for(auto &j: i)
            j = collapse_time;
    for(auto &i: timeTillCollapse)
        for(auto &j: i)
            j = collapse_time;
    std::vector<pii> frontier;
    std::vector<std::vector<bool> > alreadyUsed(getNumColumns(), std::vector<bool>(getNumRows(), false));
    std::vector<pii> startingPoints;
    for(int i=0; i<numClusters; i++) //set "seed" spots
    {
        int x = randz(0, getNumColumns()-1), y = randz(0, getNumRows()-1);
        if(tiles[x][y] == MapTile::not_set)
        {
            tiles[x][y] = MapTile::dirt;
            startingPoints.emplace_back(x, y);
            frontier.emplace_back(x-1, y);
            frontier.emplace_back(x+1, y);
            frontier.emplace_back(x, y-1);
            frontier.emplace_back(x, y+1);
            if(i == 0)
            {
                startPositionX = x;
                startPositionY = y;
            }
        }
        else
        {
            i--;
            continue;
        }
    }
    //connect the "seeds" so that the map is connected
    DisjointSet connections(numClusters);
    for(int i=1; i<(int)startingPoints.size(); i++)
    {
        std::string path; //connection path
        int pos1 = randz(0, startingPoints.size()-1), pos2 = randz(0, startingPoints.size()-1);
        if(connections.getp(pos1) == connections.getp(pos2))
            continue;
        connections.combine(pos1, pos2);
        auto &prv = startingPoints[pos1], &cur = startingPoints[pos2];
        for(int j=0; j<cur.A - prv.A; j++)
            path += 'D';
        for(int j=0; j<prv.A - cur.A; j++)
            path += 'U';
        for(int j=0; j<cur.B - prv.B; j++)
            path += 'R';
        for(int j=0; j<prv.B - cur.B; j++)
            path += 'L';
        //some randomness is always good, so make the paths a bit wonky
        for(int j=0; j < 5 + path.size()*0.5; j++)
        {
            switch(randz(0, 3))
            {
            case 0:
                path += 'D';
                break;
            case 1:
                path += 'U';
                break;
            case 2:
                path += 'R';
                break;
            case 3:
                path += 'L';
                break;
            }
        }
        std::shuffle(path.begin(), path.end(), generateRandZ());
        int curX = prv.A, curY = prv.B;
        for(auto &i: path)
        {
            switch(i)
            {
            case 'U':
                curY--;
                break;
            case 'D':
                curY++;
                break;
            case 'R':
                curX++;
                break;
            case 'L':
                curX--;
                break;
            }
            if(isInMap(curX, curY))
            {
                if(randf() < slip_tile_pct)
                    tiles[curX][curY] = MapTile::slip;
                else tiles[curX][curY] = MapTile::dirt;
            }
        }
    }
    tiles[startPositionX][startPositionY] = MapTile::portal;
    int counter = getArea() * fillFactor; //how many more tiles do we have to fill?
    for(auto &i: tiles)
        for(auto &j: i)
            if(j != MapTile::not_set)
                counter--;
    if(counter < 0)
        print_warning("map is overfilled, counter = " + to_str(counter));
    while(counter > 0) //fill in more tiles with dirt
    {
        if(frontier.size() == 0)
            print_error("frontier.size()==0 in the middle of map generation");
        int pos = randz(0, frontier.size()-1);
        int x = frontier[pos].A, y = frontier[pos].B;
        if(isInMap(x, y) && tiles[x][y] == MapTile::not_set)
        {
            if(randf() < slip_tile_pct)
                tiles[x][y] = MapTile::slip;
            else tiles[x][y] = MapTile::dirt;
            counter--;
            if(std::find(frontier.begin(), frontier.end(), mp(x-1, y)) == frontier.end())
                frontier.emplace_back(x-1, y);
            if(std::find(frontier.begin(), frontier.end(), mp(x+1, y)) == frontier.end())
                frontier.emplace_back(x+1, y);
            if(std::find(frontier.begin(), frontier.end(), mp(x, y-1)) == frontier.end())
                frontier.emplace_back(x, y-1);
            if(std::find(frontier.begin(), frontier.end(), mp(x, y+1)) == frontier.end())
                frontier.emplace_back(x, y+1);
        }
    }
    for(auto &i: tiles) //every tile that isn't set is a wall
        for(auto &j: i)
            if(j == MapTile::not_set)
                j = MapTile::wall;
}
bool GameMap::paste_room(Room &room, int x, int y) //only pastes the room if it's legal to
{
    for(int i=-1; i<=room.getNumColumns(); i++)
        for(int j=-1; j<=room.getNumRows(); j++)
            if(!isInMap(x+i, y+j) || belongs_to_room[x+i][y+j] != NOT_SET) //can't fit - rooms can't overlap or touch other rooms
                return false;
    moneyBonusOnCompletion += floor * room.moneyBonusOnCompletion;
    for(auto &i: room.units)
    {
        units.push_back(i);
        units.back()->setX(x + units.back()->getX());
        units.back()->setY(y + units.back()->getY());
        units.back()->is_active = false;
        units.back()->belongs_to_room = num_rooms;
    }
    for(int i=0; i<room.getNumColumns(); i++)
        for(int j=0; j<room.getNumRows(); j++)
        {
            initialTimeTillCollapse[x+i][y+j] = timeTillCollapse[x+i][y+j] = room.timeTillCollapse[i][j];
            tiles[x+i][y+j] = room.tiles[i][j];
        }
    for(int i=-1; i<room.getNumColumns()+1; i++)
    {
        for(int j=-1; j<room.getNumRows()+1; j++)
        {
            if(isInMap(x+i, y+j) && isPassableTile(x+i, y+j))
            {
                for(int a=-1; a<=1; a++)
                    for(int b=-1; b<=1; b++)
                        if(isInMap(x+i+a, y+j+b))
                           belongs_to_room[x+i+a][y+j+b] = num_rooms;
            }
        }
    }
    num_rooms++;
    return true; //successfully copied and pasted room
}
Room GameMap::getRandomRoom()
{
    double total_weight = 0;
    for(auto &i: PRESET_ROOMS[floor])
        total_weight += i.weight;
    double x = randf() * total_weight;
    for(int i=0; i<(int)PRESET_ROOMS[floor].size(); i++)
    {
        if(x < PRESET_ROOMS[floor][i].weight)
        {
            Room r = PRESET_ROOMS[floor][i];
            if(r.custom_func != nullptr)
                r.custom_func(r);
            return r;
        }
        x -= PRESET_ROOMS[floor][i].weight;
    }
    print_warning("could not fetch a random room");
    Room r = PRESET_ROOMS[floor][0]; //avoid compiler warning and just assume it's room 0
    if(r.custom_func != nullptr)
        r.custom_func(r);
    return r;
}
bool GameMap::ok_connection_tile(int x, int y, bool isFirst)
{
    //the way it's currently implemented tiles on the edge of the map will never return true but that should be fine
    return x>0 && y>0 && x<getNumColumns()-1 && y<getNumRows()-1 &&tiles[x][y] == MapTile::not_set;
}
void GameMap::generateRandomRoomArrangement(int c, int r, double fillPct)
{
    /*pregenerating rooms is a nice idea, but it's messy because the modifications to the units
    aka setX and setY carry through multiple attempts
    */
    double t = getTicksS();
    setMapSize(c, r);
    int goal = fillPct * c * r;
    int iter = 0;
    std::vector<pii> cornerPos;
    cornerPos.emplace_back(randz(1, 3), randz(1, 3));
    while(iter++ < 100 && goal > 0)
    {
        Room room = getRandomRoom();
        if(goal - room.getArea() < 0) //don't overfill
            continue;
        pii pos = cornerPos[randz(0, cornerPos.size()-1)];
        pos.A += randz(2, 6);
        pos.B += randz(2, 6);
        if(paste_room(room, pos.A, pos.B))
        {
            goal -= room.getArea();
            iter = 0;
            for(int i=-room.getNumColumns()/2; i<=room.getNumColumns()/2; i++)
                cornerPos.emplace_back(pos.A + i, pos.B + room.getNumRows() + 1);
            for(int i=-room.getNumRows()/2; i<=room.getNumRows()/2; i++)
                cornerPos.emplace_back(pos.A + room.getNumColumns() + 1, pos.B + i);
        }
        //generateFloorGeneric(10, 0.5);
        //generateFloorEnemies(0.08);
    }
    if(num_rooms == 0) //this shouldn't happen, but if it does try again
    {
        print_error("could not generate any rooms");
        return;
    }
    //create starting room
    bool initStartingRoom = false;
    static Room starting_room;
    if(!initStartingRoom)
    {
        initStartingRoom = true;
        starting_room = Room(3, 3);
        starting_room.fillWithTile(MapTile::dirt);
        starting_room.tiles[1][1] = MapTile::portal;
    }
    startPositionX = NOT_SET;
    iter = 0;
    while(iter++ < 200)
    {
        pii pos = mp(randz(1, getNumColumns() - 3), randz(1, getNumRows() - 3));
        if(paste_room(starting_room, pos.A, pos.B))
        {
            startPositionX = pos.A + 1;
            startPositionY = pos.B + 1;
            break;
        }
    }
    if(startPositionX == NOT_SET)
        print_error("could not place starting room");
    found_rooms.resize(num_rooms, false);
    //
    DisjointSet dsu(getArea());
    std::vector<pii> seeds;
    int numPassableTiles = 0, passableTilePos = NOT_SET;
    //ignoring edges should be fine here because connections can't really run along the edges that well anyway
    for(int i=1; i<getNumColumns()-1; i++)
    {
        for(int j=1; j<getNumRows()-1; j++)
        {
            if(isPassableTile(i, j))
            {
                if(tiles[i-1][j]==MapTile::not_set || tiles[i+1][j]==MapTile::not_set ||
                   tiles[i][j-1]==MapTile::not_set || tiles[i][j+1]==MapTile::not_set)
                    seeds.emplace_back(i, j);
                numPassableTiles++;
                passableTilePos = i + j * getNumColumns(); //not optimized but it works
                if(isPassableTile(i+1, j))
                    dsu.combine(i + j*getNumColumns(), i+1 + j*getNumColumns());
                if(isPassableTile(i, j+1))
                    dsu.combine(i + j*getNumColumns(), i + (j+1)*getNumColumns());
            }
        }
    }
    if(passableTilePos == NOT_SET)
        print_warning("no passable tiles found when trying to generate connection seeds");
    //so this could technically fail if a room is pretty much completely walled at the edges
    iter = 0;
    while(iter++<1000 && numPassableTiles != dsu.size_of_component(passableTilePos))
    {
        int pos1 = randz(0, seeds.size() - 1), pos2 = randz(0, seeds.size() - 1);
        int x1 = seeds[pos1].A, y1 = seeds[pos1].B;
        int x2 = seeds[pos2].A, y2 = seeds[pos2].B;
        if(dsu.getp(x1 + y1 * getNumColumns()) != dsu.getp(x2 + y2 * getNumColumns()))
        {
            std::string path;
            for(int j=0; j<y2 - y1; j++)
                path += 'D';
            for(int j=0; j<y1 - y2; j++)
                path += 'U';
            for(int j=0; j<x2 - x1; j++)
                path += 'R';
            for(int j=0; j<x1 - x2; j++)
                path += 'L';
            std::shuffle(path.begin(), path.end(), generateRandZ());
            int curX = x1, curY = y1;
            bool bad = false;
            for(auto &i: path)
            {
                switch(i)
                {
                case 'U':
                    curY--;
                    break;
                case 'D':
                    curY++;
                    break;
                case 'R':
                    curX++;
                    break;
                case 'L':
                    curX--;
                    break;
                }
                if(tiles[curX][curY] != MapTile::not_set && !isPassableTile(curX, curY))
                {
                    bad = true;
                    break;
                }
            }
            if(bad) //we can't create a full path, so don't do it. It isn't necessary to do this, but it results in nicer looking maps
                continue; //because there are fewer connecting tiles
            curX = x1;
            curY = y1;
            for(auto &i: path)
            {
                switch(i)
                {
                case 'U':
                    curY--;
                    break;
                case 'D':
                    curY++;
                    break;
                case 'R':
                    curX++;
                    break;
                case 'L':
                    curX--;
                    break;
                }
                if(tiles[curX][curY] == MapTile::not_set)
                {
                    numPassableTiles++;
                    tiles[curX][curY] = MapTile::connection;
                    initialTimeTillCollapse[curX][curY] = timeTillCollapse[curX][curY] = 2;
                    if(isPassableTile(curX-1, curY))
                        dsu.combine(curX + curY * getNumColumns(), (curX - 1) + curY * getNumColumns());
                    if(isPassableTile(curX+1, curY))
                        dsu.combine(curX + curY * getNumColumns(), (curX + 1) + curY * getNumColumns());
                    if(isPassableTile(curX, curY-1))
                        dsu.combine(curX + curY * getNumColumns(), curX + (curY - 1) * getNumColumns());
                    if(isPassableTile(curX, curY+1))
                        dsu.combine(curX + curY * getNumColumns(), curX + (curY + 1) * getNumColumns());
                }
            }
        }
    }
    std::vector<pii> toWall;
    for(int i=0; i<getNumColumns(); i++)
        for(int j=0; j<getNumRows(); j++)
            if(tiles[i][j] == MapTile::not_set)
                for(int a=-1; a<=1; a++)
                    for(int b=-1; b<=1; b++)
                        if(isInMap(i+a, j+b) && tiles[i+a][j+b] != MapTile::not_set)
                            toWall.emplace_back(i, j);
    for(auto &i: toWall)
        tiles[i.A][i.B] = MapTile::wall;
    std::vector<pii> toBlank;
    for(int i=0; i<getNumColumns(); i++)
        for(int j=0; j<getNumRows(); j++)
            if(tiles[i][j] == MapTile::wall)
            {
                bool turnToBlank = true;
                for(int a=-1; a<=1; a++)
                    for(int b=-1; b<=1; b++)
                        if(isInMap(i+a, j+b) && isPassableTile(i+a, j+b))
                            turnToBlank = false;
                if(turnToBlank)
                    toBlank.emplace_back(i, j);

            }
    for(auto &i: toBlank)
        tiles[i.A][i.B] = MapTile::blank;
    for(auto &i: tiles)
        for(auto &j: i)
            if(j == MapTile::not_set)
                j = MapTile::blank;
    cout << "took " << getTicksS() - t << "s to generate a map\n";
}
static uint8_t rand_col(uint8_t c)
{
    return std::min(255, (int)(c * (0.5 + randf() * 1)));
}
static void add_background_rect(int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    double priority = 1 + std::sqrt(10 * randf());
    background.addRect(priority, 5 + randf() * 10, w * randf(), h * randf(), (0.1 * (randf() + 0.1) * w),
                    (0.1 * (randf() + 0.1)) * h, r / priority, g / priority, b / priority, randf() * 255.99);
}
//CODE_WARNING: the background texture might not be big enough to cover giant maps
void GameMap::create_background_texture(uint8_t r, uint8_t g, uint8_t b)
{
    int w = getDisplayW() * 3;
    int h = getDisplayH() * 3;
    background.clear();
    background.addClearScreen(INF, INF, r, g, b);
    for(int i=0; i<200; i++)
    {
        add_background_rect(w, h, r, g, b);
    }
    /*if(background_texture == nullptr)
        SDL_DestroyTexture(background_texture);
    background_texture = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    setRenderTarget(background_texture);
    SDL_SetTextureBlendMode(background_texture, SDL_BLENDMODE_BLEND);
    renderClear(r, g, b);
    for(int i=0; i<100; i++)
    {
        SDL_Rect rect{(int)(randf() * w), (int)(randf() * h), (int)(0.1 * (randf() + 0.1) * w), (int)(0.1 * (randf() + 0.1) * h)};
        fillRect(&rect, std::min(255, (int)(r * (0.5 + randf() * 1.5))), std::min(255, (int)(g * (0.5 + randf() * 1.5))),
                  std::min(255, (int)(b * (0.5 + randf() * 1.5))), randf() * 255);
    }
    setRenderTarget(NULL);*/

}
void GameMap::generateMapFloor1()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[0];
    current_dirt_sprite = dirt_sprites[0];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    generateRandomRoomArrangement(60, 60, 0.3);
}
void GameMap::generateMapFloor2()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[0];
    current_dirt_sprite = dirt_sprites[0];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    setMapSize(11, 11);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::hurt;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::hurt;
    startPositionX = 1;
    startPositionY = 1;
    tiles[startPositionX][startPositionY] = MapTile::portal;
    switch(randz(0, 1))
    {
    case 0:
        units.push_back(std::make_shared<Unit>(7, Affiliation::generic_enemy, 7, 7));
        break;
    case 1:
        units.push_back(std::make_shared<Unit>(8, Affiliation::generic_enemy, 7, 7));
        break;
    }
}
void GameMap::generateMapFloor3()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[1];
    current_dirt_sprite = dirt_sprites[1];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    switch(randz(0, 1))
    {
    case 0:
        setMapSize(31, 31);
        generateFloorGeneric(11, 0.48);
        generateFloorEnemies(0.06);
        break;
    case 1:
        setMapSize(31, 28);
        generateFloorMaze(NOT_SET, 16, 0, 0.5);
        generateFloorEnemies(0.03);
        break;
    }

}
void GameMap::generateMapFloor4()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[1];
    current_dirt_sprite = dirt_sprites[1];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    setMapSize(11, 11);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::hurt;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::hurt;
    startPositionX = 1;
    startPositionY = 1;
    tiles[startPositionX][startPositionY] = MapTile::portal;
    switch(randz(0, 1))
    {
    case 0:
        units.push_back(std::make_shared<Unit>(9, Affiliation::generic_enemy, 7, 7));
        break;
    case 1:
        units.push_back(std::make_shared<Unit>(17, Affiliation::generic_enemy, 7, 7));
        break;
    }
}
void GameMap::generateMapFloor5()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[2];
    current_dirt_sprite = dirt_sprites[2];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    switch(randz(0, 2))
    {
    case 0:
        setMapSize(32, 22);
        generateFloorGeneric(11, 0.47, NOT_SET, 0.05);
        generateFloorEnemies(0.06);
        break;
    case 1:
        setMapSize(31, 31);
        generateFloorMaze(NOT_SET, 12, 0, 0.4);
        generateFloorEnemies(0.03);
        break;
    case 2:
        setMapSize(22, 22);
        generateFloorCollapsing(1.9);
        generateFloorEnemies(0.05);
        break;
    }
}
void GameMap::generateMapFloor6()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[2];
    current_dirt_sprite = dirt_sprites[2];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    setMapSize(12, 12);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::hurt;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::hurt;
    startPositionX = 1;
    startPositionY = 1;
    tiles[startPositionX][startPositionY] = MapTile::portal;
    switch(randz(3, 3))
    {
    case 0:
        units.push_back(std::make_shared<Unit>(15, Affiliation::generic_enemy, 7, 7));
        break;
    case 1:
        units.push_back(std::make_shared<Unit>(16, Affiliation::generic_enemy, 7, 7));
        break;
    case 2:
        units.push_back(std::make_shared<Unit>(43, Affiliation::generic_enemy, 7, 7));
        break;
    case 3:
        units.push_back(std::make_shared<Unit>(53, Affiliation::generic_enemy, 7, 7));
        break;
    }
}
void GameMap::generateMapFloor7()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[3];
    current_dirt_sprite = dirt_sprites[3];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    switch(randz(0, 2))
    {
    case 0:
        setMapSize(32, 22);
        generateFloorGeneric(6, 0.47, NOT_SET, 0.07);
        generateFloorEnemies(0.08);
        break;
    case 1:
        setMapSize(31, 31);
        generateFloorMaze(NOT_SET, 12, 0, 0.31);
        generateFloorEnemies(0.04);
        break;
    case 2:
        setMapSize(22, 22);
        generateFloorCollapsing(1.7, 0.05);
        generateFloorEnemies(0.05);
        break;
    }
}
void GameMap::generateMapFloor8()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[3];
    current_dirt_sprite = dirt_sprites[3];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    setMapSize(12, 12);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::hurt;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::hurt;
    startPositionX = 1;
    startPositionY = 1;
    tiles[startPositionX][startPositionY] = MapTile::portal;
    switch(randz(0, 1))
    {
    case 0:
        units.push_back(std::make_shared<Unit>(24, Affiliation::generic_enemy, 8, 8));
        break;
    case 1:
        units.push_back(std::make_shared<Unit>(8, Affiliation::generic_enemy, 9, 9));
        units.push_back(std::make_shared<Unit>(25, Affiliation::generic_enemy, 3, 9));
        break;
    }
}
void GameMap::generateMapFloor9()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[4];
    current_dirt_sprite = dirt_sprites[4];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    switch(randz(0, 2))
    {
    case 0:
        setMapSize(34, 23);
        generateFloorGeneric(12, 0.45, NOT_SET, 0.09);
        generateFloorEnemies(0.08);
        break;
    case 1:
        setMapSize(34, 31);
        generateFloorMaze(NOT_SET, 11, 0.05, 0.28);
        generateFloorEnemies(0.035);
        break;
    case 2:
        setMapSize(23, 23);
        generateFloorCollapsing(1.6, 0.09);
        generateFloorEnemies(0.068);
        break;
    }
}
void GameMap::generateMapFloor10()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[4];
    current_dirt_sprite = dirt_sprites[4];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    setMapSize(13, 13);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::hurt;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::hurt;
    startPositionX = 1;
    startPositionY = 1;
    tiles[startPositionX][startPositionY] = MapTile::portal;
    switch(randz(0, 1))
    {
    case 0:
        units.push_back(std::make_shared<Unit>(18, Affiliation::generic_enemy, 8, 8));
        break;
    case 1:
        units.push_back(std::make_shared<Unit>(19, Affiliation::generic_enemy, 8, 8));
        break;
    }
}
void GameMap::generateMapFloor11()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[5];
    current_dirt_sprite = dirt_sprites[5];
    current_hurt_sprite = hurt_sprites[0];
    current_portal_closed_sprite = portal_closed_sprites[0];
    current_portal_open_sprite = portal_open_sprites[0];
    current_collapse_sprite = collapse_sprites[0];
    current_slip_sprite = slip_sprites[0];
    switch(randz(0, 3))
    {
    case 0:
        setMapSize(37, 25);
        generateFloorGeneric(14, 0.43, NOT_SET, 0.1);
        generateFloorEnemies(0.08);
        break;
    case 1:
        setMapSize(40, 31);
        generateFloorMaze(NOT_SET, 8, 0.06, 0.26);
        generateFloorEnemies(0.045);
        break;
    case 2:
        setMapSize(24, 24);
        generateFloorCollapsing(1.5, 0.1);
        generateFloorEnemies(0.055);
        break;
    case 3:
        setMapSize(31, 31);
        generateFloorGeneric(14, 0.45, 2.7, 0.1);
        generateFloorEnemies(0.055);
        break;
    }
}
void GameMap::generateMapFloor12()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[8];
    current_dirt_sprite = blank_sprite;
    current_hurt_sprite = hurt_sprites[1];
    current_portal_closed_sprite = portal_closed_sprites[1];
    current_portal_open_sprite = portal_open_sprites[1];
    current_collapse_sprite = collapse_sprites[1];
    current_slip_sprite = slip_sprites[1];
    setMapSize(13, 13);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::hurt;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::hurt;
    startPositionX = 1;
    startPositionY = 1;
    tiles[startPositionX][startPositionY] = MapTile::portal;
    switch(randz(2, 2))
    {
    case 0:
        units.push_back(std::make_shared<Unit>(29, Affiliation::generic_enemy, 8, 8));
        break;
    case 1:
        units.push_back(std::make_shared<Unit>(30, Affiliation::generic_enemy, 8, 8));
        break;
    case 2:
        for(int i=6; i<=10; i+=2)
            for(int j=6; j<=10; j+=2)
                units.push_back(std::make_shared<Unit>(27, Affiliation::generic_enemy, i, j));
        break;
    }
}
void GameMap::generateMapFloor13()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[8];
    current_dirt_sprite = blank_sprite;
    current_hurt_sprite = hurt_sprites[1];
    current_portal_closed_sprite = portal_closed_sprites[1];
    current_portal_open_sprite = portal_open_sprites[1];
    current_collapse_sprite = collapse_sprites[1];
    current_slip_sprite = slip_sprites[1];
    switch(randz(1, 6))
    {
    case 1:
        setMapSize(43, 34);
        generateFloorMaze(NOT_SET, 8, 0.1);
        generateFloorEnemies(0.028);
        break;
    case 2:
        setMapSize(25, 25);
        generateFloorCollapsing(1.4, 0.1);
        generateFloorEnemies(0.039);
        break;
    case 3:
        setMapSize(33, 33);
        generateFloorGeneric(14, 0.5, 2.5, 0.1);
        generateFloorEnemies(0.04);
        break;
    case 4:
    case 5:
    case 6:
        setMapSize(40, 31);
        generateFloorMaze(3.9, 40);
        generateFloorEnemies(0.023);
        break;
    }
}
void GameMap::generateMapFloor14()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[8];
    current_dirt_sprite = blank_sprite;
    current_hurt_sprite = hurt_sprites[1];
    current_portal_closed_sprite = portal_closed_sprites[1];
    current_portal_open_sprite = portal_open_sprites[1];
    current_collapse_sprite = collapse_sprites[1];
    current_slip_sprite = slip_sprites[1];
    setMapSize(15, 15);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::hurt;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::hurt;
    startPositionX = 1;
    startPositionY = 1;
    tiles[startPositionX][startPositionY] = MapTile::portal;
    switch(randz(0, 2))
    {
    case 0:
        units.push_back(std::make_shared<Unit>(35, Affiliation::generic_enemy, 10, 10));
        break;
    case 1:
        units.push_back(std::make_shared<Unit>(36, Affiliation::generic_enemy, 10, 10));
        break;
    case 2:
        units.push_back(std::make_shared<Unit>(44, Affiliation::generic_enemy, 10, 10));
        break;
    }
}
void GameMap::generateMapFloor15()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[8];
    current_dirt_sprite = blank_sprite;
    current_hurt_sprite = hurt_sprites[1];
    current_portal_closed_sprite = portal_closed_sprites[1];
    current_portal_open_sprite = portal_open_sprites[1];
    current_collapse_sprite = collapse_sprites[1];
    current_slip_sprite = slip_sprites[1];
    switch(randz(1, 6))
    {
    case 1:
        setMapSize(52, 37);
        generateFloorMaze(NOT_SET, 8, 0.1);
        generateFloorEnemies(0.028);
        break;
    case 2:
        setMapSize(34, 34);
        generateFloorCollapsing(1.5, 0.1);
        generateFloorEnemies(0.038);
        break;
    case 3:
        setMapSize(39, 39);
        generateFloorGeneric(14, 0.5, 2.2, 0.1);
        generateFloorEnemies(0.049);
        break;
    case 4:
    case 5:
    case 6:
        setMapSize(49, 34);
        generateFloorMaze(3.7, 50, 0.06, 0.26);
        generateFloorEnemies(0.023);
        break;
    }
}
void GameMap::generateMapFloor16()
{
    current_blank_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[8];
    current_dirt_sprite = blank_sprite;
    current_hurt_sprite = hurt_sprites[1];
    current_portal_closed_sprite = portal_closed_sprites[1];
    current_portal_open_sprite = portal_open_sprites[1];
    current_collapse_sprite = collapse_sprites[1];
    current_slip_sprite = slip_sprites[1];
    setMapSize(15, 15);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::hurt;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::hurt;
    startPositionX = 1;
    startPositionY = 1;
    tiles[startPositionX][startPositionY] = MapTile::portal;
    switch(randz(0, 2))
    {
    case 0:
        units.push_back(std::make_shared<Unit>(35, Affiliation::generic_enemy, 10, 10));
        for(int i=0; i<2; i++)
        {
            units.push_back(std::make_shared<Unit>(51, Affiliation::generic_enemy, NOT_SET, NOT_SET));
            units.back()->misc_var[0] = 0.7;
            units.back()->misc_var[1] = PI/4 + i * PI;
            units.back()->owner = units[units.size() - i - 2];
        }
        break;
    case 1:
        units.push_back(std::make_shared<Unit>(36, Affiliation::generic_enemy, 10, 10));
        for(int i=0; i<2; i++)
        {
            units.push_back(std::make_shared<Unit>(51, Affiliation::generic_enemy, NOT_SET, NOT_SET));
            units.back()->misc_var[0] = 1.5;
            units.back()->misc_var[1] = PI/10 + i * PI/1.25;
            units.back()->owner = units[units.size() - i - 2];
        }
        break;
    case 2:
        units.push_back(std::make_shared<Unit>(44, Affiliation::generic_enemy, 10, 10));
        for(int i=0; i<2; i++)
        {
            units.push_back(std::make_shared<Unit>(51, Affiliation::generic_enemy, NOT_SET, NOT_SET));
            units.back()->misc_var[0] = 1.1;
            units.back()->misc_var[1] = PI/4 + i * PI;
            units.back()->owner = units[units.size() - i - 2];
        }
        break;
    }
}
void GameMap::generateMapFloor17()
{
    current_blank_sprite = blank_sprite;
    current_dirt_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[8];
    setMapSize(15, 15);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::wall;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::wall;
    startPositionX = 1;
    startPositionY = 1;
    units.push_back(std::make_shared<Unit>(31, Affiliation::generic_enemy, 10, 10));
}
void GameMap::generateMapFloor18()
{
    current_blank_sprite = blank_sprite;
    current_dirt_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[8];
    setMapSize(21, 30);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::wall;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::wall;
    for(int i=25; i>=8; i-=3)
    {
        if(i%6 == 1)
        {
            for(int j=1; j<=4; j++)
                tiles[j][i] = MapTile::wall;
            for(int j=16; j<=20; j++)
                tiles[j][i] = MapTile::wall;
        }
        else for(int j=9; j<=11; j++)
            tiles[j][i] = MapTile::wall;
    }
    units.push_back(std::make_shared<Unit>(55, Affiliation::generic_enemy, 2, 2));
    startPositionX = 1;
    startPositionY = getNumRows() - 2;
    //units.push_back(std::make_shared<Unit>(31, Affiliation::generic_enemy, 10, 5));
}
void GameMap::generateMapFloor19()
{
    current_blank_sprite = blank_sprite;
    current_dirt_sprite = blank_sprite;
    current_wall_sprite = wall_sprites[8];
    setMapSize(21, 30);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = MapTile::wall;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = MapTile::wall;
    for(int i=25; i>=8; i-=3)
    {
        if(i%6 == 1)
        {
            for(int j=1; j<=4; j++)
                tiles[j][i] = MapTile::wall;
            for(int j=16; j<=20; j++)
                tiles[j][i] = MapTile::wall;
        }
        else for(int j=9; j<=11; j++)
            tiles[j][i] = MapTile::wall;
    }
    units.push_back(std::make_shared<Unit>(55, Affiliation::generic_enemy, 2, 2));
    startPositionX = 1;
    startPositionY = getNumRows() - 2;
    //units.push_back(std::make_shared<Unit>(31, Affiliation::generic_enemy, 10, 5));
}
static SDL_Color background_color[GameMap::NUM_FLOORS]{
    /* 0*/ {},
    /* 1*/ {110, 40, 40},
    /* 2*/ {110, 40, 40}
};
void GameMap::generateFloor(GameState &game_state)
{
    println("generating a map for floor " + to_str(floor));
    int tries = 0;
    do
    {
        endFloor();
        tries++;
        switch(floor)
        {
        case 1: //Red Room
            generateMapFloor1();
            break;
        case 2: //Red Room Boss
            generateMapFloor2();
            break;
        case 3: //Blue Room
            generateMapFloor3();
            break;
        case 4: //Blue Room Boss
            generateMapFloor4();
            break;
        case 5: //Yellow Room
            generateMapFloor5();
            break;
        case 6: //Yellow Room Boss
            generateMapFloor6();
            break;
        case 7: //Purple Room
            generateMapFloor7();
            break;
        case 8: //Purple Room Boss
            generateMapFloor8();
            break;
        case 9: //Green Room
            generateMapFloor9();
            break;
        case 10: //Green Room Boss
            generateMapFloor10();
            break;
        case 11: //Orange Room
            generateMapFloor11();
            break;
        case 12: //Orange Room Boss
            generateMapFloor12();
            break;
        case 13: //BW Room 1
            generateMapFloor13();
            break;
        case 14: //BW Room 1 Boss
            generateMapFloor14();
            break;
        case 15: //BW Room 2
            generateMapFloor15();
            break;
        case 16: //BW Room 2
            generateMapFloor16();
            break;
        case 17: //Kworker
            generateMapFloor17();
            break;
        case 18: //US Army
            generateMapFloor18();
            break;
        case 19: //US Marines
            generateMapFloor18();
            break;
        default:
            print_error("cannot create map for unknown floor " + to_str(floor));
            break;
        }
    }
    while(floor%2!=0 && floor<=16 && !isValidRoom());
    //stuff that only needs to be done once
    auto p = game_state.getPlayer();
    auto pshape = static_cast<Square*>(p->shape.get());
    p->type = 0;
    pshape->setSideLength(0.7);
    switch(floor)
    {
    case 17:
        p->type = 32;
        p->sprite = Unit::sprites[32];
        pshape->setSideLength(0.6);
        break;
    case 18:
        gravity = 85;
        p->type = 32;
        p->sprite = Unit::sprites[32];
        pshape->setSideLength(0.6);
        break;
    case 19:
        gravity = 85;
        p->type = 32;
        p->sprite = Unit::sprites[32];
        pshape->setSideLength(0.6);
        break;
    default:
        create_background_texture(background_color[floor].r, background_color[floor].g, background_color[floor].b);
        break;
    }
    println("Took " + to_str(tries) + " attempt(s) to generate a valid floor");
}
static void render_copy_explored(SDL_Texture *t, double t_since_explored, int x, int y, int w, int h, double amult = 1)
{
    uint8_t alpha;
    if(t_since_explored == NOT_SET || t_since_explored > 0.2)
        alpha = 255;
    else alpha = std::min(255, (int)(255 * 5 * t_since_explored));
    alpha *= amult;
    SDL_SetTextureAlphaMod(t, alpha);
    renderCopy(t, x, y, w, h);
}
void __attribute__((hot)) GameMap::draw_tile(MapTile tile, int i, int j, int x, int y, int w, int h)
{
    if(time_since_explored[i][j] == NOT_SET)
    {
        return;
    }
    switch(tile)
    {
    case MapTile::blank:
        //renderCopy(current_blank_sprite, x, y, w, h);
        break;
    case MapTile::dirt:
    case MapTile::connection:
        if(current_dirt_sprite == nullptr)
            render_copy_explored(dirt_sprites[(int)((1 + std::sin(73 * i + 10003 * j + getTicksS())) * (NUM_DIRT_SPRITES-0.001)/2.0)],
                                 time_since_explored[i][j], x, y, w, h);
        else render_copy_explored(current_dirt_sprite, time_since_explored[i][j], x, y, w, h);
        if(timeTillCollapse[i][j] != NOT_SET)
        {
            //SDL_SetTextureAlphaMod(current_collapse_sprite, 255 - 200 * timeTillCollapse[i][j] / initialTimeTillCollapse[i][j]);
            render_copy_explored(current_collapse_sprite, time_since_explored[i][j], x, y, w, h,
                                 1 - 0.8 * timeTillCollapse[i][j] / initialTimeTillCollapse[i][j]);
            //we don't need to change the alpha back because the collapse sprite is always rendered by first changing the alpha mod
        }
        break;
    case MapTile::wall:
        render_copy_explored(current_wall_sprite, time_since_explored[i][j], x, y, w, h);
        break;
    case MapTile::hurt:
        if(current_dirt_sprite == nullptr)
            render_copy_explored(dirt_sprites[(int)((1 + std::sin(73 * i + 10003 * j + getTicksS())) * (NUM_DIRT_SPRITES-0.001)/2.0)],
                                 time_since_explored[i][j], x, y, w, h);
        else render_copy_explored(current_dirt_sprite, time_since_explored[i][j], x, y, w, h);
        render_copy_explored(current_hurt_sprite, time_since_explored[i][j], x, y, w, h);
        break;
    case MapTile::portal:
        if(current_dirt_sprite == nullptr)
            render_copy_explored(dirt_sprites[(int)((1 + std::sin(73 * i + 10003 * j + getTicksS())) * (NUM_DIRT_SPRITES-0.001)/2.0)],
                                 time_since_explored[i][j], x, y, w, h);
        else render_copy_explored(current_dirt_sprite, time_since_explored[i][j], x, y, w, h);
        if(isFloorFinished())
            render_copy_explored(current_portal_open_sprite, time_since_explored[i][j], x, y, w, h);
        else render_copy_explored(current_portal_closed_sprite, time_since_explored[i][j], x, y, w, h);
        break;
    case MapTile::slip:
        if(current_dirt_sprite == nullptr)
            render_copy_explored(dirt_sprites[(int)((1 + std::sin(73 * i + 10003 * j + getTicksS())) * (NUM_DIRT_SPRITES-0.001)/2.0)],
                                 time_since_explored[i][j], x, y, w, h);
        else render_copy_explored(current_dirt_sprite, time_since_explored[i][j], x, y, w, h);
        render_copy_explored(current_slip_sprite, time_since_explored[i][j], x, y, w, h);
        break;
    default:
        print_warning("asked to draw map tile of unknown type");
        break;
    }
}
void GameMap::draw(const GameState &game_state)
{
    int FW = game_state.getFloorDisplayW();
    int FH = game_state.getFloorDisplayH();
    fillRect(0, 0, FW, FH, 0, 0, 0);
    int ppt = getPixelsPerTile();
    int minx = std::max(0, (int)(getCameraX() - FW/2));
    int maxx = std::min(getNumColumns()-1, (int)(getCameraX() + FW/2));
    int miny = std::max(0, (int)(getCameraY() - FH/2));
    int maxy = std::min(getNumRows()-1, (int)(getCameraX() + FH/2));
    double tim = getTicksS();
    //int xpos = (int)(0.02 * getTicksS() * FW) % game_state.getFloorDisplayW(); //auto-scrolling makes me dizzy
    int xpos = (int) (0.02 * FW * (game_state.getPlayer()->getX() + 10));
    int ypos = (int) (0.02 * FH * (game_state.getPlayer()->getY() + 10));
    /*SDL_Rect src{xpos, ypos, FW, FH};
    SDL_Rect dst{0, 0, FW, FH};
    renderCopy(background_texture, &src, &dst);*/
    background.update();
    background.draw(-xpos, -ypos);
    if(floor <= 11 && randf() < 20.0 / BASE_FPS)
    {
        add_background_rect(getDisplayW() * 3, getDisplayH() * 3, background_color[floor].r, background_color[floor].g, background_color[floor].b);
    }
    if(floor == 11) //insert glitches
    {
        static double lastCheckTime = 0; //only switch up to once per second
        double tleftPct = floorTimeLeft / (double)BASE_FLOOR_TIME[floor];
        if(tim - lastCheckTime > 1.4 && tleftPct < 0.8)
        {
            lastCheckTime = tim;
            if(randf() < tleftPct * 5 - 3)
            {
                current_wall_sprite = wall_sprites[5];
                current_dirt_sprite = nullptr; //render whatever
                current_hurt_sprite = hurt_sprites[0];
                current_portal_closed_sprite = portal_closed_sprites[0];
                current_portal_open_sprite = portal_open_sprites[0];
                current_collapse_sprite = collapse_sprites[0];
                current_slip_sprite = slip_sprites[0];
            }
            else
            {
                current_wall_sprite = wall_sprites[8];
                current_dirt_sprite = blank_sprite;
                current_hurt_sprite = hurt_sprites[1];
                current_portal_closed_sprite = portal_closed_sprites[1];
                current_portal_open_sprite = portal_open_sprites[1];
                current_collapse_sprite = collapse_sprites[1];
                current_slip_sprite = slip_sprites[1];
            }
            generateMinimapTexture(game_state);
        }
    }
    static SDL_Texture *wall_borders[16]{};
    static int curPPT = NOT_SET;
    if(curPPT != ppt) //scaling adjusted, recreate these
    {
        if(curPPT != NOT_SET) //it's the first time - they haven't been created yet
            for(auto &i: wall_borders)
                SDL_DestroyTexture(i);
        curPPT = ppt;
        setColor(255, 255, 255);
        for(int i=0; i<16; i++)
        {
            int w = curPPT, h = curPPT;
            wall_borders[i] = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
            setRenderTarget(wall_borders[i]);
            SDL_SetTextureBlendMode(wall_borders[i], SDL_BLENDMODE_BLEND);
            if((i>>0) & 1)
                fillRect(w * 0.9, 0, w * 0.1, h);
            if((i>>1) & 1)
                fillRect(0, 0, w * 0.1, h);
            if((i>>2) & 1)
                fillRect(0, h * 0.9, w, h * 0.1);
            if((i>>3) & 1)
                fillRect(0, 0, w, h * 0.1);
        }
        setRenderTarget(NULL);
    }
    for(int i=minx; i<=maxx; i++)
    {
        for(int j=miny; j<=maxy; j++)
        {
            double x = i - cameraX - 0.5;
            double y = j - cameraY - 0.5;
            draw_tile(tiles[i][j], i, j, x*ppt, y*ppt, ppt, ppt);
            //more graphical effects: wall tiles have a nice border (it's probably not worth drawing this on the minimap)
            //we have to use textures or else some corners will be double filled
            if(time_since_explored[i][j]!=NOT_SET && tiles[i][j] == MapTile::wall)
            {
                int to_render = 0;
                if(isPassableTile(i+1, j))
                    to_render |= 1<<0;
                if(isPassableTile(i-1, j))
                    to_render |= 1<<1;
                if(isPassableTile(i, j+1))
                    to_render |= 1<<2;
                if(isPassableTile(i, j-1))
                    to_render |= 1<<3;
                uint8_t alpha = 100;
                SDL_SetTextureAlphaMod(wall_borders[to_render], alpha);
                renderCopy(wall_borders[to_render], x*ppt, y*ppt, ppt, ppt);
                //fill corners
                if(isPassableTile(i+1, j+1) && tiles[i][j+1]==MapTile::wall && tiles[i+1][j]==MapTile::wall)
                    fillRect((x+0.9)*ppt, (y+0.9)*ppt, 0.1*ppt, 0.1*ppt, 255, 255, 255, alpha);
                if(isPassableTile(i-1, j+1) && tiles[i][j+1]==MapTile::wall && tiles[i-1][j]==MapTile::wall)
                    fillRect(x*ppt, (y+0.9)*ppt, 0.1*ppt, 0.1*ppt, 255, 255, 255, alpha);
                if(isPassableTile(i+1, j-1) && tiles[i][j-1]==MapTile::wall && tiles[i+1][j]==MapTile::wall)
                    fillRect((x+0.9)*ppt, y*ppt, 0.1*ppt, 0.1*ppt, 255, 255, 255, alpha);
                if(isPassableTile(i-1, j-1) && tiles[i][j-1]==MapTile::wall && tiles[i-1][j]==MapTile::wall)
                    fillRect(x*ppt, y*ppt, 0.1*ppt, 0.1*ppt, 255, 255, 255, alpha);
            }
            if(time_since_explored[i][j]!=-1 && time_since_explored[i][j] < 0.25)
                time_since_explored[i][j] += 1.0 / BASE_FPS;
        }
    }
}
const static double BOUNDARY_FRACTION = 0.05;
double GameMap::getMinimapPixelsPerTile() const
{
    int w;
    SDL_QueryTexture(minimap_texture, NULL, NULL, &w, NULL);
    return w / (double)tiles.size();
}
void GameMap::getMinimapSize(int *w, int *h) const
{
    SDL_QueryTexture(minimap_texture, NULL, NULL, w, h);
}
void GameMap::getMinimapRect(const GameState &game_state, int *_x, int *_y, int *_w, int *_h) const
{
    //int x = game_state.getHudStartX();
    int x = 0;
    int w = game_state.getHudW();
    int BOUNDARY_SIZE = w*BOUNDARY_FRACTION;
    //x += BOUNDARY_SIZE;
    w -= 2*BOUNDARY_SIZE;
    int h;
    if(getNumColumns() == 0)
    {
        print_warning("there are 0 columns, can't get minimap rect");
        h = w;
    }
    else h = w * getNumRows() / getNumColumns();
    //int y = getWindowH() - h - BOUNDARY_SIZE;
    int y = 0;
    if(_x != NULL)
        *_x = x;
    if(_y != NULL)
        *_y = y;
    if(_w != NULL)
        *_w = w;
    if(_h != NULL)
        *_h = h;
}
/*void GameMap::generateExploredTexture(const GameState &game_state)
{
    int w, h;
    getMinimapRect(game_state, NULL, NULL, &w, &h);
    explored_texture = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetRenderTarget(getRenderer(), explored_texture);
    SDL_SetTextureBlendMode(explored_texture, SDL_BLENDMODE_BLEND);
    renderClear(0, 0, 0, 255);
    SDL_SetRenderTarget(getRenderer(), NULL);
    explored.resize(w, std::vector<bool>(h, false));
}*/
void GameMap::generateMinimapTexture(const GameState &game_state)
{
    if(getNumColumns() == 0) //the window was resized but the map is empty
        return;
    int w, h;
    getMinimapRect(game_state, NULL, NULL, &w, &h);
    if(minimap_texture != NULL)
        SDL_DestroyTexture(minimap_texture);
    minimap_texture = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetRenderTarget(getRenderer(), minimap_texture);
    SDL_SetTextureBlendMode(minimap_texture, SDL_BLENDMODE_BLEND);
    double wEach = w / (double)getNumColumns();
    double hEach = h / (double)getNumRows();
    for(int i=0; i<getNumColumns(); i++)
    {
        for(int j=0; j<getNumRows(); j++)
        {
            int pixelW = (int)(wEach*(i+1)) - (int)(wEach*i);
            int pixelH = (int)(hEach*(j+1)) - (int)(hEach*j);
            int pixelX = i*wEach, pixelY = j*hEach;
            draw_tile(tiles[i][j], i, j, pixelX, pixelY, pixelW, pixelH);
        }
    }
    SDL_SetRenderTarget(getRenderer(), NULL);
}
void GameMap::drawMinimap(const GameState &game_state)
{
    if(floor >= 17) //don't draw the minimap on later floors
        return;
    int x, y, w, h;
    getMinimapRect(game_state, &x, &y, &w, &h);
    //int origY = y - (getWindowH() - (y+h));
    //fillRect(game_state.getHudStartX(), origY, game_state.getHudW(), getWindowH() - origY, 100, 100, 100);
    if(minimap_texture == NULL) //due to window resize
    {
        println("window resized, recreating minimap texture");
        generateMinimapTexture(game_state);
    }
    int bsize = game_state.getHudBoundarySize();
    fillRect(0, 0, w + 2 * bsize, bsize, 0, 0, 0, 100);
    fillRect(0, bsize, bsize, h, 0, 0, 0, 100);
    fillRect(w + bsize, bsize, bsize, h, 0, 0, 0, 100);
    fillRect(0, h + bsize, w + 2 * bsize, bsize, 0, 0, 0, 100);
    fillRect(bsize, bsize, w, h, 0, 0, 0, 50);
    x += bsize;
    y += bsize;
    renderCopy(minimap_texture, x, y, w, h);
    double eppt = game_state.game_map.getMinimapPixelsPerTile();
    double scale = SPRITE_SCALE * eppt / getPixelsPerTile();
    for(auto &i: units)
    {
        if(!i->isDrawable(game_state)) //it's in a room that hasn't already been found
            continue;
        int rw, rh;
        SDL_QueryTexture(i->sprite, NULL, NULL, &rw, &rh);
        if(!i->isPlayer())
            renderCopyEx(i->sprite, x + eppt * i->getX() - rw * scale / 2 + eppt/2, y + eppt * i->getY() - rh * scale / 2 + eppt/2,
                         rw * scale, rh * scale, TO_DEG(i->getAngle()));
    }
    //renderCopy(explored_texture, x, y, w, h);
    fillRect(x + eppt*game_state.getPlayer()->getX(), y + eppt*game_state.getPlayer()->getY(), eppt, eppt, 255, 255, 255);
}
//currently vision distance is treated as an integer. This may change later but it probably won't
void GameMap::update_explored(const std::shared_ptr<Player> &player, double vision_distance)
{
    std::vector<std::vector<bool> > vis(getNumColumns(), std::vector<bool>(getNumRows(), false));
    std::queue<pii> cur;
    cur.emplace(player->getX() + 0.5, player->getY() + 0.5);
    for(int d=0; !cur.empty() && d<vision_distance; d++)
    {
        std::queue<pii> nxt;
        while(!cur.empty())
        {
            int x = cur.front().A;
            int y = cur.front().B;
            cur.pop();
            if(isInMap(x, y) && !vis[x][y])
            {
                vis[x][y] = true;
                if(time_since_explored[x][y] == NOT_SET)
                    time_since_explored[x][y] = 0;
                if(!isPassableTile(x, y))
                    continue;
                //only discover rooms if they're within half of your vision distance
                if(d < vision_distance/2 && belongs_to_room[x][y] != NOT_SET && !found_rooms[belongs_to_room[x][y]])
                {
                    found_rooms[belongs_to_room[x][y]] = true;
                    for(int i=0; i<getNumColumns(); i++)
                        for(int j=0; j<getNumRows(); j++)
                            if(time_since_explored[i][j]==NOT_SET && belongs_to_room[i][j]==belongs_to_room[x][y])
                                time_since_explored[i][j] = 0;
                    for(auto &i: units)
                        if(i->belongs_to_room == belongs_to_room[x][y])
                            i->is_active = true;
                }
                for(int a=-1; a<=1; a++)
                {
                    for(int b=-1; b<=1; b++)
                    {
                        if(a!=0 || b!=0)
                        {
                            nxt.emplace(x+a, y+b);
                        }
                    }
                }
            }
        }
        cur = std::move(nxt);
    }
}
