#include "game_state.h"
#include "sdl_base.h"
#include "math_funcs.h"
#include "menu.h"
#include <string>
void GameState::drawMainMenu() //dummy function because the function pointer has to point to a member of GameState
{
    main_menu->draw();
}
void GameState::completeInitingPrelevelMenu()
{
    switch_to_music("floor " + to_str(currentFloor));
}
void GameState::initPrelevelMenu(void (GameState::*draw_f1)())
{
    println("initializing pre-level menu");
    game_state_state.push(GameStateState::preLevelMenu);
    initTransitionScreen(draw_f1, GameState::drawPreLevelMenu, GameState::completeInitingPrelevelMenu);
    aestheticEffects.clear(); //we don't want any lingering death animations
    shopItems.clear();
    int NUM_SHOP_ITEMS = 4;
    int SHOP_ITEM_ITER = 0; //if for whatever reason we can't fill the shop up then just break (better than crashing)
    while(SHOP_ITEM_ITER++ < 20 && (int)shopItems.size() < NUM_SHOP_ITEMS) //generate shop items
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
        if(toAdd.type == -1)
        {
            print_warning("Unable to generate a shop item in this iteration");
            continue;
        }
        bool alreadyHas = false;
        for(auto &j: shopItems)
        {
            if(j.type == toAdd.type)
            {
                alreadyHas = true;
                break;
            }
        }
        if(!alreadyHas && !getPlayer()->hasItem((Item::Name)toAdd.type))
            shopItems.push_back(toAdd);
    }
    shopWeapons.clear();
    int NUM_SHOP_WEAPONS = 4;
    int SHOP_WEAPON_ITER = 0; //if for whatever reason we can't fill the shop up then just break (better than crashing)
    while(SHOP_WEAPON_ITER++ < 20 && (int)shopWeapons.size() < NUM_SHOP_WEAPONS) //generate shop weapons
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
            print_warning("Unable to generate a shop weapon in this iteration");
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
static const int MENU_W = 4, MENU_H = 4;
void GameState::operatePreLevelMenu()
{
    updateExtraAesthetics();
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
                        if((int)p->weapons.size() > menuPos && p->weapons[menuPos].ammo != INF) //you can't buy more ammo if you already have INF
                        {
                            int type = p->weapons[menuPos].type;
                            int cost = Weapon::BASE_AMMO_COST[type] * ammo_cost_mult;
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
                            int type = shopItems[menuPos-4].type;
                            if(type == NOT_SET)
                                break;
                            int cost = Item::BASE_COST[type] * item_cost_mult;
                            if(p->money >= cost)
                            {
                                p->money -= cost;
                                if(Item::IS_SHOWN[type])
                                    p->items.emplace_back(type);
                                else p->unshownItems.emplace_back(type);
                                p->applyFirstTimeItemEffects(*this, (Item::Name)type);
                                p->applyMods(p->items); //it's visually more pleasing to immediately see the effects of HP upgrades
                                shopItems[menuPos-4].type = NOT_SET;
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
                            int cost = Weapon::BASE_COST[type] * weapon_cost_mult;
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
                            p->addMoney(cost);
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
            if(Weapon::BASE_SPELL_COST[i.type] != NOT_SET)
                fillCircle(x+w-spriteSize/2, y+spriteSize/2, spriteSize/2, 0, 0, 200, 35);
            int fontSize = std::min(h/6, (int)(w*0.11));
            drawText("Owned Weapon", x, y, fontSize, 100, 0, 0);
            VerticalTextDrawer weaponText(x, y+spriteSize, fontSize, x+w);
            weaponText.draw(to_str(Weapon::BASE_NAME[i.type]));
            weaponText.draw("Buy Ammo: " + to_str(std::ceil(Weapon::BASE_AMMO_PER_PURCHASE[i.type] * ammo_cost_mult)) + "/$" +
                            to_str(Weapon::BASE_AMMO_COST[i.type]));
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
        if(i.type == NOT_SET)
            drawText("(EMPTY SHOP ITEM SLOT)", x, y, w/11);
        else
        {
            int spriteSize = std::min(h/4, w/4);
            fillRect(x+w-spriteSize, y+1, spriteSize-1, spriteSize-1, 215, 215, 215);
            renderCopy(Item::sprites[i.type], x+w-spriteSize, y, spriteSize, spriteSize);
            int fontSize = std::min(h/6, (int)(w*0.11));
            drawText("Shop Item", x, y, fontSize, 100, 0, 0);
            VerticalTextDrawer shopItemText(x, y+spriteSize, fontSize, x+w);
            shopItemText.draw(to_str(Item::BASE_NAME[i.type]));
            shopItemText.draw("Buy: $" + to_str(std::ceil(Item::BASE_COST[i.type] * item_cost_mult)));
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
            if(Weapon::BASE_SPELL_COST[i.type] != NOT_SET)
                fillCircle(x+w-spriteSize/2, y+spriteSize/2, spriteSize/2, 0, 0, 200, 35);
            int fontSize = std::min(h/6, (int)(w*0.11));
            drawText("Shop Weapon", x, y, fontSize, 100, 0, 0);
            VerticalTextDrawer shopWeaponText(x, y+spriteSize, fontSize, x+w);
            shopWeaponText.draw(to_str(Weapon::BASE_NAME[i.type]));
            shopWeaponText.draw("Buy: $" + to_str(std::ceil(Weapon::BASE_COST[i.type] * weapon_cost_mult)));
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
        drawMultilineTextUnbroken("End shopping and go to the next floor.", 0, maxY, maxX, fsz);
        break;
    case 1:
    case 2:
    case 3:
        if((int)player->weapons.size() > menuPos)
            drawMultilineTextUnbroken(Weapon::BASE_DESC[player->weapons[menuPos].type], 0, maxY, maxX, fsz);
        else drawMultilineTextUnbroken("You have an empty weapon slot. Do you know what geometric shapes do when they have empty weapon slots? They buy more weapons.", 0, maxY, maxX, fsz);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        if((int)shopItems.size() > menuPos-4)
        {
            if(shopItems[menuPos-4].type != NOT_SET)
                drawMultilineTextUnbroken(Item::BASE_DESC[shopItems[menuPos-4].type], 0, maxY, maxX, fsz);
            else drawMultilineTextUnbroken("You already bought this item, so the space appears blank", 0, maxY, maxX, fsz);
        }
        else drawMultilineTextUnbroken("The shop seems to be lacking in items, which is an... unintended feature. -kworker", 0, maxY, maxX, fsz);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
        if((int)shopWeapons.size() > menuPos-8)
        {
            if(shopWeapons[menuPos-8].type != NOT_SET)
                drawMultilineTextUnbroken(Weapon::BASE_DESC[shopWeapons[menuPos-8].type], 0, maxY, maxX, fsz);
            else drawMultilineTextUnbroken("You already bought this weapon, so the space appears blank", 0, maxY, maxX, fsz);
        }
        else drawMultilineTextUnbroken("The shop seems to be lacking in weapons, which is an... unintended feature. -kworker" , 0, maxY, maxX, fsz);
        break;
    case 12:
    case 13:
    case 14:
    case 15:
        drawMultilineTextUnbroken("This row of menu options is taking a sick leave. Check again when they've decided to become healthy!", 0, maxY, maxX, fsz);
        break;
    default:
        drawMultilineTextUnbroken("Somehow you've selected a menu position that's not allowed, so the description is going to be just this.", 0, maxY, maxX, fsz);
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
    drawExtraAesthetics();
}
