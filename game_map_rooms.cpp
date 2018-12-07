#include "game_map.h"
namespace
{
    enum UnitName
    {
        RedSquare = 1, YellowSquare = 2, GreenSquare = 3, OrangeSquare = 4,
        RedT = 5, Red4Star = 6, Red5Star = 7, Green4Star = 8, YellowArrow = 9,
        YellowT = 10, RedTriangle = 11, RedPentangon = 12, YellowPentagon = 13, Yellow4Star = 14,
        Red6Star = 15, Yellow5Star = 16, GreenArrow = 17, Yellow6Star = 18, Purple4Star = 19
    };
}
void GenericMapRoom::setMapSize(int numColumns, int numRows)
{
    moneyBonusOnCompletion = 0;
    tiles.clear();
    timeTillCollapse.clear();
    units.clear();
    tiles.resize(numColumns, std::vector<MapTile>(numRows, MapTile::not_set));
    timeTillCollapse.resize(numColumns, std::vector<double>(numRows, NOT_SET));
}
void GenericMapRoom::surroundWithTile(MapTile t)
{
    for(int i=0; i<getNumColumns(); i++)
        tiles[i][0] = tiles[i][getNumRows()-1] = t;
    for(int i=0; i<getNumRows(); i++)
        tiles[0][i] = tiles[getNumColumns()-1][i] = t;
}
void GenericMapRoom::fillWithTile(MapTile t)
{
    for(auto &i: tiles)
        for(auto &j: i)
            j = t;
}
void GenericMapRoom::dfsValidate(std::vector<std::vector<bool> > &visited, int x, int y) const
{
    if(isInMap(x, y) && isPassableTile(x, y) && !visited[x][y])
    {
        visited[x][y] = true;
        dfsValidate(visited, x-1, y);
        dfsValidate(visited, x+1, y);
        dfsValidate(visited, x, y-1);
        dfsValidate(visited, x, y+1);
    }
}
bool GenericMapRoom::isValidRoom() const
{
    /*int HPsum = 0;
    for(auto &i: units)
        HPsum += i->HP + i->HPregen * 4;
    if(HPsum<UNIT_HP_SUM[floor]*0.8 || HPsum>UNIT_HP_SUM[floor]*1.2) //ensures the time it takes to clear a floor if you hit every shot is relatively constant
        return false;*/
    std::vector<std::vector<bool> > visited(getNumColumns(), std::vector<bool>(getNumRows(), false));
    bool dfsAlready = false;
    for(int i=0; i<getNumColumns(); i++)
    {
        for(int j=0; j<getNumRows(); j++)
        {
            if(isPassableTile(i, j))
            {
                if(!dfsAlready)
                {
                    dfsValidate(visited, i, j);
                    dfsAlready = true;
                }
                else if(!visited[i][j])
                {
                    return false;
                }
            }
        }
    }
    return true;
}
//
Room::Room()
{
    this->weight = 0;
    this->moneyBonusOnCompletion = 0;
}
Room::Room(int numColumns, int numRows, double weight)
{
    this->weight = weight;
    this->custom_func = nullptr;
    setMapSize(numColumns, numRows);
}
void Room::generateGenericRoom(int numColumns, int numRows, int numClusters, double fillFactor, double collapse_time, double slip_tile_pct)
{
    setMapSize(numColumns, numRows);
    if(numClusters > getArea())
        print_error("number of clusters is greater than map area");
    moneyBonusOnCompletion += 0.04 * getArea() * (1 / (1 + collapse_time));
    for(auto &i: timeTillCollapse)
        for(auto &j: i)
            j = collapse_time;
    std::vector<std::vector<bool> > alreadyUsed(getNumColumns(), std::vector<bool>(getNumRows(), false));
    std::vector<pii> startingPoints;
    custom_hash_set<pii> frontier;
    int iter = 0;
    for(int i=0; i<numClusters; i++) //set "seed" spots
    {
        if(iter++ > 1000)
            print_warning("possible infinite loop: 1000 iterations when seeding starting points");
        int x = randz(0, getNumColumns()-1), y = randz(0, getNumRows()-1);
        if(tiles[x][y] == MapTile::not_set)
        {
            tiles[x][y] = MapTile::dirt;
            startingPoints.emplace_back(x, y);
            frontier.insert(mp(x, y));
        }
        else
        {
            i--;
            continue;
        }
    }
    //connect the "seeds" so that the map is connected
    DisjointSet connections(numClusters);
    iter = 0;
    for(int i=1; i<(int)startingPoints.size(); i++)
    {
        if(iter++ > 1000)
            print_warning("possible infinite loop: 1000 iterations when connecting starting points");
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
            if(!isInMap(curX, curY))
            {
                bad = true;
                break;
            }
        }
        if(bad)
        {
            i--;
            continue;
        }
        curX = prv.A;
        curY = prv.B;
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
    int counter = getArea() * fillFactor; //how many more tiles do we have to fill?
    for(auto &i: tiles)
        for(auto &j: i)
            if(j != MapTile::not_set)
                counter--;
    if(counter < 0)
        print_warning("map is overfilled, counter = " + to_str(counter));
    std::vector<std::vector<bool> > vis(getNumColumns(), std::vector<bool>(getNumRows(), false));
    iter = 0;
    while(counter > 0) //fill in more tiles with dirt
    {
        if(iter++ > 100000)
            print_warning("possible infinite loop: 100000 iterations when expanding frontier");
        if(frontier.size() == 0)
            print_error("frontier.size()==0 in the middle of map generation");
        pii &pos = frontier.get_random_element();
        frontier.erase(pos);
        int x = pos.A, y = pos.B;
        //we can't just check if tiles[x][y] == MapTile::not_set because it's possible that the connecting paths that we created
        //just before this may set all tiles[x][y] that are in the frontier to MapTile::dirt and cause an infinite loop
        //because we can't expand on any part of the frontier
        if(isInMap(x, y) && !vis[x][y])
        {
            vis[x][y] = true;
            if(tiles[x][y] == MapTile::not_set)
            {
                if(randf() < slip_tile_pct)
                    tiles[x][y] = MapTile::slip;
                else tiles[x][y] = MapTile::dirt;
                counter--;
            }
            frontier.insert(mp(x-1, y));
            frontier.insert(mp(x+1, y));
            frontier.insert(mp(x, y-1));
            frontier.insert(mp(x, y+1));
        }
    }
    for(auto &i: tiles) //every tile that isn't set is a wall
        for(auto &j: i)
            if(j == MapTile::not_set)
                j = MapTile::wall;
}
void Room::generateEnemies(double spawnChance, fast_map<int, double> &spawn_weight)
{
    double curDiff = 0; //difference between # of spawned units so far and expected number of spawned units
    double curChance = spawnChance; //if we were supposed to create a unit but it didn't fit, then increase this
    double spawn_weight_sum = 0;
    for(auto &i: spawn_weight)
        spawn_weight_sum += i.B;
    for(int i=0; i<getNumColumns(); i++) //add enemies
        for(int j=0; j<getNumRows(); j++)
            if(isPassableTile(i, j))
            {
                curDiff += spawnChance;
                if(randf() - curDiff / 10 < curChance)
                {
                    double x = randf() * spawn_weight_sum;
                    for(auto &u: spawn_weight)
                    {
                        if(x < u.B)
                        {
                            std::shared_ptr<Unit> unit = std::make_shared<Unit>(u.A, Affiliation::generic_enemy, i, j);
                            if(!unit->isLegalPosition(*this))
                            {
                                curChance = 1;
                                break;
                            }
                            curDiff--;
                            curChance = spawnChance;
                            units.push_back(std::move(unit));
                            break;
                        }
                        x -= u.B;
                    }
                }
            }
}
void Room::init()
{
    init_floor_1();
    init_floor_3();
    init_floor_5();
    init_floor_7();
}
void Room::init_floor_1()
{
    GameMap::PRESET_ROOMS[1].resize(10);
    {
        auto &curRoom = GameMap::PRESET_ROOMS[1][0];
        curRoom.weight = 0.4;
        curRoom.custom_func = [](Room &room)
        {
            room.setMapSize(randz(6, 10), randz(6, 10));
            room.fillWithTile(MapTile::dirt);
            static fast_map<int, double> spawn_weight;
            if(spawn_weight.empty())
            {
                spawn_weight[OrangeSquare] = 1;
            }
            room.generateEnemies(0.045, spawn_weight);
        };
    }
    {
        auto &curRoom = GameMap::PRESET_ROOMS[1][1];
        curRoom.weight = 1;
        curRoom.custom_func = [](Room &room)
        {
            room.generateGenericRoom(randz(13, 20), randz(13, 20), 3, 0.5, NOT_SET, 0);
            static fast_map<int, double> spawn_weight;
            if(spawn_weight.empty())
            {
                spawn_weight[RedSquare] = 1;
                spawn_weight[YellowSquare] = 0.5;
                spawn_weight[GreenSquare] = 0.3;
                spawn_weight[OrangeSquare] = 0.5;
                spawn_weight[RedT] = 0.4; //these get spawned in walls for some reason
                spawn_weight[Red4Star] = 0.3;
            }
            room.generateEnemies(0.04, spawn_weight);
        };
    }
    {
        auto &curRoom = GameMap::PRESET_ROOMS[1][2];
        curRoom.weight = 0.4;
        curRoom.custom_func = [](Room &room)
        {
            room = Room(randz(6, 10), randz(6, 10));
            room.fillWithTile(MapTile::dirt);
            static fast_map<int, double> spawn_weight;
            if(spawn_weight.empty())
            {
                spawn_weight[RedSquare] = 1;
            }
            room.generateEnemies(0.06, spawn_weight);
        };
    }
}
void Room::init_floor_3()
{

}
void Room::init_floor_5()
{

}
void Room::init_floor_7()
{

}
