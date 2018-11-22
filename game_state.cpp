#include "game_state.h"
#include "sdl_base.h"
#include "math_funcs.h"
#include <string>
GameState::GameState(){}
int GameState::getFloorDisplayW() const
{
    return getWindowW() * (1 - getHudSizeFraction());
}
int GameState::getFloorDisplayH() const
{
    return getWindowH();
}
double GameState::getHudSizeFraction() const
{
    return hudSizeFraction;
}
void GameState::setHudSizeFraction(double f)
{
    hudSizeFraction = f;
}
int GameState::getCurrentFloor() const
{
    return currentFloor;
}
int GameState::getHudStartX() const
{
    return getFloorDisplayW() + getHudBoundarySize();
}
int GameState::getHudW() const
{
    return getWindowW() - getHudStartX();
}
int GameState::getHudH() const
{
    return getWindowH();
}
int GameState::getHudBoundarySize() const
{
    return getWindowW() * hudBoundaryFraction;
}
int GameState::getPixelsPerTile() const
{
    return pixelsPerTile;
}
void GameState::setPixelsPerTile(int p)
{
    pixelsPerTile = p;
}
std::shared_ptr<Player> GameState::getPlayer()
{
    return player;
}
void GameState::initPrelevelMenu()
{
    game_state_state.push(GameStateState::preLevelMenu);
    shopItems.clear();
    int NUM_SHOP_ITEMS = 4;
    int SHOP_ITEM_ITER = 0; //if for whatever reason we can't fill the shop up then just break (better than crashing)
    while(SHOP_ITEM_ITER++ < 100 && (int)shopItems.size() < NUM_SHOP_ITEMS) //generate shop items
    {
        Item toAdd(-1);
        double x = randf() * GameMap::TOTAL_ITEM_SHOP_SPAWN_WEIGHT[currentFloor];
        for(int k=0; k<Item::NUM_ITEMS; k++)
        {
            if(x < GameMap::ITEM_SHOP_SPAWN_WEIGHT[currentFloor][k])
            {
                toAdd = Item(k);
                break;
            }
            x -= GameMap::ITEM_SHOP_SPAWN_WEIGHT[currentFloor][k];
        }
        if(toAdd.name == -1)
        {
            print_warning("Unable to generate a shop item in this iteration");
            continue;
        }
        bool alreadyHas = false;
        for(auto &j: shopItems)
        {
            if(j.name == toAdd.name)
            {
                alreadyHas = true;
                break;
            }
        }
        for(auto &j: getPlayer()->items)
        {
            if(j.name == toAdd.name)
            {
                alreadyHas = true;
                break;
            }
        }
        if(!alreadyHas)
            shopItems.push_back(toAdd);
    }
    shopWeapons.clear();
    int NUM_SHOP_WEAPONS = 4;
    int SHOP_WEAPON_ITER = 0; //if for whatever reason we can't fill the shop up then just break (better than crashing)
    while(SHOP_WEAPON_ITER++ < 100 && (int)shopWeapons.size() < NUM_SHOP_WEAPONS) //generate shop weapons
    {
        Weapon toAdd(-1);
        double x = randf() * GameMap::TOTAL_WEAPON_SHOP_SPAWN_WEIGHT[currentFloor];
        for(int k=0; k<Weapon::NUM_WEAPONS; k++)
        {
            if(x < GameMap::WEAPON_SHOP_SPAWN_WEIGHT[currentFloor][k])
            {
                toAdd = Weapon(k);
                break;
            }
            x -= GameMap::WEAPON_SHOP_SPAWN_WEIGHT[currentFloor][k];
        }
        if(toAdd.type == -1)
        {
            print_warning("Unable to generate a shop item in this iteration");
            continue;
        }
        bool alreadyHas = false;
        for(auto &j: shopWeapons)
        {
            if(j.type == toAdd.type)
            {
                alreadyHas = true;
                break;
            }
        }
        for(auto &j: getPlayer()->weapons)
        {
            if(j.type == toAdd.type)
            {
                alreadyHas = true;
                break;
            }
        }
        if(!alreadyHas)
            shopWeapons.push_back(toAdd);
    }
    prelevelMenuPos = 0;
    //make sure that the default Poofy Pistol (which shouldn't appear in the menu) is at position 0
    //weapons[0] is ignored/not rendered in the menu, and the ignored weapon should be the default one with infinite ammo
    //at any rate, weapons[0] should always exist and never be anything other than the Poofy Pistol, hence the warning here
    auto p = getPlayer();
    if(p->weapons.size() == 0)
        print_warning("player has no weapons");
    else if(p->weapons[0].type != 0)
        print_warning("weapons[0] is not the Poofy Pistol");
    for(int i=0; i<(int)p->weapons.size(); i++)
    {
        if(p->weapons[i].type == 0)
        {
            std::swap(p->weapons[0], p->weapons[i]);
            break;
        }
    }
}
void GameState::initNewFloor()
{
    game_map = GameMap(*this);
    //game_map.generateExploredTexture(*this);
    auto p = getPlayer();
    game_map.units.insert(game_map.units.begin(),p);
    p->setX(game_map.getStartPositionX());
    p->setY(game_map.getStartPositionY());
    p->curWeapon = 0; //the initial selected weapon is always the Poofy Pistol
    //Item behavior is all set here. Assume that items can not be acquired in the middle of a floor.
    p->applyMods(p->items);
    for(auto &i: p->weapons)
        i.applyMods(p->items);
    game_state_state.push(GameStateState::inGame);
}
void GameState::initNewGame()
{
    currentFloor = 1;
    player = std::make_shared<Player>(0, 0); ///pass in whatever, the position isn't set yet
    initPrelevelMenu();
}
void GameState::partitionUnits() //1D partitioning should be fine. To optimize further, sorting + binary search can be used.
{
    unitPartition.clear();
    unitPartition.resize(game_map.getNumColumns(), std::vector<std::shared_ptr<Unit> >());
    for(auto &i: game_map.units)
    {
        if(i->getX()+0.5<0 || i->getX()+0.5>=game_map.getNumColumns())
        {
            println("warning: unit column is out of map, not adding to partition vector");
            println(to_str(i->getX()+0.5));
        }
        else unitPartition[(int)(i->getX()+0.5)].push_back(i);
    }
    for(auto &i: unitPartition)
        std::sort(i.begin(), i.end(), Unit::compare_y_with_unit);
}
void GameState::operateGameInGame()
{
    auto p = getPlayer();
    game_map.floorTimeLeft -= 1.0 / BASE_FPS;
    if(game_map.floorTimeLeft <= 0)
        Unit::dealDamage(nullptr, p.get(), currentFloor / (double)BASE_FPS);
    game_map.alreadyCollided.clear(); //the set of the IDs of all unit pairs that have already collided this frame
    getPlayer()->handleInput(*this);
    for(int iter=0; iter<ITER_PER_FRAME; iter++)
    {
        for(auto &i: game_map.units)
            i->updateTemporaryMods();
        partitionUnits();
        for(auto &i: game_map.projectiles)
            i.move(*this);
        for(auto &i: game_map.units)
        {
            i->operate(*this);
        }
        auto &proj = game_map.projectiles;
        for(int i=0; i<(int)proj.size(); i++) //check projectile collisions (do it separately from moving so projectiles don't appear in walls)
        {
            if(proj[i].collide(*this))
            {
                proj.erase(proj.begin() + i);
                i--;
            }
        }
        auto &u = game_map.units;
        int playerID = p->ID;
        for(int i=0; i<(int)u.size(); i++) //remove all dead units. note: this only needs to occasionally be done probably, but it's fast so it's staying here.
        {
            if(u[i]->ID!=playerID && u[i]->isDead())
            {
                disapObjs.emplace_back(u[i]->sprites[u[i]->type], 15, u[i]->getX(), u[i]->getY(), u[i]->getAngle());
                u.erase(u.begin() + i);
                i--;
            }
        }
    }
    if(game_map.units.size() == 1) //level cleared! the player is the only unit remaining
    {
        game_state_state.pop();
        if(currentFloor%2 == 0) //only show the prelevel menu on non-boss floors
        {
            currentFloor++;
            initPrelevelMenu();
        }
        else
        {
            currentFloor++;
            initNewFloor();
        }
    }
    for(int i=0; i<(int)disapObjs.size(); i++) //remove all dead units
    {
        disapObjs[i].update();
        if(disapObjs[i].shouldRemove())
        {
            disapObjs.erase(disapObjs.begin() + i);
            i--;
        }
    }
}
void GameState::drawHUD()
{
    fillRect(getHudStartX(), 0, getHudW(), getHudH(), hudColor.r, hudColor.g, hudColor.b, hudColor.a);
    fillRect(getFloorDisplayW(), 0, getHudBoundarySize(), getHudH(),
             hudBoundaryColor.r, hudBoundaryColor.g, hudBoundaryColor.b, hudBoundaryColor.a);
    VerticalTextDrawer HUDtext(getHudStartX(), 0, getFontSize(0.5), getWindowW());
    HUDtext.draw(GameMap::BASE_FLOOR_NAME[currentFloor], VerticalTextDrawer::Justify::right);
    double &ftl = game_map.floorTimeLeft;
    if(ftl < 10) //not much time remaining!
        HUDtext.draw(seconds_to_str_no_h(std::max(0, (int)(ftl + 1 - EPSILON))), VerticalTextDrawer::Justify::right, 127.5 * (1 + std::sin(10 * (ftl - 10))), 0, 0);
    else HUDtext.draw(seconds_to_str_no_h(std::max(0, (int)(ftl + 1 - EPSILON))), VerticalTextDrawer::Justify::right);
    HUDtext.draw("$", VerticalTextDrawer::Justify::left, 0, 170, 0);
    HUDtext.drawOnSameLine(to_str(getPlayer()->money), VerticalTextDrawer::Justify::right, 0, 255, 0);
    HUDtext.draw("HP", VerticalTextDrawer::Justify::left, 170, 0, 0);
    HUDtext.drawOnSameLine(to_str((int)(getPlayer()->HP + 1 - EPSILON)) + "/" + to_str((int)(getPlayer()->maxHP + 1 - EPSILON)), VerticalTextDrawer::Justify::right, 255, 0, 0);
    HUDtext.fillRect(0, 0, 0, 30);
    HUDtext.draw("Weapons");
    int cnt = 0;
    auto p = getPlayer();
    for(auto &i: p->weapons)
    {
        if(cnt++ == p->curWeapon)
        {
            fillRect(HUDtext.x, HUDtext.y, std::max(0.0, 1 - p->weapons[p->curWeapon].fireCounter / p->weapons[p->curWeapon].fireRate) * (HUDtext.maxx - HUDtext.x), HUDtext.h, 150, 150, 150);
        }
        renderCopy(i.sprites[i.type], HUDtext.x, HUDtext.y, HUDtext.h, HUDtext.h);
        if(i.ammo == INF)
            HUDtext.draw("inf", VerticalTextDrawer::Justify::right);
        else HUDtext.draw(to_str(i.ammo), VerticalTextDrawer::Justify::right);
    }
    HUDtext.fillRect(0, 0, 0, 30);
    HUDtext.draw("Items");
    double spriteSize = getHudW() / 4;
    cnt = 0;
    for(auto &i: player->items)
    {
        renderCopy(Item::sprites[(int)i.name], HUDtext.x + spriteSize * (cnt%4), HUDtext.y + spriteSize * (cnt/4), spriteSize, spriteSize);
        cnt++;
    }
}
void GameState::drawInGame()
{
    game_map.setCameraPosition(getPlayer()->getX() - getFloorDisplayW()/(2.0*getPixelsPerTile()),
                                     getPlayer()->getY() - getFloorDisplayH()/(2.0*getPixelsPerTile()));
    setViewport(0, 0, getFloorDisplayW(), getFloorDisplayH());
    game_map.draw(*this);
    for(auto &i: game_map.units)
        i->draw(*this);
    for(auto &i: game_map.projectiles)
        i.draw(*this);
    setViewport(NULL);
    drawHUD();
    game_map.drawMinimap(*this);
    for(auto &i: disapObjs)
        i.draw(*this);
}
static const int MENU_W = 4, MENU_H = 4;
void GameState::operatePreLevelMenu()
{
    int &menuPos = prelevelMenuPos;
    for(auto &i: inputEvents)
    {
        switch(i.type)
        {
        case SDL_KEYDOWN:
            switch(i.key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w:
                menuPos = (menuPos + MENU_W * (MENU_H - 1)) % (MENU_W * MENU_H);
                break;
            case SDLK_DOWN:
            case SDLK_s:
                menuPos = (menuPos + MENU_W) % (MENU_W * MENU_H);
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                if(menuPos%MENU_W == 3)
                    menuPos -= (MENU_W - 1);
                else menuPos++;
                break;
            case SDLK_LEFT:
            case SDLK_a:
                if(menuPos%4 == 0)
                    menuPos += (MENU_W - 1);
                else menuPos--;
                break;
            case SDLK_RETURN:
            case SDLK_RETURN2:
                switch(menuPos)
                {
                case 0:
                    game_state_state.pop();
                    initNewFloor();
                    break;
                case 1:
                case 2:
                case 3:
                    {
                        auto p = getPlayer();
                        if((int)p->weapons.size() > menuPos)
                        {
                            int type = p->weapons[menuPos].type;
                            int cost = Weapon::BASE_AMMO_COST[type];
                            if(p->money >= cost)
                            {
                                p->money -= cost;
                                p->weapons[menuPos].ammo += Weapon::BASE_AMMO_PER_PURCHASE[type];
                            }
                        }
                    }
                    break;
                case 4:
                case 5:
                case 6:
                case 7:
                    {
                        auto p = getPlayer();
                        if((int)shopItems.size() > menuPos-4)
                        {
                            Item::Name name = shopItems[menuPos-4].name;
                            if(name == NOT_SET)
                                break;
                            int cost = Item::BASE_COST[(int)name];
                            if(p->money >= cost)
                            {
                                p->money -= cost;
                                p->items.emplace_back(name);
                                p->applyMods(p->items); //it's visually more pleasing to immediately see the effects of HP upgrades
                                shopItems[menuPos-4].name = (Item::Name)NOT_SET;
                            }
                        }
                    }
                    break;
                case 8:
                case 9:
                case 10:
                case 11:
                    {
                        auto p = getPlayer();
                        if((int)shopWeapons.size() > menuPos-8 && p->weapons.size()<4) //max of 4 weapons at a time
                        {
                            int type = shopWeapons[menuPos-8].type;
                            if(type == NOT_SET)
                                break;
                            int cost = Weapon::BASE_COST[type];
                            if(p->money >= cost)
                            {
                                p->money -= cost;
                                p->weapons.emplace_back(type);
                                shopWeapons[menuPos-8].type = NOT_SET;
                            }
                        }
                    }
                    break;
                }
                break;
            case SDLK_x:
                switch(menuPos)
                {
                case 1:
                case 2:
                case 3:
                    {
                        auto p = getPlayer();
                        if((int)p->weapons.size() > menuPos)
                        {
                            int type = p->weapons[menuPos].type;
                            int cost = Weapon::BASE_COST[type] / 4;
                            p->money += cost;
                            p->weapons.erase(p->weapons.begin() + menuPos);
                        }
                    }
                    break;
                }
                break;
            }
            break;
        }
    }
}
static void getMenuPosRect(GameState &game_state, int pos, int &x, int &y, int &w, int &h)
{
    w = game_state.getFloorDisplayW() / MENU_W;
    h = game_state.getFloorDisplayH() * 0.8 / MENU_H;
    x = (pos%MENU_W) * game_state.getFloorDisplayW() / MENU_W; //don't do *= w because ints lose a little accuracy
    y = (pos/MENU_W) * game_state.getFloorDisplayH() * 0.8 / MENU_H; //the bottom 75% is reserved for descriptions
}
void GameState::drawPreLevelMenu()
{
    int &menuPos = prelevelMenuPos;
    renderClear(230, 230, 230);
    fillRect(0, getFloorDisplayH() * 0.8, getFloorDisplayW(), getFloorDisplayH()*0.2 + 2, 200, 220, 200); //description box
    int pos = 0;
    int x, y, w, h;
    getMenuPosRect(*this, pos, x, y, w, h);
    drawText("END SHOPPING", x, y, w/6);
    pos++;
    for(auto &i: getPlayer()->weapons)
    {
        if(i.type != 0) //don't draw the default weapon
        {
            getMenuPosRect(*this, pos, x, y, w, h);
            int spriteSize = std::min(h/4, w/4);
            fillRect(x+w-spriteSize, y+1, spriteSize-1, spriteSize-1, 215, 215, 215);
            renderCopy(Weapon::sprites[i.type], x+w-spriteSize, y, spriteSize, spriteSize);
            int fontSize = std::min(h/6, (int)(w*0.11));
            drawText("Owned Weapon", x, y, fontSize, 100, 0, 0);
            VerticalTextDrawer weaponText(x, y+spriteSize, fontSize, x+w);
            weaponText.draw(to_str(Weapon::BASE_NAME[i.type]));
            weaponText.draw("Buy Ammo: " + to_str(Weapon::BASE_AMMO_PER_PURCHASE[i.type]) + "/$" + to_str(Weapon::BASE_AMMO_COST[i.type]));
            weaponText.draw("Sell [X]: $" + to_str(Weapon::BASE_COST[i.type]/4));
            pos++;
        }
    }
    while(pos < 4)
    {
        getMenuPosRect(*this, pos, x, y, w, h);
        drawText("(EMPTY WEAPON SLOT)", x, y, w/9.5);
        pos++;
    }
    for(auto &i: shopItems)
    {
        getMenuPosRect(*this, pos, x, y, w, h);
        if(i.name == Item::None)
            drawText("(EMPTY SHOP ITEM SLOT)", x, y, w/11);
        else
        {
            int spriteSize = std::min(h/4, w/4);
            fillRect(x+w-spriteSize, y+1, spriteSize-1, spriteSize-1, 215, 215, 215);
            renderCopy(Item::sprites[(int)i.name], x+w-spriteSize, y, spriteSize, spriteSize);
            int fontSize = std::min(h/6, (int)(w*0.11));
            drawText("Shop Item", x, y, fontSize, 100, 0, 0);
            VerticalTextDrawer shopItemText(x, y+spriteSize, fontSize, x+w);
            shopItemText.draw(to_str(Item::BASE_NAME[i.name]));
            shopItemText.draw("Buy: $" + to_str(Item::BASE_COST[i.name]));
        }
        pos++;
    }
    while(pos < 8)
    {
        getMenuPosRect(*this, pos, x, y, w, h);
        drawText("(EMPTY SHOP ITEM SLOT)", x, y, w/11);
        pos++;
    }
    for(auto &i: shopWeapons)
    {
        getMenuPosRect(*this, pos, x, y, w, h);
        if(i.type == NOT_SET)
            drawText("(EMPTY SHOP WEAPON SLOT)", x, y, w/12);
        else
        {
            int spriteSize = std::min(h/4, w/4);
            fillRect(x+w-spriteSize, y+1, spriteSize-1, spriteSize-1, 215, 215, 215);
            renderCopy(Weapon::sprites[i.type], x+w-spriteSize, y, spriteSize, spriteSize);
            int fontSize = std::min(h/6, (int)(w*0.11));
            drawText("Shop Weapon", x, y, fontSize, 100, 0, 0);
            VerticalTextDrawer shopWeaponText(x, y+spriteSize, fontSize, x+w);
            shopWeaponText.draw(to_str(Weapon::BASE_NAME[i.type]));
            shopWeaponText.draw("Buy: $" + to_str(Weapon::BASE_COST[i.type]));
        }
        pos++;
    }
    while(pos < 12)
    {
        getMenuPosRect(*this, pos, x, y, w, h);
        drawText("(EMPTY SHOP WEAPON SLOT)", x, y, w/12);
        pos++;
    }
    int maxY = getFloorDisplayH() * 0.8, maxX = getFloorDisplayW();
    int fsz = getFontSize(0.25);
    switch(menuPos)
    {
    case 0:
        drawMultilineText("Shopping time is over when you press enter. Time to kill some geometric objects! Woohoo!", 0, maxY, maxX, fsz);
        break;
    case 1:
    case 2:
    case 3:
        if((int)player->weapons.size() > menuPos)
            drawMultilineText(Weapon::BASE_DESC[player->weapons[menuPos].type], 0, maxY, maxX, fsz);
        else drawMultilineText("You have an empty weapon slot. Do you know what geometric shapes do when they have empty weapon slots? They buy more weapons.", 0, maxY, maxX, fsz);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        if((int)shopItems.size() > menuPos-4)
        {
            if(shopItems[menuPos-4].name != Item::None)
                drawMultilineText(Item::BASE_DESC[shopItems[menuPos-4].name], 0, maxY, maxX, fsz);
            else drawMultilineText("You already bought this item, so the space appears blank", 0, maxY, maxX, fsz);
        }
        else drawMultilineText("The shop seems to be lacking in items, which is an... unintended feature.", 0, maxY, maxX, fsz);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
        if((int)shopWeapons.size() > menuPos-8)
        {
            if(shopWeapons[menuPos-8].type != NOT_SET)
                drawMultilineText(Weapon::BASE_DESC[shopWeapons[menuPos-8].type], 0, maxY, maxX, fsz);
            else drawMultilineText("You already bought this weapon, so the space appears blank", 0, maxY, maxX, fsz);
        }
        else drawMultilineText("The shop seems to be lacking in weapons, which is an... unintended feature.", 0, maxY, maxX, fsz);
        break;
    case 12:
    case 13:
    case 14:
    case 15:
        drawMultilineText("This row of menu options is taking a sick leave. Check again when they've decided to become healthy!", 0, maxY, maxX, fsz);
        break;
    default:
        drawMultilineText("Somehow you've selected a menu position that's not allowed, so the description is going to be just this.", 0, maxY, maxX, fsz);
        break;
    }
    getMenuPosRect(*this, menuPos, x, y, w, h);
    fillRect(x, y, w+1, h+1, 0, 0, 0, 20); //highlight current menu selectiotn
    setColor(0, 0, 0);
    for(int i=1; i<MENU_W; i++)
        drawLine(maxX * i / MENU_W, 0, maxX * i / MENU_W, maxY);
    for(int i=1; i<=MENU_H; i++)
        drawLine(0, maxY * i / MENU_H, maxX, maxY * i / MENU_H);
    drawHUD();
}
