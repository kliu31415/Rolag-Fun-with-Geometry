#include "game_state.h"
#include "sdl_base.h"
#include "math_funcs.h"
#include "menu.h"
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
void GameState::completeInitingNewFloor() //split it up so the transition screen doesn't display the same floor
{
    clearExtraAesthetics(); //no lingering text boxes pls
    game_map = GameMap(*this);
    //game_map.generateExploredTexture(*this);
    auto p = getPlayer();
    //pushing in the player later is good because (a) it won't be cleared in the GameMap constructor and (b) it doesn't count towards the HP sum for floor validation
    game_map.units.insert(game_map.units.begin(), p);
    p->setX(game_map.getStartPositionX());
    p->setY(game_map.getStartPositionY());
    if(currentFloor%2==1 && currentFloor<17) //normal non-boss floor
        game_map.update_explored(p, p->getVisionDistance(*this));
    else game_map.update_explored(p, INF); //reveal the entire boss room immediately
    //p->curWeapon = 0; //the initial selected weapon is always the Poofy Pistol
    //Item behavior is all set here. Assume that items can not be acquired in the middle of a floor.
    if(currentFloor == 17) //kworker
    {
        switch_to_music("floor 17");
        p->curWeapon = 0;
        p->items.clear();
        p->unshownItems.clear();
        p->weapons.resize(1);
        p->unshownItems.push_back(Item::Name::Kworker_Mod);
    }
    else if(currentFloor==18 || currentFloor==19) //give a little ms boost too. This is kept throughout the remainder of the run.
    {
        switch_to_music("floor 18");
        p->unshownItems.push_back(Item::Name::Kworker_Mod);
    }
    std::vector<Item> concatItems = p->items;
    for(auto &i: p->unshownItems)
        concatItems.push_back(i);
    p->applyMods(concatItems);
    for(auto &i: p->weapons)
    {
        i.applyMods(concatItems);
    }
}
void GameState::initNewFloor()
{
    println("starting to initialize new floor");
    game_state_state.push(GameStateState::inGame);
    if(currentFloor>=17 || currentFloor%2==0) //boss floor
        initTransitionScreen(GameState::draw_console_prev_floor, GameState::drawInGame, GameState::completeInitingNewFloor);
    else initTransitionScreen(GameState::drawPreLevelMenu, GameState::drawInGame, GameState::completeInitingNewFloor);
}
static bool firstInitConsole;
void GameState::initNewGame()
{
    println("initializing new game");
    item_cost_mult = weapon_cost_mult = ammo_cost_mult = 1;
    currentFloor = 0; //set this to one less than the starting floor
    player = std::make_shared<Player>(0, 0); //pass in whatever (x, y), it doesn't matter because the position isn't set yet
    firstInitConsole = true;
    initConsole();
}
void GameState::partitionUnits() //1D partitioning should be fine. To optimize further, sorting + binary search can be used.
{
    /*for(auto &i: unitPartition)
    {
        for(auto &j: i)
        {
            if(j.use_count() == 1)
                cout << "WARNING" << endl;
        }
    }*/
    unitPartition.clear();
    unitPartitionY.clear();
    unitPartition.resize(game_map.getNumColumns(), std::vector<std::shared_ptr<Unit> >());
    unitPartitionY.resize(game_map.getNumColumns(), std::vector<double>());
    for(auto &i: game_map.units)
    {
        if(i->getX()+0.5<0 || i->getX()+0.5>=game_map.getNumColumns())
        {
            if(!i->isSpectral)
            {
                println("warning: non spectral unit has column that is out of the map, not adding to partition vector");
                println("x = " + to_str(i->getX()+0.5));
            }
        }
        else unitPartition[(int)(i->getX()+0.5)].push_back(i);
    }
    int pos = 0;
    for(auto &i: unitPartition)
    {
        std::sort(i.begin(), i.end(), Unit::compare_y_with_unit);
        for(auto &j: i)
            unitPartitionY[pos].push_back(j->getY());
        pos++;
    }
}
void GameState::operateGameInGame(bool isPlayerAlreadyDead /*=false*/)
{
    auto p = getPlayer();
    game_map.floorTimeLeft -= 1.0 / BASE_FPS;
    if(game_map.floorTimeLeft <= 0)
        Unit::dealDamage(nullptr, p.get(), currentFloor / (double)BASE_FPS);
    if(!isPlayerAlreadyDead)
        p->handleInput(*this);
    game_map.alreadyCollided.clear(); //the set of the IDs of all unit pairs that have already collided this frame
    partitionUnits(); //it should be fine only doing this once per frame unless things move super fast
    for(int iter=0; iter<ITER_PER_FRAME; iter++)
    {
        for(auto &i: game_map.units)
            i->updateTemporaryMods();
        //don't use enhanced for loops for either because in case push_back is called and the iterators are invalidated
        int sz = game_map.projectiles.size();
        for(int i=0; i<sz; i++)
            game_map.projectiles[i].move(*this);
        sz = game_map.units.size();
        for(int i=0; i<sz; i++)
            game_map.units[i]->operate(*this);
        p->handleCollisionsWithHurtTiles(*this);
        auto &proj = game_map.projectiles;
        for(int i=0; i<(int)proj.size(); i++) //check projectile collisions (do it separately from moving so projectiles don't appear in walls)
        {
            if(proj[i].collide(*this)) //checks if the projectile should be removed (it hit something or its lifespan is over)
            {
                proj.erase(proj.begin() + i);
                i--;
            }
        }
        auto &u = game_map.units;
        for(int i=0; i<(int)u.size(); i++) //remove all dead units. note: this only needs to occasionally be done probably, but it's fast so it's staying here.
        {
            if(u[i]->isDead())
            {
                if(!isPlayerAlreadyDead && u[i]->isPlayer()) //the player should never be removed
                {
                    game_state_state.pop();
                    initDeathScreen();
                    isPlayerAlreadyDead = true;
                    aestheticEffects.push_back(std::make_shared<DisappearingObject>(u[i]->sprite, 0.25, u[i]->getX(), u[i]->getY(), 0));
                    u.erase(u.begin() + i);
                    i--;
                }
                else
                {
                    aestheticEffects.push_back(std::make_shared<DisappearingObject>(u[i]->sprite, 0.25, u[i]->getX(), u[i]->getY(),
                                                u[i]->getAngle()));
                    u[i]->die(*this);
                    u.erase(u.begin() + i);
                    i--;
                }
            }
        }
    }
    if(!isPlayerAlreadyDead && game_map.isFloorFinished())
    {
        if(p->isOnPortal(*this) || (currentFloor>=17 && currentFloor<=19))
        {
            p->addMoney(50 + game_map.floorTimeLeft + game_map.moneyBonusOnCompletion);
            game_state_state.pop();
            initConsole();
        }
    }
    game_map.update_explored(p, p->getVisionDistance(*this));
    if(currentFloor != 17) //no minimap for kworker
    {
        static int framesSinceLastGen = 5;
        framesSinceLastGen++;
        if(framesSinceLastGen >= 5) //this is expensive so don't it that often
        {
            framesSinceLastGen = 0;
            game_map.generateMinimapTexture(*this);
        }
    }
    updateExtraAesthetics();
}
void GameState::drawHUD()
{
    auto p = getPlayer();
    fillRect(getHudStartX(), 0, getHudW(), getHudH(), hudColor.r, hudColor.g, hudColor.b, hudColor.a);
    fillRect(getFloorDisplayW(), 0, getHudBoundarySize(), getHudH(),
             hudBoundaryColor.r, hudBoundaryColor.g, hudBoundaryColor.b, hudBoundaryColor.a);
    VerticalTextDrawer HUDtext(getHudStartX(), 0, getFontSize(0.25), getWindowW());
    static unsigned rand_hex_name = (((unsigned)randz(0, 65535))<<16) + randz(0, 65535);
    if(GameMap::BASE_FLOOR_NAME[currentFloor] == "random_hex")
    {
        if(randf() * BASE_FPS < 0.5 + 5.0/(currentFloor - 5))
            rand_hex_name = (((unsigned)randz(0, 65535))<<16) + randz(0, 65535);
        HUDtext.draw(to_hex_len8(rand_hex_name), VerticalTextDrawer::Justify::right);
    }
    else HUDtext.draw(GameMap::BASE_FLOOR_NAME[currentFloor], VerticalTextDrawer::Justify::right);
    double &ftl = game_map.floorTimeLeft;
    if(ftl < 10) //not much time remaining!
        HUDtext.draw(seconds_to_str_no_h(std::max(0, (int)(ftl + 1 - EPSILON))), VerticalTextDrawer::Justify::right,
                     127.5 * (1 + std::sin(-PI/2 + 10 * (ftl - 10))), 0, 0);
    else HUDtext.draw(seconds_to_str_no_h(std::max(0, (int)(ftl + 1 - EPSILON))), VerticalTextDrawer::Justify::right);
    if(currentFloor >= 7)
        HUDtext.draw("\u1172", VerticalTextDrawer::Justify::left, 0, 150, 0);
    else HUDtext.draw("$", VerticalTextDrawer::Justify::left, 0, 150, 0);
    HUDtext.drawOnSameLine(to_str(getPlayer()->money), VerticalTextDrawer::Justify::right, 0, 200, 0);
    HUDtext.draw("HP", VerticalTextDrawer::Justify::left, 150, 0, 0);
    HUDtext.drawOnSameLine(to_str(std::max(0, (int)(getPlayer()->HP + 1 - EPSILON))) + "/" + to_str((int)(getPlayer()->maxHP + 1 - EPSILON)),
                           VerticalTextDrawer::Justify::right, 200, 0, 0);
    fillRect(HUDtext.x, HUDtext.y, std::max(0.0, 1 - (p->spellSecTillCastable / p->spellSecCooldown)) * (HUDtext.maxx - HUDtext.x),
             HUDtext.h, 150, 200, 150);
    HUDtext.draw("Mana", VerticalTextDrawer::Justify::left, 0, 0, 150);
    HUDtext.drawOnSameLine(to_str((int)(getPlayer()->mana)) + "/" + to_str((int)(getPlayer()->maxMana)), VerticalTextDrawer::Justify::right,
                           0, 0, 200);
    HUDtext.fillRect(0, 0, 0, 30);
    if(currentFloor >= 6)
        HUDtext.draw("W\u00E1apons");
    else HUDtext.draw("Weapons");
    int cnt = 0;
    for(auto &i: p->weapons)
    {
        if(cnt++ == p->curWeapon)
        {
            if(i.getFireRate(p.get()) < 0.2) //don't give people seizures by weapons that fire super fast
                fillRect(HUDtext.x, HUDtext.y, HUDtext.maxx - HUDtext.x, HUDtext.h, 150, 150, 150);
            else fillRect(HUDtext.x, HUDtext.y, std::max(0.0, (1 - i.fireCounter / ((1 + Weapon::BASE_FIRERATE_IRREGULARITY[i.type]) *
                                                         i.getFireRate(p.get()))) * (HUDtext.maxx - HUDtext.x)), HUDtext.h, 150, 150, 150);
        }
        renderCopy(i.sprites[i.type], HUDtext.x, HUDtext.y, HUDtext.h, HUDtext.h);
        if(Weapon::BASE_SPELL_COST[i.type] != NOT_SET)
        {
            if(Weapon::BASE_SPELL_COST[i.type] > p->mana)
                drawText(to_str(Weapon::BASE_SPELL_COST[i.type]), HUDtext.x + HUDtext.h, HUDtext.y, HUDtext.h, 100, 100, 200);
            else drawText(to_str(Weapon::BASE_SPELL_COST[i.type]), HUDtext.x + HUDtext.h, HUDtext.y, HUDtext.h, 0, 0, 200);
        }
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
        renderCopy(Item::sprites[i.type], HUDtext.x + spriteSize * (cnt%4), HUDtext.y + spriteSize * (cnt/4), spriteSize, spriteSize);
        cnt++;
    }
}
void GameState::drawInGame()
{
    game_map.setCameraPosition(*this, getPlayer()->getX() - getFloorDisplayW()/(2.0*getPixelsPerTile()),
                                     getPlayer()->getY() - getFloorDisplayH()/(2.0*getPixelsPerTile()));
    setViewport(0, 0, getFloorDisplayW(), getFloorDisplayH());
    game_map.draw(*this);
    for(auto &i: game_map.units)
    {
        i->draw(*this);
        if(show_hitboxes)
        {
            if(!i->isPlayer())
                i->shape->drawHitbox(game_map.getCameraX(), game_map.getCameraY());
            else //the player doesn't actually rotate
            {
                double origAngle = i->shape->getAngle();
                i->shape->setAngle(0);
                i->shape->drawHitbox(game_map.getCameraX(), game_map.getCameraY());
                i->shape->setAngle(origAngle);
            }
        }
    }
    for(auto &i: game_map.units)
        i->drawHPbar(*this);
    for(auto &i: game_map.projectiles)
    {
        i.draw(*this);
        if(show_hitboxes)
            i.shape->drawHitbox(game_map.getCameraX(), game_map.getCameraY());
    }
    drawExtraAesthetics();
    setViewport(NULL);
    drawHUD();
    if(currentFloor != 17) //no minimap for kworker
    {
        game_map.drawMinimap(*this);
    }
}
static double deathTime;
void GameState::initDeathScreen()
{
    println("initializing death screen");
    game_state_state.push(GameStateState::deathScreen);
    deathTime = getTicksS();
}
void GameState::operateDeathScreen()
{
    for(auto &i: inputEvents) //let the menu access the events that we polled earlier
        SDL_PushEvent(&i);
    death_menu->operate();
}
void GameState::drawDeathScreen()
{
    operateGameInGame(true);
    drawInGame(); //honestly it's faster to keep rendering this than using SDL_RenderReadPixels
    uint8_t alpha = std::min(230, (int)((getTicksS() - deathTime)*100));
    fillRect(0, 0, getWindowW(), getWindowH(), 100, 100, 100, alpha);
    death_menu->draw(alpha, false);
}
static void (GameState::*draw_s_1)(), (GameState::*draw_s_2)(), (GameState::*init_fn)();
static double transitionStartTime;
static bool alreadyInitTransitionFn;
static const double TRANSITION_LEN = 0.2; //fade out then fade in (so total transition time is double this value)
void GameState::initTransitionScreen(void (GameState::*drawScreen1)(), void (GameState::*drawScreen2)(), void (GameState::*init)())
{
    println("initializing transition screen");
    game_state_state.push(GameStateState::transition);
    transitionStartTime = getTicksS();
    finished_transition_screen = false;
    draw_s_1 = drawScreen1;
    draw_s_2 = drawScreen2;
    init_fn = init;
    alreadyInitTransitionFn = false;
}
void GameState::operateTransitionScreen()
{
    //nothing to do here probably
}
void GameState::drawTransitionScreen()
{
    double t = getTicksS();
    if(t - transitionStartTime <= TRANSITION_LEN)
    {
        (this->*draw_s_1)();
        fillRect(0, 0, getWindowW(), getWindowH(), 0, 0, 0, 255 * (t - transitionStartTime) / TRANSITION_LEN);
    }
    else
    {
        if(!alreadyInitTransitionFn)
        {
            alreadyInitTransitionFn = true;
            if(init_fn != nullptr)
                (this->*init_fn)();
        }
        if(draw_s_2 == nullptr || t - transitionStartTime > TRANSITION_LEN * 2)
        {
            finished_transition_screen = true;
            game_state_state.pop(); //remove the transition screen from the stack cuz it's done
        }
        else //alpha value might exceed 255 so don't render if we pop
        {
            (this->*draw_s_2)();
            fillRect(0, 0, getWindowW(), getWindowH(), 0, 0, 0, 255 * (TRANSITION_LEN - (t - (transitionStartTime + TRANSITION_LEN))) / TRANSITION_LEN);
        }
    }
}
void GameState::operatePauseMenu()
{
    for(auto &i: inputEvents) //let the menu access the events that we polled earlier
        SDL_PushEvent(&i);
    if(menuStack.empty())
        print_warning("menuStack is empty");
    menuStack.top()->operate();
    if(menuStack.empty())
        game_state_state.pop();
}
void GameState::drawPauseMenu()
{
    drawInGame();
    fillRect(0, 0, getWindowW(), getWindowH(), 100, 100, 100, 230);
    menuStack.top()->draw(230, false);
}
void GameState::initConsole()
{
    println("initializing console");
    game_state_state.push(GameStateState::console);
    init_console_screen("floor " + to_str(currentFloor));
    game_state_state.push(GameStateState::transition);
    if(firstInitConsole)
    {
        firstInitConsole = false;
        initTransitionScreen(GameState::draw_main_menu, nullptr);
    }
    else initTransitionScreen(GameState::drawInGame, nullptr);
}
void GameState::draw_main_menu()
{
    main_menu->draw();
}
void GameState::draw_console_cur_floor()
{
    draw_console_screen("floor " + to_str(currentFloor));
}
void GameState::draw_console_prev_floor()
{
    draw_console_screen("floor " + to_str(currentFloor-1));
}
void GameState::operateConsole()
{
    for(auto &i: inputEvents) //let the menu access the events that we polled earlier
        SDL_PushEvent(&i);
    menuStack.top()->operate();
    if(menuStack.empty()) //ok we're supposed to advance to the next floor
    {
        game_state_state.pop();
        auto p = getPlayer();
        p->HP = std::min(p->HP + p->maxHP * 0.1, p->maxHP);
        switch(currentFloor)
        {
        case 0:
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
        case 12:
        case 14: //only show the prelevel menu on non-boss floors
            currentFloor++;
            initPrelevelMenu(GameState::draw_console_prev_floor);
            break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
        case 11:
        case 13:
        case 15:
        default:
            currentFloor++;
            initNewFloor();
            break;
        }
    }
}
void GameState::drawConsole()
{
    menuStack.top()->draw();
}
void GameState::updateExtraAesthetics()
{
    TextBox::is_first_update = TextBox::is_first_draw = true; //only show one text box at a time
    for(int i=0; i<(int)aestheticEffects.size(); i++) //handle aesthetics
    {
        aestheticEffects[i]->update(*this);
        if(aestheticEffects[i]->shouldRemove())
        {
            aestheticEffects.erase(aestheticEffects.begin() + i);
            i--;
        }
    }
}
void GameState::drawExtraAesthetics()
{
    for(auto &i: aestheticEffects)
        i->draw(*this);
}
void GameState::clearExtraAesthetics()
{
    aestheticEffects.clear();
}
