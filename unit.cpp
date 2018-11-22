#include "unit.h"
#include "sdl_base.h"
#include "math_funcs.h"
#include "game_state.h"
#include "basic_game_funcs.h"
#include <algorithm>
#include <string>
#include <cmath>
SDL_Texture *Unit::sprites[Unit::NUM_UNITS];
double Unit::BASE_MOVEMENT_SPEED[Unit::NUM_UNITS], Unit::BASE_HP[Unit::NUM_UNITS];
GeometricObject::Type Unit::BASE_SHAPE[Unit::NUM_UNITS];
double Unit::BASE_SIZE1[Unit::NUM_UNITS], Unit::BASE_VISION_DISTANCE[Unit::NUM_UNITS];
Unit::MovementBehavior Unit::BASE_MOVEMENT_BEHAVIOR[Unit::NUM_UNITS];
Unit::WeaponBehavior Unit::BASE_WEAPON_BEHAVIOR[Unit::NUM_UNITS];
Unit::RotationBehavior Unit::BASE_ROTATION_BEHAVIOR[Unit::NUM_UNITS];
double (*Unit::BASE_ROTATION_FUNC[Unit::NUM_UNITS])(double);
bool Unit::BASE_IS_SPECTRAL[Unit::NUM_UNITS];
double Unit::BASE_DAMAGE_ON_COLLISION[Unit::NUM_UNITS];
static_array<Point> Unit::BASE_VERTICES[Unit::NUM_UNITS];
int Unit::BASE_WEAPON[Unit::NUM_UNITS];
int Unit::IDcounter = 0;
double Unit::BASE_PROJECTILE_OFFSET[Unit::NUM_UNITS];
double Unit::BASE_MONEY_REWARD[Unit::NUM_UNITS];
void (*Unit::BASE_OTHER_BEHAVIOR[NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle);
std::string Unit::BASE_NAME[Unit::NUM_UNITS];
Unit::Unit(int type, Affiliation affiliation, double x, double y)
{
    this->creationTime = getTicksNs() / 1e9;
    this->ID = IDcounter++;
    this->showHPbar = true;
    this->curWeapon = NOT_SET;
    this->type = type;
    this->origAffiliation = affiliation; //the original affiliation must be kept track of in case it changes for whatever reason
    resetToBaseStats();
    this->HP = this->maxHP;
    if(BASE_WEAPON[type] != NOT_SET)
    {
        curWeapon = 0;
        weapons.emplace_back(BASE_WEAPON[type]);
    }
    switch(BASE_SHAPE[type])
    {
    case GeometricObject::Type::circle:
        this->shape = std::make_unique<Circle>(x, y, BASE_SIZE1[type]);
        break;
    case GeometricObject::Type::square:
        this->shape = std::make_unique<Square>(x, y, BASE_SIZE1[type]);
        break;
    case GeometricObject::Type::polygon:
        this->shape = std::make_unique<Polygon>(x, y, BASE_VERTICES[type]);
        break;
    default:
        print_error("attempting to construct a unit with unknown shape " + to_str((int)BASE_SHAPE[type]));
    }
}
void Unit::resetToBaseStats()
{
    this->HPregen = 0;
    this->affiliation = this->origAffiliation;
    this->movementSpeed = BASE_MOVEMENT_SPEED[type];
    this->maxHP = BASE_HP[type];
    this->visionDistance = BASE_VISION_DISTANCE[type];
    this->isSpectral = BASE_IS_SPECTRAL[type];
    this->damageOnCollision = BASE_DAMAGE_ON_COLLISION[type];
    this->omnivamp = 0;
    this->damageDealtMult = 1;
    this->damageReceivedMult = 1;
    this->HPregenOnKill = 0;
    this->timeWhenLastTookDamage = -INF;
}
bool Unit::compare_y_with_unit(const std::shared_ptr<Unit> &a, const std::shared_ptr<Unit> &b)
{
    return a->getY() < b->getY();
}
bool Unit::compare_y_with_double(const std::shared_ptr<Unit> &a, const double &b)
{
    return a->getY() < b;
}
bool Unit::isPlayer() const
{
    return type == 0;
}
void Unit::applyMod(const Item &item)
{
    double xDamageOnCollision = 1, xMovementSpeed = 1;
    switch(item.name)
    {
    case Item::Name::SpikedVest:
        xDamageOnCollision += 1;
        break;
    case Item::Name::BlueBoots:
        xMovementSpeed += 0.1;
        break;
    case Item::Name::SadistsMachete:
        omnivamp += 0.02;
        damageDealtMult += 0.05;
        break;
    case Item::Name::RedPillow:
        maxHP += 20;
        break;
    case Item::Name::MysterialVial:
        HPregen += 0.1;
        break;
    case Item::Name::MysteriousPhial:
        damageDealtMult += 0.15;
        damageReceivedMult += 0.15;
        break;
    case Item::Name::TheArtOfSadism:
        HPregenOnKill += 1;
        break;
    default:
        break;
    }
    damageOnCollision *= xDamageOnCollision;
    movementSpeed *= xMovementSpeed;
}
double Unit::getAgeSeconds() const
{
    return getTicksS() - creationTime;
}
void Unit::applyMods(const std::vector<Item> &items)
{
    resetToBaseStats();
    for(auto &i: items)
        applyMod(i);
}
bool Unit::hasItem(Item::Name name) const
{
    for(auto &i: items)
        if(i.name == name)
            return true;
    return false;
}
double Unit::getCenterDistanceFromClosestEnemy(const GameMap &game_map) const
{
    double distance = INF;
    for(auto &i: game_map.units)
        distance = std::min(distance, std::hypot(getX() - i->getX(), getY() - i->getY()));
    return distance;
}
double Unit::getMovementSpeed(const GameMap &game_map) const
{
    double ms = movementSpeed;
    double xMS = 1;
    for(auto &i: tempMods)
    {
        switch(i.type)
        {
        case 0: //multiplicative movement speed
            ms *= i.vars[0];
            break;
        case 1: //decaying multiplicative movement speed
            ms *= (1 + (i.vars[0] - 1) * i.timeLeft / i.maxTimeLeft);
            break;
        case 2: //additive decaying movement speed
            xMS += (i.vars[0] - 1) * i.timeLeft / i.maxTimeLeft;
            break;
        }
    }
    for(auto &i: items)
    {
        switch(i.name)
        {
        case Item::Name::AmuletOfAir:
            xMS += std::min(0.15, 0.0375 * getCenterDistanceFromClosestEnemy(game_map));
            break;
        case Item::Name::Toothbrush:
            xMS += std::max(0.0, 0.6 * (60 - std::max(game_map.floorTimeLeft, 0.0)));
            break;
        default:
            break;
        }
    }
    return ms * xMS;
}
double Unit::dist(const Unit *other) const
{
    return shape->centerDist(other->shape.get());
}
void Unit::updateTemporaryMods()
{
    for(int i=0; i<(int)tempMods.size(); i++)
    {
        tempMods[i].timeLeft -= TICK_SPEED;
        if(tempMods[i].timeLeft <= 0)
        {
            tempMods.erase(tempMods.begin() + i);
            i--;
        }
    }
}
void Unit::moveToNewPosition(GameState &game_state, double dX, double dY)
{
    double origX = getX(), origY = getY();
    Unit::tryMovingToPosition(game_state, dX, dY);
    double d = std::hypot(moveX, moveY);
    double ms_per_frame = getMovementSpeed(game_state.game_map) * TICK_SPEED;
    if(d * 1.001 < ms_per_frame) //don't get stuck on walls
    {
        if(dX != 0) //shouldn't matter whether you try x or y first
            Unit::tryMovingToPosition(game_state, sign(dX)*std::sqrt(square(ms_per_frame) - square(d)), 0);
        d = std::hypot(getX() - origX, getY() - origY); //reset d and try to move in the y direction if you can
        if(d * 1.001 < ms_per_frame)
        {
            if(dY != 0) //shouldn't matter whether you try x or y first
                Unit::tryMovingToPosition(game_state, 0, sign(dY)*std::sqrt(square(ms_per_frame) - square(d)));
        }
    }
}
void Unit::tryMovingToPosition(GameState &game_state, double dX, double dY)
{
    double origX = getX(), origY = getY();
    if(std::fabs(dX)>0 || std::fabs(dY)>0)
    {
        setX(origX + dX);
        setY(origY + dY);
        //note that the order is important here. check terrain first to avoid unit collisions diagonally through terrain.
        if(collidesWithTerrain(game_state.game_map) || collidesWithOtherUnit(game_state))
        {
            setX(origX);
            setY(origY);
        }
    }
    //if a unit hits a player it won't start moving in another direction e.g. moveX and moveY won't change
    if(!game_state.game_map.alreadyCollided.count(std::minmax(ID, game_state.getPlayer()->ID)))
    {
        moveX = (getX() - origX) / TICK_SPEED;
        moveY = (getY() - origY) / TICK_SPEED;
    }

}
void Unit::rotateToNewAngle(GameState &game_state, double dAngle)
{
    double origAngle = getAngle();
    setAngle(origAngle + dAngle);
    if(collidesWithTerrain(game_state.game_map) || collidesWithOtherUnit(game_state))
    {
        double lo = origAngle, hi = origAngle + dAngle;
        for(int iter=0; iter<5; iter++)
        {
            double mid = (lo + hi) / 2;
            setAngle(mid);
            if(collidesWithTerrain(game_state.game_map) || collidesWithOtherUnit(game_state))
                hi = mid;
            else lo = mid;
        }
        setAngle(lo); //it's safer to go with lo because otherwise units that are supposed to be stuck will still rotate
    }
    if(!game_state.game_map.alreadyCollided.count(std::minmax(ID, game_state.getPlayer()->ID)))
    {
        moveAngle = (getAngle() - origAngle) / TICK_SPEED;
    }
}
//this function speeds calculations up by trying to rotate and translate in one go
bool Unit::tryToMoveToNewPositionAndAngle(GameState &game_state, double dX, double dY, double dAngle)
{
    double origAngle = getAngle();
    double origX = getX(), origY = getY();
    setAngle(origAngle + dAngle);
    setX(origX + dX);
    setY(origY + dY);
    if(collidesWithTerrain(game_state.game_map) || collidesWithOtherUnit(game_state))
    {
        setX(origX);
        setY(origY);
        setAngle(origAngle);
        return false; //failed to move to new position and angle
    }
    moveX = getX() - origX;
    moveY = getY() - origY;
    moveAngle = getAngle() - origAngle;
    return true; //successfully moved to new position and angle
}
void Unit::setInitialPosition(double x, double y)
{
    setX(x);
    setY(y);
}
bool Unit::collidesWithTerrain(GameMap &game_map) const
{
    return shape->collidesWithTerrain(game_map);
}
void Unit::addTemporaryMod(int type)
{
    for(auto &i: tempMods)
    {
        if(i.type == type) //if it's already affected by this temporary mod then just refresh the timer
        {
            i.timeLeft = i.maxTimeLeft;
            return;
        }
    }
    //if we reach here it doesn't have this temporary mod yet so add it
    switch(type)
    {
    case 0: //dirty serrated shirt
        tempMods.emplace_back(0, 0.80, 0.35);
        break;
    case 1: //red boots
        tempMods.emplace_back(2, 0.5, 1.3);
        break;
    case 2: //green boots
        tempMods.emplace_back(2, 0.65, 1.4);
        break;
    }
}
//this is a one-sided function because collision damage dealing is mutual, unlike general damage
void Unit::handleCollision(Unit &other) //the other unit applies its collision effects to this unit
{
    if(isDead() || other.isDead()) //don't interact with dead units
        return;
    Unit::dealDamage(&other, this, BASE_DAMAGE_ON_COLLISION[other.type] / BASE_FPS);
    for(auto &i: other.items) //item interactions with collisions
    {
        switch(i.name)
        {
        case Item::Name::DirtySerratedShirt:
            addTemporaryMod(0);
            break;
        default:
            break;
        }
    }
}
bool Unit::collidesWithOtherUnit_checkToValidateMap(std::vector<std::shared_ptr<Unit> > units) const
{
    for(auto &i: units)
    {
        if(shape->collidesWith(i->shape.get()))
        {
            return true;
        }
    }
    return false;
}
bool Unit::collidesWithOtherUnit(GameState &game_state) //depending on onlyCheck, may also modify the units if they do collide
{
    if(isSpectral)
        return false;
    bool collides = false;
    //naive algorithm that doesn't use partitioning
    /*for(auto &i: game_state.game_map.units)
    {
        if(!i->isSpectral && ID!=i->ID && !i->isDead() && shape->collidesWith(i->shape.get()))
        {
            if(areEnemies(affiliation, i->affiliation))
            {
                pii IDpair = std::minmax(ID, i->ID);
                if(!game_state.game_map.alreadyCollided.count(IDpair))
                {
                    handleCollision(*i);
                    i->handleCollision(*this);
                    game_state.game_map.alreadyCollided.insert(IDpair);
                }
            }
            collides = true;
        }
    }*/
    auto &uPart = game_state.unitPartition;
    int minx = std::max(0, (int)(getX() + 0.5 - shape->getRadius() - PARTITION_DIST));
    int maxx = std::min((int)uPart.size()-1, (int)(getX() + 0.5 + shape->getRadius() + PARTITION_DIST));
    for(int i=minx; i<=maxx; i++)
    {
        auto miny = std::lower_bound(uPart[i].begin(), uPart[i].end(), getY() - shape->getRadius() - PARTITION_DIST, Unit::compare_y_with_double);
        auto maxy = std::lower_bound(miny, uPart[i].end(), getY() + shape->getRadius() + PARTITION_DIST + 0.001, Unit::compare_y_with_double);
        while(miny < maxy)
        {
            auto &j = *miny;
            if(!j->isSpectral && ID!=j->ID && !j->isDead() && shape->collidesWith(j->shape.get()))
            {
                if(areEnemies(affiliation, j->affiliation))
                {
                    pii IDpair = std::minmax(ID, j->ID);
                    if(!game_state.game_map.alreadyCollided.count(IDpair))
                    {
                        handleCollision(*j);
                        j->handleCollision(*this);
                        game_state.game_map.alreadyCollided.insert(IDpair);
                    }
                }
                collides = true;
            }
            miny++;
        }
    }
    return collides;
}
void Unit::draw(const GameState &game_state) const
{
    int ppt = game_state.getPixelsPerTile();
    int rw, rh;
    SDL_QueryTexture(sprites[type], NULL, NULL, &rw, &rh);
    int rx = (getX() - game_state.game_map.getCameraX())*ppt - rw * SPRITE_SCALE / 2;
    int ry = (getY() - game_state.game_map.getCameraY())*ppt - rh * SPRITE_SCALE / 2;
    double t = getTicksS() - timeWhenLastTookDamage;
    if(t < 0.4)
        SDL_SetTextureColorMod(sprites[type], 135 + t*300, 135 + t*300, 135 + t*300);
    if(isPlayer())
        renderCopy(sprites[type], rx, ry, rw * SPRITE_SCALE, rh * SPRITE_SCALE);
    else renderCopyEx(sprites[type], rx, ry, rw * SPRITE_SCALE, rh * SPRITE_SCALE, TO_DEG(shape->getAngle()));
    if(t < 1)
        SDL_SetTextureColorMod(sprites[type], 255, 255, 255);
    if(!weapons.empty())
    {
        double angle = getAngle();
        weapons[curWeapon].draw_wield(game_state, game_state.getFloorDisplayW()/2 + ppt*std::cos(angle)/4, game_state.getFloorDisplayH()/2 + ppt*std::sin(angle)/4, angle);
    }
    if(showHPbar)
    {
        int W = shape->getRadius()*1.4 * ppt;
        int H = W * 0.2;
        int X = (getX() - game_state.game_map.getCameraX()) * ppt - W/2;
        int Y = (getY() - game_state.game_map.getCameraY()) * ppt - W*0.55 - H;
        double hpPercent = HP / maxHP;
        fillRect(X, Y, hpPercent * W, H, 0, 255, 0, 150);
        fillRect(X + hpPercent*W, Y, (1 - hpPercent)*W, H, 255, 0, 0, 150);
        drawRect(X, Y, W, H, 0, 0, 0);
    }
}
void Unit::moveRandomly(const GameMap &game_map, double k)
{
    moveX += getMovementSpeed(game_map) * k  * (randf() - 0.5) * TICK_SPEED;
    moveY += getMovementSpeed(game_map) * k  * (randf() - 0.5) * TICK_SPEED;
}
void Unit::operate(GameState &game_state)
{
    if(isDead() || getMovementSpeed(game_state.game_map)==0)
        return;
    HP += HPregen * TICK_SPEED;
    auto p = game_state.getPlayer();
    switch(BASE_MOVEMENT_BEHAVIOR[type])
    {
    case MovementBehavior::player:
    case MovementBehavior::none:
        break;
    case MovementBehavior::cardinal:
        if(moveX==0 && moveY==0)
        {
            switch(randz(0, 3))
            {
            case 0:
                moveX = 1;
                moveY = 0;
                break;
            case 1:
                moveX = -1;
                moveY = 0;
                break;
            case 2:
                moveX = 0;
                moveY = -1;
                break;
            case 3:
                moveX = 0;
                moveY = 1;
                break;
            }
        }
        break;
    case MovementBehavior::bouncy:
        if(moveX==0 && moveY==0)
        {
            moveX = randf() - 0.5;
            moveY = randf() - 0.5;
        }
        break;
    case MovementBehavior::homing_erratic:
        if(shape->centerDist(p->shape.get()) < visionDistance)
        {
            moveRandomly(game_state.game_map, 1);
            moveX += 5 * sign(p->getX() - getX()) * TICK_SPEED;
            moveY += 5 * sign(p->getY() - getY()) * TICK_SPEED;
            break;
        }
        else moveRandomly(game_state.game_map, 100);
        break;
    case MovementBehavior::homing_simple:
        if(shape->centerDist(p->shape.get()) < visionDistance)
        {
            moveX = p->getX() - getX();
            moveY = p->getY() - getY();
            break;
        }
        else moveRandomly(game_state.game_map, 100);
        break;
    case MovementBehavior::random:
        moveRandomly(game_state.game_map, 100);
        break;
    default:
        print_warning("unknown unit movement behavior " + to_str((int)BASE_MOVEMENT_BEHAVIOR[type]));
    }
    //dX, dY, and dAngle represent where the unit wants to move
    //moveX, moveY, and moveAngle represent the unit's actual movement last frame (until they're updated this frame)
    normToHypot(moveX, moveY, getMovementSpeed(game_state.game_map));
    double dX = moveX, dY = moveY;
    moveToNewPosition(game_state, moveX * TICK_SPEED, moveY * TICK_SPEED);
    double dAngle = 0;
    switch(BASE_ROTATION_BEHAVIOR[type])
    {
    case RotationBehavior::player:
    case RotationBehavior::none:
        dAngle = 0;
        break;
    case RotationBehavior::constant:
        dAngle = BASE_ROTATION_FUNC[type](getAgeSeconds());
        rotateToNewAngle(game_state, dAngle * TICK_SPEED);
        break;
    case RotationBehavior::constant_bidirectional:
        dAngle = BASE_ROTATION_FUNC[type](getAgeSeconds());
        if(moveAngle == 0)
        {
            moveX = moveY = 0;
            if(randz(0, 1) == 0)
                dAngle = -dAngle;
            rotateToNewAngle(game_state, dAngle * TICK_SPEED);
            if(moveAngle == 0)
                rotateToNewAngle(game_state, -dAngle * TICK_SPEED);
        }
        else rotateToNewAngle(game_state, sign(moveAngle) * dAngle * TICK_SPEED);
        break;
    }
    //faster but results in messier code
    /*double origMoveX = moveX, origMoveY = moveY;
    if(!tryToMoveToNewPositionAndAngle(game_state, moveX / ITER_PER_FRAME, moveY / ITER_PER_FRAME, dAngle / ITER_PER_FRAME))
    {
        moveToNewPosition(game_state, origMoveX / ITER_PER_FRAME, origMoveY / ITER_PER_FRAME);
        if(dAngle != 0)
            rotateToNewAngle(game_state, dAngle / ITER_PER_FRAME);
    }*/
    if(curWeapon != NOT_SET)
    {
        switch(BASE_WEAPON_BEHAVIOR[type]) //non player units fire ALL of the weapons they have
        {
        case WeaponBehavior::player:
            weapons[curWeapon].update(game_state, shared_from_this(), isMouseButtonPressed(SDL_BUTTON_LEFT));
            break;
        case WeaponBehavior::none:
            break;
        case WeaponBehavior::random:
            break;
        case WeaponBehavior::line_to_player:
            break;
        case WeaponBehavior::same_angle:
            for(auto &i: weapons)
                i.update(game_state, shared_from_this(), true);
            break;
        }
    }
    if(BASE_OTHER_BEHAVIOR[type] != nullptr)
    {
        BASE_OTHER_BEHAVIOR[type](game_state, shared_from_this(), dX, dY, dAngle);
    }
}
void Unit::dealDamage(Unit *a, Unit *b, double damage) //a deals damage to b
{
    if(b == nullptr)
        print_error("Unit *b == nullptr");
    //we have to make sure not to interact with dead units but to still interact with their projectiles
    if(a != nullptr)
        damage *= a->damageDealtMult;
    damage *= b->damageReceivedMult;
    b->HP -= damage;
    if(a != nullptr) //damage applied by the floor after the floor timer ends shouldn't make the player's color change
        b->timeWhenLastTookDamage = getTicksS();
    for(auto &i: b->items)
    {
        switch(i.name)
        {
        case Item::Name::GreenBoots:
            b->addTemporaryMod(2);
            break;
        default:
            break;
        }
    }
    if(a != nullptr)
    {
        if(b->isDead())
        {
            a->money += BASE_MONEY_REWARD[b->type]; //should only affect the player though
            a->HP += a->HPregenOnKill;
        }
        a->HP += damage * a->omnivamp;
        a->HP = std::min(a->HP, a->maxHP);
        for(auto &i: a->items)
        {
            switch(i.name)
            {
            case Item::Name::RedBoots:
                a->addTemporaryMod(1);
                break;
            default:
                break;
            }
        }
    }
}
bool Unit::isDead() const
{
    return HP <= 0;
}
TemporaryMod::TemporaryMod(int type, double timeLeft, double var1)
{
    this->type = type;
    this->timeLeft = this->maxTimeLeft = timeLeft;
    vars.push_back(var1);
}
TemporaryMod::TemporaryMod(int type, double timeLeft, double var1, double var2)
{
    this->type = type;
    this->timeLeft = this->maxTimeLeft = timeLeft;
    vars.push_back(var1);
    vars.push_back(var2);
}
