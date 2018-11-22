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
SDL_Texture *GameMap::dirt_sprites[NUM_DIRT_SPRITES], *GameMap::wall_sprites[NUM_WALL_SPRITES];
int GameMap::BASE_FLOOR_TIME[GameMap::NUM_FLOORS];
std::string GameMap::BASE_FLOOR_NAME[GameMap::NUM_FLOORS];
double GameMap::UNIT_SPAWN_WEIGHT[GameMap::NUM_FLOORS][Unit::NUM_UNITS];
double GameMap::UNIT_SPAWN_CHANCE[GameMap::NUM_FLOORS], GameMap::TOTAL_UNIT_SPAWN_WEIGHT[GameMap::NUM_FLOORS];
double GameMap::TOTAL_ITEM_SHOP_SPAWN_WEIGHT[GameMap::NUM_FLOORS], GameMap::ITEM_SHOP_SPAWN_WEIGHT[GameMap::NUM_FLOORS][Item::NUM_ITEMS];
double GameMap::TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[GameMap::NUM_FLOORS], GameMap::WEAPON_SHOP_SPAWN_WEIGHT[GameMap::NUM_FLOORS][Weapon::NUM_WEAPONS];
int GameMap::FLOOR_W[GameMap::NUM_FLOORS], GameMap::FLOOR_H[GameMap::NUM_FLOORS];
GameMap::GameMap(){}
GameMap::GameMap(GameState &game_state)
{
    this->floor = game_state.currentFloor;
    minimap_texture = NULL;
    //explored_texture = NULL;
    generateFloor();
    generateMinimapTexture(game_state);
    double xFloorTimeLeft = 1;
    for(auto &i: game_state.getPlayer()->items)
    {
        switch(i.name)
        {
        case Item::Name::DaltonsAtom: //dalton's atom
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
    units.clear();
    projectiles.clear();
    explored.clear();
    tiles.clear();
    shortestPathDir.clear();
}
double GameMap::getStartPositionX() const
{
    return startPositionX;
}
double GameMap::getStartPositionY() const
{
    return startPositionY;
}
int GameMap::getNumRows() const
{
    if(tiles.size() == 0)
        print_error("trying to fetch number of rows, but there are 0 columns");
    return tiles[0].size();
}
int GameMap::getNumColumns() const
{
    return tiles.size();
}
int GameMap::getArea() const
{
    return getNumRows() * getNumColumns();
}
bool GameMap::isInMap(int x, int y) const
{
    return x>=0 && y>=0 && x<getNumColumns() && y<getNumRows();
}
bool GameMap::isPassableTile(int x, int y) const
{
    return x>=0 && y>=0 && x<getNumColumns() && y<getNumRows() && tiles[x][y] == MapTile::dirt;
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
void GameMap::setCameraPosition(double x, double y)
{
    cameraX = x;
    cameraY = y;
}
void GameMap::setMapSize(int columns, int rows)
{
    tiles.clear();
    tiles.resize(columns, std::vector<MapTile>(rows, MapTile::not_set));
}
/*void GameMap::generateShortestPaths()
{
    if(shortestPathDir.size() != 0)
        print_warning("shortestPathDir.size() is supposed to be 0, but it's " + to_str(shortestPathDir.size()));
    shortestPathDir.resize(getNumColumns());
    for(auto &i: shortestPathDir)
    {
        i.resize(getNumRows());
        for(auto &j: i)
        {
            j.resize(getNumColumns());
            for(auto &k: j)
                k.resize(getNumRows());
        }
    }
    //SPFA instead BFS because we may want to include diagonals later
    std::vector<std::vector<bool> > inq(getNumColumns(), std::vector<bool>(getNumRows()));
    std::vector<std::vector<double> > dist(getNumColumns(), std::vector<double>(getNumRows()));
    for(int x=0; x<getNumColumns(); x++)
    {
        for(int y=0; y<getNumRows(); y++)
        {
            if(!isPassableTile(x, y))
                continue;
            for(int i=0; i<getNumColumns(); i++)
            {
                for(int j=0; j<getNumRows(); j++)
                {
                    if(isPassableTile(i, j))
                        inq[i][j] = false;
                    else inq[i][j] = true; //this way impassable tiles will never be pushed into the queue
                }
            }
            for(auto &i: dist)
                for(auto &j: i)
                    j = INF;
            shortestPathDir[x][y].resize(getNumColumns(), std::vector<pii>(getNumRows()));
            dist[x][y] = 0;
            inq[x][y] = true;
            std::queue<pii> q;
            if(x > 0 && isPassableTile(x-1, y))
            {
                inq[x-1][y] = true;
                dist[x-1][y] = 1;
                q.emplace(x-1, y);
                shortestPathDir[x][y][x-1][y] = mp(-1, 0);
            }
            if(x < getNumColumns()-1 && isPassableTile(x+1, y))
            {
                inq[x+1][y] = true;
                dist[x+1][y] = 1;
                q.emplace(x+1, y);
                shortestPathDir[x][y][x+1][y] = mp(1, 0);
            }
            if(y > 0 && isPassableTile(x, y-1))
            {
                inq[x][y-1] = true;
                dist[x][y-1] = 1;
                q.emplace(x, y-1);
                shortestPathDir[x][y][x][y-1] = mp(0, -1);
            }
            if(y < getNumRows()-1 && isPassableTile(x, y+1))
            {
                inq[x][y+1] = true;
                dist[x][y+1] = 1;
                q.emplace(x, y+1);
                shortestPathDir[x][y][x][y+1] = mp(0, 1);
            }
            while(!q.empty())
            {
                int curX = q.front().A, curY = q.front().B;
                q.pop();
                inq[curX][curY] = false;
                if(curX>0 && !inq[curX-1][curY] && dist[curX-1][curY]>dist[curX][curY]+1)
                {
                    q.emplace(curX-1, curY);
                    inq[curX-1][curY] = true;
                    dist[curX-1][curY] = dist[curX][curY]+1;
                    shortestPathDir[x][y][curX-1][curY] = shortestPathDir[x][y][curX][curY];
                }
                if(curX<getNumRows()-1 && !inq[curX+1][curY] && dist[curX+1][curY]>dist[curX][curY]+1)
                {
                    q.emplace(curX+1, curY);
                    inq[curX+1][curY] = true;
                    dist[curX+1][curY] = dist[curX][curY]+1;
                    shortestPathDir[x][y][curX+1][curY] = shortestPathDir[x][y][curX][curY];
                }
                if(curY>0 && !inq[curX][curY-1] && dist[curX][curY-1]>dist[curX][curY]+1)
                {
                    q.emplace(curX, curY-1);
                    inq[curX][curY-1] = true;
                    dist[curX][curY-1] = dist[curX][curY]+1;
                    shortestPathDir[x][y][curX][curY-1] = shortestPathDir[x][y][curX][curY];
                }
                if(curY<getNumColumns()-1 && !inq[curX][curY+1] && dist[curX][curY+1]>dist[curX][curY]+1)
                {
                    q.emplace(curX, curY+1);
                    inq[curX][curY+1] = true;
                    dist[curX][curY+1] = dist[curX][curY]+1;
                    shortestPathDir[x][y][curX][curY+1] = shortestPathDir[x][y][curX][curY];
                }
            }
        }
    }
}*/
struct generateRandZ
{
    typedef size_t result_type;
    static size_t min()
    {
        return 0;
    }
    static size_t max()
    {
        return RANDUZ_MAX;
    }
    size_t operator()()
    {
        return randuz();
    }
};
void GameMap::generateFloorGeneric(int numClusters, double fillFactor)
{
    if(numClusters > getArea())
        print_error("number of clusters is greater than map area");
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
                tiles[curX][curY] = MapTile::dirt;
        }
    }
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
            tiles[x][y] = MapTile::dirt;
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
    double curDiff = 0; //difference between # of spawned units so far and expected number of spawned units
    double curChance = UNIT_SPAWN_CHANCE[floor]; //if we were supposed to create a unit but it didn't fit, then increase this
    for(int i=0; i<getNumColumns(); i++) //add enemies
        for(int j=0; j<getNumRows(); j++)
            if(isPassableTile(i, j))
            {
                curDiff += UNIT_SPAWN_CHANCE[floor];
                if(std::hypot(getStartPositionX()-i, getStartPositionY()-j) > 5 && randf() - curDiff / 10 < curChance)
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
                            curChance = UNIT_SPAWN_CHANCE[floor];
                            units.push_back(unit);
                            break;
                        }
                        x -= UNIT_SPAWN_WEIGHT[floor][k];
                    }
                }
            }
}
void GameMap::dfsValidate(std::vector<std::vector<bool> > &visited, int x, int y) const
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
bool GameMap::isValidFloor() const
{
    if(floor%2 == 0) //it's a custom boss floor
        return true;
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
void GameMap::generateMapFloor1()
{
    current_blank_sprite = wall_sprites[0];
    current_wall_sprite = wall_sprites[1];
    current_dirt_sprite = dirt_sprites[0];
    setMapSize(FLOOR_W[1], FLOOR_H[1]);
    generateFloorGeneric(10, 0.5);
}
void GameMap::generateMapFloor2()
{
    current_blank_sprite = wall_sprites[0];
    current_wall_sprite = wall_sprites[1];
    current_dirt_sprite = dirt_sprites[0];
    setMapSize(10, 10);
    for(auto &i: tiles)
        for(auto &j: i)
            j = MapTile::dirt;
    startPositionX = 0;
    startPositionY = 0;
    units.push_back(std::make_shared<Unit>(7, Affiliation::generic_enemy, 4.5, 4.5));
}
void GameMap::generateFloor()
{
    println("Generating a map for floor " + to_str(floor));
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
        default:
            print_error("cannot create map for unknown floor " + to_str(floor));
            break;
        }
    }
    while(!isValidFloor());
    println("Took " + to_str(tries) + " attempt(s) to generate a valid floor");
}
void GameMap::draw(const GameState &game_state) const
{
    fillRect(0, 0, game_state.getFloorDisplayW(), game_state.getFloorDisplayH(), 0, 0, 0);
    int ppt = game_state.getPixelsPerTile();
    int minx = std::max(0, (int)(getCameraX() - game_state.getFloorDisplayW()/2));
    int maxx = std::min(getNumColumns()-1, (int)(getCameraX() + game_state.getFloorDisplayW()/2));
    int miny = std::max(0, (int)(getCameraY() - game_state.getFloorDisplayH()/2));
    int maxy = std::min(getNumRows()-1, (int)(getCameraX() + game_state.getFloorDisplayH()/2));
    for(int i=minx; i<=maxx; i++)
    {
        for(int j=miny; j<=maxy; j++)
        {
            switch(tiles[i][j])
            {
            case MapTile::blank:
                renderCopy(current_blank_sprite, (i - cameraX - 0.5) * ppt,
                           (j - cameraY - 0.5) * ppt, ppt, ppt);
                break;
            case MapTile::dirt:
                renderCopy(current_dirt_sprite, (i - cameraX - 0.5) * ppt,
                           (j - cameraY - 0.5) * ppt, ppt, ppt);
                break;
            case MapTile::wall:
                renderCopy(current_wall_sprite, (i - cameraX - 0.5) * ppt,
                           (j - cameraY - 0.5) * ppt, ppt, ppt);
                break;
            default:
                print_warning("map tile at (" + to_str(i) + ", " + to_str(j) + ") of unknown type " + to_str((int)tiles[i][j]));
                break;
            }
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
    int x = game_state.getHudStartX();
    int w = game_state.getHudW();
    int BOUNDARY_SIZE = w*BOUNDARY_FRACTION;
    x += BOUNDARY_SIZE;
    w -= 2*BOUNDARY_SIZE;
    int h = w * getNumRows() / getNumColumns();
    int y = getWindowH() - h - BOUNDARY_SIZE;
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
    int w, h;
    getMinimapRect(game_state, NULL, NULL, &w, &h);
    minimap_texture = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetRenderTarget(getRenderer(), minimap_texture);
    double wEach = w / (double)getNumColumns();
    double hEach = h / (double)getNumRows();
    for(int i=0; i<getNumColumns(); i++)
    {
        for(int j=0; j<getNumRows(); j++)
        {
            int pixelW = (int)(wEach*(i+1)) - (int)(wEach*i);
            int pixelH = (int)(hEach*(j+1)) - (int)(hEach*j);
            int pixelX = i*wEach, pixelY = j*hEach;
            switch(tiles[i][j])
            {
            case MapTile::blank:
                renderCopy(current_blank_sprite, pixelX, pixelY, pixelW, pixelH);
                break;
            case MapTile::dirt:
                renderCopy(current_dirt_sprite, pixelX, pixelY, pixelW, pixelH);
                break;
            case MapTile::wall:
                renderCopy(current_wall_sprite, pixelX, pixelY, pixelW, pixelH);
                break;
            default:
                print_warning("map tile at (" + to_str(i) + ", " + to_str(j) + ") of unknown type " + to_str((int)tiles[i][j]));
                break;
            }
        }
    }
    SDL_SetRenderTarget(getRenderer(), NULL);
}
void GameMap::drawMinimap(GameState &game_state) const
{
    int x, y, w, h;
    getMinimapRect(game_state, &x, &y, &w, &h);
    int origY = y - (getWindowH() - (y+h));
    fillRect(game_state.getHudStartX(), origY, game_state.getHudW(), getWindowH() - origY, 100, 100, 100);
    renderCopy(minimap_texture, x, y, w, h);
    double eppt = game_state.game_map.getMinimapPixelsPerTile();
    double scale = SPRITE_SCALE * eppt / game_state.getPixelsPerTile();
    for(auto &i: units)
    {
        int rw, rh;
        SDL_QueryTexture(Unit::sprites[i->type], NULL, NULL, &rw, &rh);
        if(!i->isPlayer())
            renderCopyEx(Unit::sprites[i->type], x + eppt * i->getX() - rw * scale / 2 + eppt/2, y + eppt * i->getY() - rh * scale / 2 + eppt/2,
                         rw * scale, rh * scale, TO_DEG(i->getAngle()));
    }
    //renderCopy(explored_texture, x, y, w, h);
    fillRect(x + eppt*game_state.getPlayer()->getX(), y + eppt*game_state.getPlayer()->getY(), eppt, eppt, 255, 255, 255);
}
