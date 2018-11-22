#include <cmath>
#include "weapon.h"
#include "sdl_base.h"
#include "game_state.h"
#include "unit.h"
#include "basic_game_funcs.h"
int Weapon::BASE_AMMO[Weapon::NUM_WEAPONS];
double Weapon::BASE_FIRE_RATE[Weapon::NUM_WEAPONS], Weapon::BASE_SHOT_SPEED[Weapon::NUM_WEAPONS];
double Weapon::BASE_PROJ_LIFESPAN[Weapon::NUM_WEAPONS], Weapon::BASE_DAMAGE[Weapon::NUM_WEAPONS];
double Weapon::BASE_SPREAD[Weapon::NUM_WEAPONS];
SDL_Texture *Weapon::sprites[Weapon::NUM_WEAPONS], *Weapon::wield_sprites[Weapon::NUM_WEAPONS];
int Weapon::BASE_PROJ_TYPE[Weapon::NUM_WEAPONS];
int Weapon::BASE_AMMO_PER_PURCHASE[Weapon::NUM_WEAPONS], Weapon::BASE_AMMO_COST[Weapon::NUM_WEAPONS];
int Weapon::BASE_COST[Weapon::NUM_WEAPONS], Weapon::BASE_PROJ_PER_SHOT[Weapon::NUM_WEAPONS];
std::string Weapon::BASE_NAME[Weapon::NUM_WEAPONS], Weapon::BASE_DESC[Weapon::NUM_WEAPONS];
double Weapon::BASE_FIRERATE_IRREGULARITY[Weapon::NUM_WEAPONS];
bool Weapon::HAS_SPRITE[Weapon::NUM_WEAPONS];
Weapon::Weapon()
{
    this->type = NOT_SET;
}
Weapon::Weapon(int type)
{
    this->type = type;
    resetToBaseStats();
    this->ammo = BASE_AMMO[type];
    this->fireCounter = 0;
}
void Weapon::resetToBaseStats()
{
    this->fireRate = BASE_FIRE_RATE[type];
    this->damage = BASE_DAMAGE[type];
    this->spread = BASE_SPREAD[type];
}
void Weapon::applyMod(const Item &item)
{
    double xAccuracy = 1, dFireRate = 1;
    switch(item.name)
    {
    case Item::Name::Bullseye:
        xAccuracy += 0.4;
        break;
    case Item::Name::RedGloves:
        dFireRate += 0.1;
        break;
    default:
        break;
    }
    spread /= xAccuracy;
    fireRate /= dFireRate;
}
void Weapon::applyMods(const std::vector<Item> &items)
{
    resetToBaseStats();
    for(auto &i: items)
        applyMod(i);
}
void Weapon::draw(const GameState &game_state, int x, int y, double angle) const
{
    if(HAS_SPRITE[type])
    {
        int ppt = game_state.getPixelsPerTile();
        renderCopyEx(sprites[type], x-ppt/2, y-ppt/2, ppt, ppt, TO_DEG(angle));
    }
}
void Weapon::draw_wield(const GameState &game_state, int x, int y, double angle) const
{
    int ppt = game_state.getPixelsPerTile();
    renderCopyEx(wield_sprites[type], x-ppt, y-ppt, ppt*2, ppt*2, TO_DEG(angle));
}
void Weapon::update(GameState &game_state, std::shared_ptr<Unit> unit, bool fire)
{
    fireCounter -= TICK_SPEED;
    if(fire && fireCounter<=0 && ammo>0)
    {
        if(ammo != INF)
            ammo--;
        fireCounter = fireRate * (1 + randf() * BASE_FIRERATE_IRREGULARITY[type]);
        for(int iter=0; iter<BASE_PROJ_PER_SHOT[type]; iter++)
        {
            Projectile::create(game_state, unit, BASE_PROJ_TYPE[type], damage, BASE_SHOT_SPEED[type],
                               unit->getAngle() + spread * (1 - 2*randf()), BASE_PROJ_LIFESPAN[type]);
        }
    }
}
void Weapon::resetFireCounter()
{
    //discourage the player from frequently swapping weapons by setting the fireCounter to the max possible value (no use of randf() here)
    fireCounter = fireRate * (1 + BASE_FIRERATE_IRREGULARITY[type]);
}
//projectiles
SDL_Texture *Projectile::sprites[NUM_PROJECTILES];
std::string Projectile::BASE_SHAPE[NUM_PROJECTILES];
double Projectile::BASE_SIZE1[NUM_PROJECTILES];
Projectile::Projectile(Affiliation affiliation, int type, std::shared_ptr<Unit> owner, double damage, double x, double y, double dX, double dY, double timeTillExpiration, double angle)
{
    this->affiliation = affiliation;
    this->type = type;
    this->owner = owner;
    this->dX = dX;
    this->dY = dY;
    this->timeTillExpiration = timeTillExpiration;
    this->angle = angle;
    this->damage = damage;
    if(BASE_SHAPE[type] == "circle")
        shape = std::make_unique<Circle>(x, y, BASE_SIZE1[type]);
    else if(BASE_SHAPE[type] == "square")
        shape = std::make_unique<Square>(x, y, BASE_SIZE1[type]);
}
void Projectile::create(GameState &game_state, std::shared_ptr<Unit> unit, int type, double dmg, double shotSpeed, double projAngle, double lifespan)
{
    double dX = shotSpeed * std::cos(projAngle);
    double dY = shotSpeed * std::sin(projAngle);
    double offset = Unit::BASE_PROJECTILE_OFFSET[unit->type];
    double norm = std::hypot(dX, dY);
    double initX = unit->getX() + dX * offset / norm;
    double initY = unit->getY() + dY * offset / norm;
    game_state.game_map.projectiles.emplace_back(unit->affiliation, type, unit, dmg, initX, initY, dX + unit->moveX*0.5,
                                        dY + unit->moveY*0.5, lifespan, projAngle);
}
bool Projectile::collide(GameState &game_state)
{
    //check if it has hit terrain or has expired
    if(shape->collidesWithTerrain(game_state.game_map)) //check this first to prevent diagonally firing through terrain
        return true;
    if(timeTillExpiration < 0)
        return true;
    //now check if it's hit someone
    //slow version
    /*for(auto &i: game_state.game_map.units)
    {
        if(areEnemies(affiliation, i->affiliation) && !i->isDead() && shape->collidesWith(i->shape.get()))
        {
            auto u = game_state.game_map.getUnitByID(ownerID);
            if(u != nullptr)
            {
                if(u->hasItem(4)) //Sadist's Boots
                    u->addTemporaryMod(1);
            }
            i->receiveDamage(damage);
            return true;
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
            auto j = *miny;
            if(areEnemies(affiliation, j->affiliation) && !j->isDead() && shape->collidesWith(j->shape.get()))
            {
                Unit::dealDamage(owner.get(), j.get(), damage);
                return true;
            }
            miny++;
        }
    }
    return false;
}
void Projectile::move(GameState &game_state)
{
    setX(getX() + dX * TICK_SPEED);
    setY(getY() + dY * TICK_SPEED);
    timeTillExpiration -= TICK_SPEED;
}
void Projectile::draw(const GameState &game_state) const
{
    int ppt = game_state.getPixelsPerTile();
    int rx = (getX() - game_state.game_map.getCameraX())*ppt - ppt/2;
    int ry = (getY() - game_state.game_map.getCameraY())*ppt - ppt/2;
    renderCopyEx(sprites[type], rx, ry, ppt, ppt, TO_DEG(angle));
}
double Projectile::getX() const
{
    return shape->getX();
}
double Projectile::getY() const
{
    return shape->getY();
}
void Projectile::setX(double x)
{
    shape->setX(x);
}
void Projectile::setY(double y)
{
    shape->setY(y);
}
