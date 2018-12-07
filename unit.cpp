#include "unit.h"
#include "sdl_base.h"
#include "math_funcs.h"
#include "game_state.h"
#include "basic_game_funcs.h"
#include <algorithm>
#include <string>
#include <cmath>
SDL_Texture* (*Unit::GEN_CUSTOM_TEXTURE[NUM_UNITS])();
SDL_Texture *Unit::sprites[Unit::NUM_UNITS];
double Unit::BASE_MOVEMENT_SPEED[Unit::NUM_UNITS], Unit::BASE_HP[Unit::NUM_UNITS];
GeometricObject::Type Unit::BASE_SHAPE[Unit::NUM_UNITS];
double Unit::BASE_SIZE1[Unit::NUM_UNITS], Unit::BASE_VISION_DISTANCE[Unit::NUM_UNITS];
Unit::MovementBehavior Unit::BASE_MOVEMENT_BEHAVIOR[Unit::NUM_UNITS];
Unit::WeaponBehavior Unit::BASE_WEAPON_BEHAVIOR[Unit::NUM_UNITS];
Unit::RotationBehavior Unit::BASE_ROTATION_BEHAVIOR[Unit::NUM_UNITS];
double (*Unit::BASE_ROTATION_FUNC[Unit::NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit);
void (*Unit::BASE_MOVEMENT_FUNC[Unit::NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit);
void (*Unit::BASE_ON_DEATH_FUNC[Unit::NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit);
double Unit::BASE_DAMAGE_ON_COLLISION[Unit::NUM_UNITS];
static_array<Point> Unit::BASE_VERTICES[Unit::NUM_UNITS];
int Unit::BASE_WEAPON[Unit::NUM_UNITS];
int Unit::IDcounter = 0;
double Unit::BASE_HP_REGEN[Unit::NUM_UNITS];
double Unit::BASE_PROJECTILE_OFFSET[Unit::NUM_UNITS];
double Unit::BASE_MONEY_REWARD[Unit::NUM_UNITS];
void (*Unit::BASE_OTHER_BEHAVIOR[Unit::NUM_UNITS])(GameState &game_state, const std::shared_ptr<Unit> &unit, double dX, double dY, double dAngle);
void (*Unit::BASE_TAKE_DAMAGE_FUNC[Unit::NUM_UNITS])(Unit *a, Unit *b, double damage);
bool Unit::BASE_IS_SPECTRAL[Unit::NUM_UNITS];
std::string Unit::BASE_NAME[Unit::NUM_UNITS];
Unit::Unit(int type, Affiliation affiliation, double x, double y)
{
    this->ID = IDcounter++;
    this->curWeapon = NOT_SET;
    this->type = type;
    if(GEN_CUSTOM_TEXTURE[type] == nullptr)
        this->sprite = sprites[type];
    else this->sprite = GEN_CUSTOM_TEXTURE[type]();
    this->origAffiliation = affiliation; //the original affiliation must be kept track of in case it changes for whatever reason
    resetToBaseStats();
    this->HP = this->maxHP;
    this->showHPbar = (this->maxHP != INF);
    this->mana = this->maxMana;
    this->rand_var = randf(); //we might want random behavior in the same classes of units to make the game less predictable
    for(auto &i: misc_var)
        i = 0;
    if(BASE_WEAPON[type] != NOT_SET) //probably deprecated
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
    tempMods.clear(); //temporary mods do not carry across floors
    this->age = 0;
    this->moveX = this->moveY = this->moveAngle = 0;
    this->time_since_stuck = INF;
    this->time_since_took_damage = INF;
    this->HPregen = BASE_HP_REGEN[type];
    this->affiliation = this->origAffiliation;
    this->movementSpeed = BASE_MOVEMENT_SPEED[type];
    this->maxHP = BASE_HP[type];
    this->visionDistance = BASE_VISION_DISTANCE[type];
    this->isSpectral = BASE_IS_SPECTRAL[type];
    this->damageOnCollision = BASE_DAMAGE_ON_COLLISION[type];
    this->omnivamp = 0;
    this->damageDealtMult = 1;
    this->cdMult = 1;
    this->is_active = true;
    if(isPlayer()) //testing purposes
    {
        //this->damageDealtMult *= 2;
        this->movementSpeed *= 1.2;
        this->maxHP = 200;
        bool has = false;
        for(int i=0; i<(int)weapons.size(); i++)
            if(weapons[i].type == (int)Weapon::Name::Laser)
                has = true;
        if(!has)
        {
            weapons.emplace_back(Weapon::Name::Laser);
            weapons.back().ammo = INF;
        }
    }
    this->damageReceivedMult = 1;
    this->HPregenOnKill = 0;
    this->maxMana = 50; //should only be used for the player
    this->manaRegen = 0.1; //player only
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
    return type==0 || type==32;
}
void Unit::die(GameState &game_state)
{
    if(BASE_ON_DEATH_FUNC[type] != nullptr)
        BASE_ON_DEATH_FUNC[type](game_state, shared_from_this());
}
void Unit::applyMods(const std::vector<Item> &items)
{
    resetToBaseStats();
    double xDamageOnCollision = 1, xMovementSpeed = 1;
    for(auto &i: items)
    {
        switch((Item::Name)i.type)
        {
        case Item::Name::SpikedVest:
            xDamageOnCollision += 1;
            break;
        case Item::Name::BlueBoots:
            xMovementSpeed += 0.05;
            break;
        case Item::Name::SadistsMachete:
            omnivamp += 0.01;
            damageDealtMult += 0.05;
            break;
        case Item::Name::RedPillow:
            maxHP += 20;
            break;
        case Item::Name::MysteriousVial:
            HPregen += 0.2;
            break;
        case Item::Name::MysteriousPhial:
            damageDealtMult += 0.15;
            damageReceivedMult += 0.15;
            break;
        case Item::Name::TheArtOfSadism:
            HPregenOnKill += 2;
            break;
        case Item::Name::BlueTome:
            manaRegen += 0.3;
            break;
        case Item::Name::PhilosophersBook:
            maxMana += 25;
            break;
        case Item::Name::PurpleBoots:
            xMovementSpeed += 0.1;
            break;
        case Item::Name::TheSadistManifesto:
            omnivamp += 0.03;
            break;
        case Item::Name::IllegallyMadeFurCoat:
            HPregen += 0.3;
            maxHP += 20;
            break;
        case Item::Name::WaifuBodyPillow:
            xMovementSpeed += 0.1;
            break;
        case Item::Name::FloppyDisk_1:
            xMovementSpeed += 0.15;
            break;
        case Item::Name::Aspirin:
            maxHP += 50;
            break;
        case Item::Name::ArcheryLesson_1:
        case Item::Name::ArcheryLesson_2:
            maxHP -= 10;
            break;
        case Item::Name::ArcheryLesson_3:
            maxMana -= 5;
            break;
        case Item::Name::Fentanyl:
            maxHP += 80;
            break;
        case Item::Name::CSGOSteamCode:
            maxMana += 50;
            break;
        case Item::Name::Kworker_Mod:
            xMovementSpeed += 0.4;
            break;
        default:
            break;
        }
    }
    damageOnCollision *= xDamageOnCollision;
    movementSpeed *= xMovementSpeed;
    if(isPlayer())
    {
        damageReceivedMult *= DIFFICULTY;
        if(DIFFICULTY < 1)
            damageDealtMult /= DIFFICULTY;
    }
}
bool Unit::hasItem(Item::Name name) const
{
    for(auto &i: items)
        if((Item::Name)i.type == name)
            return true;
    for(auto &i: unshownItems)
        if((Item::Name)i.type == name)
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
double Unit::getMovementSpeed(const GameState &game_state) const
{
    double ms, xMS;
    TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::MovementSpeed, movementSpeed, &ms, &xMS);
    //note: should be ok to use static variables if we need to because only the player can own items anyway (for now at least) and there's only one player
    for(auto &i: items)
    {
        switch((Item::Name)i.type)
        {
        case Item::Name::AmuletOfAir:
            xMS += std::min(0.10, 0.025 * getCenterDistanceFromClosestEnemy(game_state.game_map));
            break;
        case Item::Name::Toothbrush:
            xMS += std::max(0.0, 1.0 * (30 - std::max(game_state.game_map.floorTimeLeft, 0.0)));
            break;
        case Item::Name::LaggyRouter:
            ms *= 1.4 + 0.5 * std::sin(age + 2 * std::sin(3*age + 4));
            break;
        case Item::Name::CSGOSteamCode:
            ms *= std::min(1.0, 0.8 + 0.2 * time_since_took_damage);
            break;
        default:
            break;
        }
    }
    if(!isPlayer()) //the player's items may negatively affect other units
    {
        auto p = game_state.getPlayer();
        for(auto &i: p->items)
        {
            switch((Item::Name)i.type)
            {
            case Item::Name::MahouShoujoTshirt:
                ms *= std::min(1.0, 0.2 + 0.2 * dist(p.get()));
                break;
            default:
                break;
            }
        }
    }
    ms *= xMS;
    if(!isPlayer())
        ms *= 1 + (DIFFICULTY - 1) / 5;
    return ms;
}
double Unit::getVisionDistance(const GameState &game_state) const
{
    double vd = visionDistance;
    if(game_state.getPlayer()->hasItem(Item::Name::ShowerPass))
        vd *= 0.6666;
    return vd;
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
    double ms_per_frame = getMovementSpeed(game_state) * TICK_SPEED;
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
    if(isPlayer() && game_state.game_map.gravity!=0 && dX!=NOT_SET && dY!=NOT_SET)
    {
        tryMovingToPosition(game_state, dX, NOT_SET);
        double origMoveX = moveX;
        tryMovingToPosition(game_state, NOT_SET, dY);
        moveX = origMoveX;
        if(dY < 0 && moveY==0) //prevent sticking to ceilings
            moveY = 1e-9;
    }
    else
    {
        if(dX == NOT_SET)
            dX = 0;
        if(dY == NOT_SET)
            dY = 0;
        double origX = getX(), origY = getY();
        if(std::fabs(dX)>0 || std::fabs(dY)>0)
        {
            setX(origX + dX);
            setY(origY + dY);
            //note that the order might be important here. check terrain first to avoid unit collisions diagonally through terrain.
            if(!isSpectral && (collidesWithTerrain(game_state.game_map) || collidesWithOtherUnit(game_state)))
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
}
void Unit::rotateToNewAngle(GameState &game_state, double dAngle)
{
    double origAngle = getAngle();
    setAngle(origAngle + dAngle);
    if(collidesWithTerrain(game_state.game_map) || collidesWithOtherUnit(game_state))
    {
        setAngle(origAngle);
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
bool Unit::collidesWithTerrain(const GenericMapRoom &room) const
{
    if(affiliation != Affiliation::player) //enemies shouldn't be on the connecting tiles
        return shape->collidesWithTerrain(room) || shape->collidesWithTile(room, MapTile::connection);
    return shape->collidesWithTerrain(room);
}
void Unit::addTemporaryMod(TemporaryMod::Name name, TemporaryMod::Mod mod, TemporaryMod::Type type, double duration, double var1)
{
    for(auto &i: tempMods)
    {
        if(name == i.name) //if it's already affected by this temporary mod then just refresh the timer
        {
            i.timeLeft = i.maxTimeLeft;
            return;
        }
    }
    //if we reach here it doesn't have this temporary mod yet so add it
    tempMods.emplace_back(name, mod, type, duration, var1);
}
//this is a one-sided function because collision damage dealing is mutual, unlike general damage
void Unit::handleCollision(Unit &other) //the other unit applies its collision effects to this unit
{
    if(isDead() || other.isDead()) //don't interact with dead units
        return;
    Unit::dealDamage(&other, this, BASE_DAMAGE_ON_COLLISION[other.type] / BASE_FPS);
    typedef TemporaryMod Tmod;
    for(auto &i: other.items) //item interactions with collisions
    {
        switch((Item::Name)i.type)
        {
        case Item::Name::DirtySerratedShirt:
            addTemporaryMod(Tmod::Name::DirtySerratedShirt, Tmod::Mod::MovementSpeed, Tmod::Type::Multiplicative, 0.50, 0.35);
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
    auto &uPartY = game_state.unitPartitionY;
    int minx = std::max(0, (int)(getX() + 0.5 - shape->getRadius() - PARTITION_DIST));
    int maxx = std::min((int)uPart.size()-1, (int)(getX() + 0.5 + shape->getRadius() + PARTITION_DIST));
    for(int i=minx; i<=maxx; i++)
    {
        auto miny = std::lower_bound(uPartY[i].begin(), uPartY[i].end(), getY() - shape->getRadius() - PARTITION_DIST);
        auto maxy = std::lower_bound(miny, uPartY[i].end(), getY() + shape->getRadius() + PARTITION_DIST + 0.001);
        int last = maxy - uPartY[i].begin();
        for(int curY = miny - uPartY[i].begin(); curY < last; curY++)
        {
            auto &j = uPart[i][curY];
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
    if(!isDrawable(game_state)) //it's in a room that hasn't already been found
        return;
    int ppt = getPixelsPerTile();
    int rw, rh;
    SDL_QueryTexture(sprite, NULL, NULL, &rw, &rh);
    int rx = (getX() - game_state.game_map.getCameraX())*ppt - rw * SPRITE_SCALE / 2;
    int ry = (getY() - game_state.game_map.getCameraY())*ppt - rh * SPRITE_SCALE / 2;
    double t = time_since_took_damage;
    if(t < 0.4)
        SDL_SetTextureColorMod(sprite, 135 + t*300, 135 + t*300, 135 + t*300);
    if(isPlayer())
        renderCopy(sprite, rx, ry, rw * SPRITE_SCALE, rh * SPRITE_SCALE);
    else renderCopyEx(sprite, rx, ry, rw * SPRITE_SCALE, rh * SPRITE_SCALE, TO_DEG(shape->getAngle()));
    if(t < 1) //reset color, we can use any value greater than 0.4 + SECONDS_PER_FRAME
        SDL_SetTextureColorMod(sprite, 255, 255, 255);
    if(!weapons.empty())
    {
        double angle = getAngle();
        if(!isPlayer() || finished_transition_screen) //don't draw the weapon for the player if transitioning cuz it looks weird when the weapon suddenly rotates towards the mouse cursor
            weapons[curWeapon].draw_wield(game_state, rx + ppt/2 + ppt*std::cos(angle)/4, ry + ppt/2 + ppt*std::sin(angle)/4, angle);
    }
}
void Unit::drawHPbar(const GameState &game_state) const
{
    if(!isDrawable(game_state)) //it's in a room that hasn't already been found
        return;
    int ppt = getPixelsPerTile();
    if(showHPbar)
    {
        int W = shape->getRadius()*1.4 * ppt;
        if(shape->type == GeometricObject::Type::circle)
             W *= 1.4;
        int H = W * 0.2;
        int X = (getX() - game_state.game_map.getCameraX()) * ppt - W/2;
        int Y = (getY() - game_state.game_map.getCameraY()) * ppt - W*0.55 - H;
        double hpPercent = HP / maxHP;
        fillRect(X, Y, hpPercent * W, H, 0, 255, 0, 150);
        fillRect(X + hpPercent*W, Y, (1 - hpPercent)*W, H, 255, 0, 0, 150);
        drawRect(X, Y, W, H, 0, 0, 0);
    }
}
void Unit::moveRandomly(const GameState &game_state, double k)
{
    //sqrt is used here because that's empirically (and there might be a random-walk related proof) the "best" across all possible values for TICK_SPEED
    moveX += getMovementSpeed(game_state) * k  * (randf() - 0.5) * 0.05 * std::sqrt(TICK_SPEED);
    moveY += getMovementSpeed(game_state) * k  * (randf() - 0.5) * 0.05 * std::sqrt(TICK_SPEED);
}
void Unit::operate(GameState &game_state)
{
    if(isDead())
        return;
    age += TICK_SPEED;
    HP += HPregen * TICK_SPEED;
    HP = std::min(HP, maxHP);
    mana += manaRegen * TICK_SPEED;
    mana = std::min(mana, maxMana);
    spellSecTillCastable -= TICK_SPEED;
    time_since_took_damage += TICK_SPEED;
    for(auto &i: weapons) //update all of them, not just the currently wielded one
        i.updateTemporaryMods();
    if(!isActive(game_state))
        return;
    auto p = game_state.getPlayer();
    switch(BASE_MOVEMENT_BEHAVIOR[type])
    {
    case MovementBehavior::player:
    case MovementBehavior::none:
    case MovementBehavior::pure_custom:
        break;
    case MovementBehavior::cardinal: //not entirely cardinal. randomly switches
        if((moveX==0 && moveY==0) || 2 * randf() < TICK_SPEED)
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
            //probably obsolete
            moveX = randf() - 0.5;
            moveY = randf() - 0.5;
        }
        break;
    case MovementBehavior::homing_erratic:
        if(shape->centerDist(p->shape.get()) < getVisionDistance(game_state))
        {
            moveRandomly(game_state, 50);
            moveX += 5 * sign(p->getX() - getX()) * TICK_SPEED;
            moveY += 5 * sign(p->getY() - getY()) * TICK_SPEED;
            break;
        }
        else moveRandomly(game_state, 100);
        break;
    case MovementBehavior::homing_simple:
        if(std::fabs(moveX)<EPSILON || std::fabs(moveY)<EPSILON || std::hypot(moveX, moveY) * 1.01 < getMovementSpeed(game_state))
            time_since_stuck = 0;
        if(time_since_stuck < 1) //get it unstuck
        {
            time_since_stuck += TICK_SPEED;
            moveRandomly(game_state, 100);
        }
        else if(shape->centerDist(p->shape.get()) < getVisionDistance(game_state))
        {
            moveX = p->getX() - getX();
            moveY = p->getY() - getY();
            break;
        }
        else moveRandomly(game_state, 100);
        break;
    case MovementBehavior::random:
        moveRandomly(game_state, 100);
        break;
    case MovementBehavior::follow_angle:
        //check if it's stuck
        if(age > TICK_SPEED && (std::fabs(moveX)<EPSILON || std::fabs(moveY)<EPSILON || std::hypot(moveX, moveY) * 1.01 <
                         getMovementSpeed(game_state)))
        {
            time_since_stuck = 0;
        }
        //if it's stuck, then it uses simple_homing behavior to get unstuck
        if(time_since_stuck < 0.5)
        {
            time_since_stuck += TICK_SPEED;
            moveX = p->getX() - getX();
            moveY = p->getY() - getY();
        }
        else
        {
            moveX = std::cos(getAngle());
            moveY = std::sin(getAngle());
        }
        break;
    default:
        print_warning("unknown unit movement behavior " + to_str((int)BASE_MOVEMENT_BEHAVIOR[type]));
    }
    //dX, dY, and dAngle represent where the unit wants to move
    //moveX, moveY, and moveAngle represent the unit's actual movement last frame (until they're updated this frame)
    if((game_state.game_map.gravity==0 || !isPlayer()) && BASE_MOVEMENT_BEHAVIOR[type] != MovementBehavior::pure_custom)
        normToHypot(moveX, moveY, getMovementSpeed(game_state));
    if(BASE_MOVEMENT_FUNC[type] != nullptr) //custom movement modifiers
        BASE_MOVEMENT_FUNC[type](game_state, shared_from_this());
    double dX = moveX;
    double dY = moveY;
    moveToNewPosition(game_state, moveX * TICK_SPEED, moveY * TICK_SPEED);
    //make bouncy units more bouncy
    if(BASE_MOVEMENT_BEHAVIOR[type] == MovementBehavior::bouncy &&
       (std::fabs(moveX) + EPSILON < std::fabs(dX) || std::fabs(moveY) + EPSILON < std::fabs(dY)))
    {
        moveX = randf() - 0.5;
        moveY = randf() - 0.5;
    }
    double dAngle = 0;
    switch(BASE_ROTATION_BEHAVIOR[type])
    {
    case RotationBehavior::player:
    case RotationBehavior::none:
        dAngle = 0;
        break;
    case RotationBehavior::unidirectional:
        dAngle = BASE_ROTATION_FUNC[type](game_state, shared_from_this());
        rotateToNewAngle(game_state, dAngle * TICK_SPEED);
        break;
    case RotationBehavior::bidirectional:
        dAngle = BASE_ROTATION_FUNC[type](game_state, shared_from_this());
        if(moveAngle == 0)
        {
            if(randz(0, 1) == 0)
                dAngle = -dAngle;
            rotateToNewAngle(game_state, dAngle * TICK_SPEED);
            if(moveAngle == 0)
                rotateToNewAngle(game_state, -dAngle * TICK_SPEED);
        }
        else rotateToNewAngle(game_state, dAngle * TICK_SPEED);
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
    if(isPlayer() && isMouseButtonPressed(SDL_BUTTON_RIGHT)) //only the player can cast spells
    {
        int weaponType = weapons[curWeapon].type;
        if(spellSecTillCastable<=0 && mana >= Weapon::BASE_SPELL_COST[weaponType] && weapons[curWeapon].ammo > 0)
        {
            if(weapons[curWeapon].cast_spell(game_state, shared_from_this())) //only reset cd and use mana if it's successful
            {
                mana -= Weapon::BASE_SPELL_COST[weaponType];
                spellSecCooldown = spellSecTillCastable = Weapon::BASE_SPELL_COOLDOWN[weaponType] * cdMult;
            }
        }
    }
    if(BASE_OTHER_BEHAVIOR[type] != nullptr)
    {
        //NOTE: we pass moveVar instead of dVar here sssbecause we care about the unit's actual movement, not its intended movement
        BASE_OTHER_BEHAVIOR[type](game_state, shared_from_this(), moveX, moveY, moveAngle);
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
    if(b->HP != INF)
        b->HP -= damage;
    if(BASE_TAKE_DAMAGE_FUNC[b->type] != nullptr)
        BASE_TAKE_DAMAGE_FUNC[b->type](a, b, damage);
    if(a != nullptr) //damage applied by the map (aka hurt tiles or after the floor timer ends) shouldn't cause a color change
        b->time_since_took_damage = 0;
    for(auto &i: b->items)
    {
        typedef TemporaryMod Tmod;
        switch((Item::Name)i.type)
        {
        case Item::Name::GreenBoots:
            b->addTemporaryMod(Tmod::Name::GreenBoots, Tmod::Mod::MovementSpeed, Tmod::Type::AdditiveDecaying, 0.65, 0.3);
            break;
        default:
            break;
        }
    }
    if(a != nullptr)
    {
        if(b->isDead())
        {
            a->addMoney(BASE_MONEY_REWARD[b->type]); //should only affect the player though
            a->HP += a->HPregenOnKill;
        }
        a->HP += damage * a->omnivamp;
        a->HP = std::min(a->HP, a->maxHP);
        for(auto &i: a->items)
        {
            typedef TemporaryMod Tmod;
            switch((Item::Name)i.type)
            {
            case Item::Name::RedBoots:
                a->addTemporaryMod(Tmod::Name::RedBoots, Tmod::Mod::MovementSpeed, Tmod::Type::AdditiveDecaying, 0.5, 0.15);
                break;
            default:
                break;
            }
        }
    }
}
void Unit::addMoney(double amount)
{
    money += amount;
}
bool Unit::isLegalPosition(const GenericMapRoom &room) const
{
    return !collidesWithOtherUnit_checkToValidateMap(room.units) && !collidesWithTerrain(room);
}
bool Unit::isActive(const GameState &game_state) const
{
    return is_active;
}
bool Unit::isDrawable(const GameState &game_state) const
{
    return isPlayer() || game_state.currentFloor%2==0 || game_state.game_map.found_rooms[belongs_to_room];
}
