#include <cmath>
#include "weapon.h"
#include "sdl_base.h"
#include "game_state.h"
#include "unit.h"
#include "basic_game_funcs.h"
audio_chunk Weapon::FIRE_AUDIO[Weapon::NUM_WEAPONS];
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
double Weapon::BASE_SPELL_COST[Weapon::NUM_WEAPONS], Weapon::BASE_SPELL_COOLDOWN[Weapon::NUM_WEAPONS];
double Weapon::BASE_MAXHP_PCT_DAMAGE[Weapon::NUM_WEAPONS];
void (*Weapon::BASE_PROJ_MOVEMENT_FUNC[Weapon::NUM_WEAPONS])(GameState&, const std::shared_ptr<Unit>&, Projectile&);
int Weapon::BASE_PIERCE_COUNT[Weapon::NUM_WEAPONS];
bool Weapon::HAS_SPRITE[Weapon::NUM_WEAPONS], Weapon::BASE_IS_SPECTRAL[Weapon::NUM_WEAPONS];
void (*Weapon::BASE_FIRE_FUNC[Weapon::NUM_WEAPONS])(GameState&, const std::shared_ptr<Unit>&, const Weapon&);
Weapon::Weapon()
{
    this->type = NOT_SET;
    resetToBaseStats();
}
Weapon::Weapon(int type)
{
    this->type = type;
    resetToBaseStats();
    this->ammo = BASE_AMMO[type];
    this->fireCounter = 0;
}
Weapon::Weapon(Name name)
{
    this->type = (int)name;
    resetToBaseStats();
    this->ammo = BASE_AMMO[type];
    this->fireCounter = 0;
}
void Weapon::resetToBaseStats()
{
    this->fireRate = BASE_FIRE_RATE[type];
    this->damage = BASE_DAMAGE[type];
    this->spread = BASE_SPREAD[type];
    this->shotSpeed = BASE_SHOT_SPEED[type];
    this->projLifespan = BASE_PROJ_LIFESPAN[type];
    this->pierceCount = BASE_PIERCE_COUNT[type];
}
double Weapon::getProjPierceCount(const Unit *unit) const
{
    return TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::PierceCount, pierceCount);
}
double Weapon::getFireRate(const Unit *unit) const
{
    double fr, xFR;
    TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::FireRate, fireRate, &fr, &xFR);
    return fr / xFR;
}
double Weapon::getProjLifespan(const Unit *unit) const
{
    if(unit->hasItem(Item::Name::WizardHat))
        return INF;
    return TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::ProjLife, projLifespan);
}
double Weapon::getProjSpread(const Unit *unit) const
{
    return std::min(PI, TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::ProjSpread, spread));
}
double Weapon::getProjSpeed(const Unit *unit) const
{
    double SS, xSS;
    TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::ProjSpeed, shotSpeed, &SS, &xSS);
    for(auto &i: unit->items)
    {
        switch((Item::Name)i.type)
        {
        case Item::Name::YellowPill:
            xSS += 0.6 * randf();
            break;
        case Item::Name::PhysicsMechanics:
            SS += 10;
            break;
        default:
            break;
        }
    }
    return SS * xSS;
}
double Weapon::getProjDamage(const Unit *unit) const
{
    return TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::ProjDamage, damage);
}
void Weapon::updateTemporaryMods()
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
void Weapon::addTemporaryMod(TemporaryMod::Name name, TemporaryMod::Mod mod, TemporaryMod::Type type, double duration, double var1)
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
double Weapon::getProjMaxHpPctDamage(const Unit *unit) const
{
    double pct = 0;
    if(!isLaser() && unit->hasItem(Item::Name::TheCommunistManifesto))
        pct += 0.01;
    return pct;
}
void Weapon::applyMods(const std::vector<Item> &items)
{
    resetToBaseStats();
    double xAccuracy = 1, xFireRate = 1, xShotSpeed = 1, xDmg = 1, xProjlifespan = 1, xSpread = 1;
    for(auto &i: items)
    {
        switch((Item::Name)i.type)
        {
        case Item::Name::Bullseye:
            xAccuracy += 0.3;
            break;
        case Item::Name::RedGloves:
            xFireRate += 0.07;
            break;
        case Item::Name::PistolEnhancementI:
            if((Name)type == Name::PoofyPistol)
                xAccuracy += 0.6;
            break;
        case Item::Name::PistolEnhancementII:
            if((Name)type == Name::PoofyPistol)
                xShotSpeed += 0.6;
            break;
        case Item::Name::PistolEnhancementIII:
            if((Name)type == Name::PoofyPistol)
                xDmg += 0.5;
            break;
        case Item::Name::PurpleBoots:
            xShotSpeed += 0.1;
            break;
        case Item::Name::Projektyles:
            if(!isLaser())
                damage += 1.5;
            break;
        case Item::Name::WaifuBodyPillow:
            xDmg += 0.1;
            xShotSpeed += 0.1;
            break;
        case Item::Name::Barber_1:
            xFireRate -= 0.10;
            break;
        case Item::Name::Barber_2:
            xFireRate -= 0.05;
            break;
        case Item::Name::Barber_3:
            xFireRate += 0.15;
            break;
        case Item::Name::NeighborsGardenHose:
            if(isWaterUser())
                ammo = INF;
            break;
        case Item::Name::AngryGamersManifesto_1:
            xDmg += 0.12;
            break;
        case Item::Name::PhysicsForBots:
            xDmg += 0.12;
            break;
        case Item::Name::ArcheryLesson_4:
            if(isBow())
            {
                xFireRate += 0.5;
                xProjlifespan += 0.5;
                xSpread += 0.5;
                xDmg += 1;
            }
        case Item::Name::LaissezFaireManifesto:
            if(!isLaser())
                damage += 4.0;
            break;
        case Item::Name::IndecisivePersonsManifesto:
            pierceCount++;
            break;
        default:
            break;
        }
    }
    spread /= xAccuracy;
    fireRate /= xFireRate;
    projLifespan *= xProjlifespan;
    shotSpeed *= xShotSpeed;
    damage *= xDmg;
    spread /= xSpread;
}
void Weapon::draw(const GameState &game_state, int x, int y, double angle) const
{
    if(HAS_SPRITE[type])
    {
        int ppt = getPixelsPerTile();
        renderCopyEx(sprites[type], x-ppt/2, y-ppt/2, ppt, ppt, TO_DEG(angle));
    }
}
void Weapon::draw_wield(const GameState &game_state, int x, int y, double angle) const
{
    int ppt = getPixelsPerTile();
    renderCopyEx(wield_sprites[type], x-ppt, y-ppt, ppt*2, ppt*2, TO_DEG(angle));
}
void Weapon::fireWeapon(GameState &game_state, std::shared_ptr<Unit> unit)
{
    if(ammo != INF)
        ammo--;
    FIRE_AUDIO[type].play();
    for(int iter=0; iter<TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::ProjPerShot, BASE_PROJ_PER_SHOT[type]); iter++)
    {
        //create_normal is like create but the initial starting position for projectiles with heavy spread is more accurate
        //create_normal is good for non-rotating units (e.g. the player)
        bool relVelocity = unit->hasItem(Item::Name::AnIntroductionToRelativity);
        Projectile::create_normal(game_state, unit, BASE_PROJ_TYPE[type], getProjDamage(unit.get()), getProjSpeed(unit.get()),
            unit->getAngle() + getProjSpread(unit.get()) * (1 - 2*randf()), getProjLifespan(unit.get()), unit->getAngle(), relVelocity);
        auto &proj = game_state.game_map.projectiles.back();
        proj.maxHPPctdamage = getProjMaxHpPctDamage(unit.get());
        proj.collisionsLeft = getProjPierceCount(unit.get());
        proj.is_spectral = BASE_IS_SPECTRAL[type];
        proj.MOVEMENT_FUNC = BASE_PROJ_MOVEMENT_FUNC[type];
    }
}
void Weapon::update(GameState &game_state, std::shared_ptr<Unit> unit, bool fire) //fire acts as if the LMB is always pressed down
{
    fireCounter -= TICK_SPEED;
    bool alwaysFire = false;
    for(auto &i: tempMods)
        if(i.mod == TemporaryMod::Mod::AlwaysFire)
            alwaysFire = true;
    fire |= alwaysFire;
    while(fire && fireCounter<=0 && ammo>0)
    {
        fireCounter = getFireRate(unit.get()) * (1 + randf() * BASE_FIRERATE_IRREGULARITY[type]);
        if(BASE_FIRE_FUNC[type] != nullptr)
            BASE_FIRE_FUNC[type](game_state, unit, *this);
        else fireWeapon(game_state, unit);
    }
}
void Weapon::resetFireCounter(const Unit *unit)
{
    //discourage the player from frequently swapping weapons by setting the fireCounter to the max possible value (no use of randf() here)
    //if the fire rate is temporarily still boosted then it might not reset to how we like it (but there's no perfect way anyway so whatever)
    fireCounter = getFireRate(unit) * (1 + BASE_FIRERATE_IRREGULARITY[type]);
}
bool Weapon::cast_spell(GameState &game_state, std::shared_ptr<Unit> unit)
{
    typedef TemporaryMod Tmod;
    switch((Weapon::Name)type)
    {
    case Name::PoofyPistol:
        fireCounter = 0;
        addTemporaryMod(Tmod::Name::PoofyPistolSpecial_1, Tmod::Mod::FireRate, Tmod::Type::Multiplicative, 0.1, 0.05);
        addTemporaryMod(Tmod::Name::PoofyPistolSpecial_2, Tmod::Mod::AlwaysFire, Tmod::Type::None, 0.1, NOT_SET);
        return true;
    case Name::CadmiumCrossbow:
        fireCounter = 0;
        addTemporaryMod(Tmod::Name::CadmiumCrossbowSpecial_1, Tmod::Mod::PierceCount, Tmod::Type::Additive, 0.1, INF);
        addTemporaryMod(Tmod::Name::CadmiumCrossbowSpecial_2, Tmod::Mod::AlwaysFire, Tmod::Type::None, 0.1, NOT_SET);
        return true;
    case Name::UnbrandedSniperRifle:
        fireCounter = 0;
        addTemporaryMod(Tmod::Name::UnbrandedSniperRifle_1, Tmod::Mod::ProjSpread, Tmod::Type::Multiplicative, 0.05, 0);
        addTemporaryMod(Tmod::Name::UnbrandedSniperRifle_2, Tmod::Mod::ProjLife, Tmod::Type::Additive, 0.05, INF);
        addTemporaryMod(Tmod::Name::UnbrandedSniperRifle_3, Tmod::Mod::ProjSpeed, Tmod::Type::Multiplicative, 0.05, 1.5);
        addTemporaryMod(Tmod::Name::UnbrandedSniperRifle_4, Tmod::Mod::ProjDamage, Tmod::Type::Multiplicative, 0.05, 1.5);
        addTemporaryMod(Tmod::Name::UnbrandedSniperRifle_5, Tmod::Mod::AlwaysFire, Tmod::Type::None, 0.05, NOT_SET);
        return true;
    case Name::HuntingRifle:
        fireCounter = 0;
        addTemporaryMod(Tmod::Name::HuntingRifle_1, Tmod::Mod::ProjSpread, Tmod::Type::Multiplicative, 0.05, 0);
        addTemporaryMod(Tmod::Name::HuntingRifle_2, Tmod::Mod::ProjLife, Tmod::Type::Additive, 0.05, INF);
        addTemporaryMod(Tmod::Name::HuntingRifle_3, Tmod::Mod::ProjSpeed, Tmod::Type::Multiplicative, 0.05, 1.8);
        addTemporaryMod(Tmod::Name::HuntingRifle_4, Tmod::Mod::ProjDamage, Tmod::Type::Multiplicative, 0.05, 1.8);
        addTemporaryMod(Tmod::Name::HuntingRifle_5, Tmod::Mod::AlwaysFire, Tmod::Type::None, 0.05, NOT_SET);
        break;
    case Name::SmolStick:
        return true;
    case Name::WaterGun:
        fireCounter = 0;
        addTemporaryMod(Tmod::Name::WaterGunSpecial_1, Tmod::Mod::FireRate, Tmod::Type::Multiplicative, 0.4, 0.15);
        addTemporaryMod(Tmod::Name::WaterGunSpecial_2, Tmod::Mod::AlwaysFire, Tmod::Type::None, 0.4, NOT_SET);
        addTemporaryMod(Tmod::Name::WaterGunSpecial_3, Tmod::Mod::ProjLife, Tmod::Type::Multiplicative, 0.4, 2);
        return true;
    case Name::BigStick:
        addTemporaryMod(Tmod::Name::BigStick_1, Tmod::Mod::ProjDamage, Tmod::Type::Multiplicative, 1, 3.5);
        unit->addTemporaryMod(Tmod::Name::BigStick_2, Tmod::Mod::MovementSpeed, Tmod::Type::MultiplicativeDecaying, 1, 1.7);
        addTemporaryMod(Tmod::Name::BigStick_3, Tmod::Mod::AlwaysFire, Tmod::Type::None, 1, NOT_SET);
        return true;
    case Name::BiggerStick:
        unit->addTemporaryMod(Tmod::Name::BiggerStick, Tmod::Mod::HurtTileDamage, Tmod::Type::Multiplicative, 2, 0);
        return true;
    case Name::BiggestStick:
        for(auto &i: game_state.game_map.units)
            if(areEnemies(unit->affiliation, i->affiliation))
                i->HP -= i->maxHP * 0.1;
        return true;
    case Name::Pi:
        fireCounter = 0;
        addTemporaryMod(Tmod::Name::Pi_1, Tmod::Mod::FireRate, Tmod::Type::Multiplicative, 1, 0.5);
        addTemporaryMod(Tmod::Name::Pi_2, Tmod::Mod::AlwaysFire, Tmod::Type::None, 1, NOT_SET);
        return true;
    case Name::SSS:
        fireCounter = 0;
        addTemporaryMod(Tmod::Name::SSS_1, Tmod::Mod::FireRate, Tmod::Type::Multiplicative, 0.5, 0.1);
        addTemporaryMod(Tmod::Name::SSS_3, Tmod::Mod::AlwaysFire, Tmod::Type::None, 0.5, NOT_SET);
        return true;
    case Name::Railgun:
        fireCounter = 0;
        addTemporaryMod(Tmod::Name::Railgun_1, Tmod::Mod::ProjSpeed, Tmod::Type::Multiplicative, 0.1, 2);
        addTemporaryMod(Tmod::Name::Railgun_2, Tmod::Mod::ProjSpread, Tmod::Type::Multiplicative, 0.1, 0);
        addTemporaryMod(Tmod::Name::Railgun_3, Tmod::Mod::FireRate, Tmod::Type::Multiplicative, 0.1, 0.01);
        addTemporaryMod(Tmod::Name::Railgun_4, Tmod::Mod::AlwaysFire, Tmod::Type::None, 0.1, NOT_SET);
        return true;
    default:
        print_warning("tried to cast spell with weapon " + to_str((int)type) + ", which doesn't have a spell");
        return true;
    }
    return true; //just to avoid the warning
}
bool Weapon::isLaser() const
{
    return type == (int)Name::Flashlight || type == (int)Name::Laser || type == (int)Name::QuasarGun || type == (int)Name::BrokenLaser;
}
bool Weapon::isWaterUser() const
{
    return type == (int)Name::WaterGun || type == (int)Name::AWBP || type == (int)Name::SprinklerHead;
}
bool Weapon::isSniper() const
{
    return type == (int)Name::UnbrandedSniperRifle || type == (int)Name::HuntingRifle || type == (int)Name::SSS || type == (int)Name::Blowgun;
}
bool Weapon::isBow() const
{
    return type == (int)Name::BromineBow || type == (int)Name::CadmiumCrossbow;
}
//projectiles
SDL_Texture* (*Projectile::GEN_CUSTOM_TEXTURE[NUM_PROJECTILES])();
SDL_Texture *Projectile::sprites[NUM_PROJECTILES];
std::string Projectile::BASE_SHAPE[NUM_PROJECTILES];
double Projectile::BASE_SIZE1[NUM_PROJECTILES];
static_array<Point> Projectile::BASE_VERTICES[Projectile::NUM_PROJECTILES];
Projectile::Projectile(Affiliation affiliation, int type, const std::shared_ptr<Unit> &owner, double damage, double x, double y,
                       double dX, double dY, double timeTillExpiration, double angle,
                       void (*moveFunc)(GameState&, const std::shared_ptr<Unit>&, Projectile&))
{
    this->affiliation = affiliation;
    this->type = type;
    if(GEN_CUSTOM_TEXTURE[type] == nullptr)
        this->sprite = sprites[type];
    else this->sprite = GEN_CUSTOM_TEXTURE[type]();
    this->owner = owner;
    this->dX = dX;
    this->dY = dY;
    this->timeTillExpiration = timeTillExpiration;
    this->damage = damage;
    this->MOVEMENT_FUNC = moveFunc;
    this->age = 0;
    this->maxHPPctdamage = 0;
    this->rand_var = randf();
    this->collisionsLeft = 1;
    this->is_spectral = false;
    for(auto &i: misc_var)
        i = 0;
    if(BASE_SHAPE[type] == "circle")
        shape = std::make_unique<Circle>(x, y, BASE_SIZE1[type]);
    else if(BASE_SHAPE[type] == "square")
        shape = std::make_unique<Square>(x, y, BASE_SIZE1[type]);
    else if(BASE_SHAPE[type] == "polygon")
        shape = std::make_unique<Polygon>(x, y, BASE_VERTICES[type]);
    else print_warning("unsupported projectile type: " + to_str(type));
    this->shape->setAngle(angle);
}
void Projectile::create_normal(GameState &game_state, const std::shared_ptr<Unit> &unit, int type, double dmg, double shotSpeed,
                        double projAngle, double lifespan, double origAngle, bool relVelocity) //better start location for the player
{
    double dX = shotSpeed * std::cos(projAngle);
    double dY = shotSpeed * std::sin(projAngle);
    double offset = Unit::BASE_PROJECTILE_OFFSET[unit->type];
    double initX = unit->getX() + std::cos(origAngle) * offset;
    double initY = unit->getY() + std::sin(origAngle) * offset;
    if(relVelocity)
        game_state.game_map.projectiles.push_back({unit->affiliation, type, unit, dmg, initX, initY, dX + unit->moveX*0.5,
                                        dY + unit->moveY*0.5, lifespan, projAngle});
    else game_state.game_map.projectiles.push_back({unit->affiliation, type, unit, dmg, initX, initY, dX,
                                        dY, lifespan, projAngle});
}
void Projectile::create(GameState &game_state, const std::shared_ptr<Unit> &unit, int type, double dmg, double shotSpeed,
                        double projAngle, double lifespan, double offset /*=NOT_SET*/,
                        void (*moveFunc)(GameState&, const std::shared_ptr<Unit>&, Projectile&))
{
    double dX = shotSpeed * std::cos(projAngle);
    double dY = shotSpeed * std::sin(projAngle);
    if(offset == NOT_SET)
        offset = Unit::BASE_PROJECTILE_OFFSET[unit->type];
    double initX = unit->getX() + std::cos(projAngle) * offset;
    double initY = unit->getY() + std::sin(projAngle) * offset;
    game_state.game_map.projectiles.push_back({unit->affiliation, type, unit, dmg, initX, initY, dX + unit->moveX*0.5,
                                        dY + unit->moveY*0.5, lifespan, projAngle, moveFunc});
}
void Projectile::create_no_relative_velocity(GameState &game_state, const std::shared_ptr<Unit> &unit, int type, double dmg, double shotSpeed,
                        double projAngle, double lifespan, double offset /*=NOT_SET*/,
                        void (*moveFunc)(GameState&, const std::shared_ptr<Unit>&, Projectile&))
{
    double dX = shotSpeed * std::cos(projAngle);
    double dY = shotSpeed * std::sin(projAngle);
    if(offset == NOT_SET)
        offset = Unit::BASE_PROJECTILE_OFFSET[unit->type];
    double initX = unit->getX() + std::cos(projAngle) * offset;
    double initY = unit->getY() + std::sin(projAngle) * offset;
    game_state.game_map.projectiles.push_back({unit->affiliation, type, unit, dmg, initX, initY, dX,
                                        dY, lifespan, projAngle, moveFunc});
}
bool Projectile::collide(GameState &game_state)
{
    //check if it has hit terrain or has expired
    timeTillExpiration -= TICK_SPEED;
    if(timeTillExpiration < 0)
        return true;
    if(!isSpectral())
    {
        if(shape->collidesWithUnflyableTile(game_state.game_map))
            return true;
    }
    else if(game_state.game_map.isOutOfMap(shape.get()))
        return true;
    age += TICK_SPEED;
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
            if(areEnemies(affiliation, j->affiliation) && !j->isDead() && !alreadyCollidedWith.count(j->ID) && shape->collidesWith(j->shape.get()))
            {
                Unit::dealDamage(owner.get(), j.get(), damage + j->maxHP * maxHPPctdamage);
                collisionsLeft--;
                if(collisionsLeft <= 0) //note: if collisionsLeft is originally <=0 then it effectively is 1
                    return true;
                else
                {
                    alreadyCollidedWith.insert(j->ID);
                }
            }
            miny++;
        }
    }
    return false;
}
void Projectile::move(GameState &game_state)
{
    if(MOVEMENT_FUNC == nullptr)
    {
        setX(getX() + dX * TICK_SPEED);
        setY(getY() + dY * TICK_SPEED);
    }
    else MOVEMENT_FUNC(game_state, owner, *this);
}
void Projectile::draw(const GameState &game_state) const
{
    int ppt = getPixelsPerTile();
    int rw, rh;
    SDL_QueryTexture(sprite, NULL, NULL, &rw, &rh);
    int rx = (getX() - game_state.game_map.getCameraX())*ppt - rw*SPRITE_SCALE/2;
    int ry = (getY() - game_state.game_map.getCameraY())*ppt - rh*SPRITE_SCALE/2;
    uint8_t alpha = 255 * std::min(1.0, square(4 * timeTillExpiration));
    SDL_SetTextureAlphaMod(sprite, alpha);
    renderCopyEx(sprite, rx, ry, rw * SPRITE_SCALE, rh * SPRITE_SCALE, TO_DEG(shape->getAngle()));
}
double Projectile::getX() const
{
    return shape->getX();
}
double Projectile::getY() const
{
    return shape->getY();
}
double Projectile::getAngle() const
{
    return shape->getAngle();
}
void Projectile::setX(double x)
{
    shape->setX(x);
}
void Projectile::setY(double y)
{
    shape->setY(y);
}
void Projectile::setAngle(double angle)
{
    shape->setAngle(angle);
}
bool Projectile::isSpectral()
{
    if(is_spectral)
        return true;
    if(owner->hasItem(Item::Name::XrayGlasses))
        return true;
    return false;
}
