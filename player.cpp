#include "player.h"
#include "sdl_base.h"
#include "game_map.h"
#include "game_state.h"
#include "fundamentals.h"
#include <algorithm>
#include <cmath>
#define SDL_SCANCODE_W SDL_SCANCODE_T
#define SDL_SCANCODE_S SDL_SCANCODE_G
#define SDL_SCANCODE_A SDL_SCANCODE_F
#define SDL_SCANCODE_D SDL_SCANCODE_H
using namespace std;
Player::Player(){}
Player::Player(double startX, double startY): Unit(0, Affiliation::player, startX, startY)
{
    moveUp = SDL_SCANCODE_W;
    moveDown = SDL_SCANCODE_S;
    moveLeft = SDL_SCANCODE_A;
    moveRight = SDL_SCANCODE_D;
    pressTimer.resize(4, 0);
    money = 10000;
    weapons.emplace_back(0);
    weapons.emplace_back(1);
    curWeapon = 0;
    spellSecTillCastable = 0;
    spellSecCooldown = NOT_SET;
}
void Player::updateKeyPressTimer(const uint8_t *keystate, int &timer, SDL_Scancode key)
{
    if(keystate[key])
        timer++;
    else timer = 0;
}
static void ntb(std::string s) //short for New Text Box
{
    aestheticEffects.push_back(std::make_shared<TextBox>(s, INF, 0.04, 0.6, 0.75, 0.35));
}
void Player::applyFirstTimeItemEffects(GameState &game_state, Item::Name name)
{
    switch(name)
    {
    case Item::Name::SpikedVest:
        ntb("The spikes eviscerate enemies and cause you to deal 200% contact damage.");
        break;
    case Item::Name::Bullseye:
        ntb("Getting a bullseye tattoo somehow helps you fire more accurately.");
        break;
    case Item::Name::DirtySerratedShirt:
        ntb("You find out that the shirt gives enemies tetanus on contact, causing them to move more slowly for a bit when you collide with them.");
        break;
    case Item::Name::BlueBoots:
        ntb("Blue makes you happy so you move a bit faster.");
        break;
    case Item::Name::RedBoots:
        ntb("Gain a blood rush and move faster when damaging others");
        break;
    case Item::Name::GreenBoots:
        ntb("Move faster when damaged.");
        break;
    case Item::Name::SadistsMachete:
        ntb("Deal a bit more damage and heal for a small portion of all damage dealt.");
        break;
    case Item::Name::AmuletOfAir:
        ntb("This item actually somehow works. It generates a cool breeze that makes you move faster. Unfortunately, nearby enemies obstruct the wind.");
        break;
    case Item::Name::DaltonsAtom:
        ntb("You feel very unenlightened but your chemistry professor now reveres you and uses magical powers to grant you +10% time per floor.");
        break;
    case Item::Name::Toothbrush:
        ntb("Gain more and more movement speed when there's less and less time left on the clock.");
        break;
    case Item::Name::RedGloves:
        ntb("Your weapons are happy that they are held by a nicely colored fuzzy material and fire faster.");
        break;
    case Item::Name::RedPillow:
        ntb("The fluff fills you with renewed vigor. +15 max HP.");
        HP += 15;
        break;
    case Item::Name::MysteriousVial:
        ntb("Minimal HP regeneration over time.");
        break;
    case Item::Name::MysteriousPhial:
        ntb("Deal more and receive more damage.");
        break;
    case Item::Name::TheArtOfSadism:
        ntb("Gain renewed vigor from killing.");
        break;
    case Item::Name::PistolEnhancementI:
        ntb("Well, whatever this thing does, it seems to make the Poofy Pistol fire a lot more accurately.");
        break;
//FLOOR 4
    case Item::Name::BlueTome:
        ntb("Small mana regen up.");
        break;
    case Item::Name::PistolEnhancementII:
        ntb("Your Poofy Pistol's shots now travel faster");
        break;
    case Item::Name::PhilosophersBook:
        ntb("You philosophize about the possibility that simply carrying this book may grant you 30 IQ points. You philosophize that as the smartest philosopher you know, you are probably right, so you gain 30 IQ points.");
        mana += 25;
        break;
    case Item::Name::PurpleBoots:
        ntb("+Movement speed and +Shot speed.");
        break;
    case Item::Name::TheSadistManifesto:
        ntb("It has detailed instructions that allow you to regain vigor from dealing damage to others. Gain health from hurting shapes.");
        break;
    case Item::Name::Projektyles:
        ntb("Reading Projektyles grants allows you to infuse your projectiles with extra power, granting them a flat damage bonus.");
        break;
    case Item::Name::IllegallyMadeFurCoat:
        ntb("You ignore the disclaimer. The coat makes you warm and grants +20 max HP and a small amount of HP regen.");
        HP += 20;
        break;
    case Item::Name::ProfessionalProtester:
        switch(randz(0, 1))
        {
        case 0:
            ntb("The presence of a protester causes business to tank, causing the prices "
                "of all items and services to go down by 20%. The weapons shop keeps operating as normal.");
            break;
        case 1:
            ntb("The professional protester stayed up too late watching anime and slept through "
                "his alarm the day he was supposed to protest. He managed to protest for the last five minutes of the shop's open hours that "
                "day to no effect. He is unwilling to try again to make up for his mess-up and disappears without a trace.");
            break;
        }
        break;
    case Item::Name::WaifuBodyPillow:
        ntb("Despite the low quality art, it still makes you comfortable in every way possible and gives you lots of minor stat boosts.");
        HP += 10;
        break;
//FLOOR 6
    case Item::Name::AngryGamersManifesto:
        switch(randz(0, 1))
        {
        case 0:
            ntb("After this is published, the dev gets scared and gives you free microtransactions. He also offers you the ability to respawn for $0.00. Wow, what a great deal!");
            break;
        case 1:
            ntb("The developer ignores you. Time to leave a negative review! Oh wait, you can't leave reviews for a closed beta version. You get angry and decide to vent your anger on shapes, dealing more damage to them in your fury. +20% base damage.");
            unshownItems.push_back(Item::Name::AngryGamersManifesto_1);
            break;
        }
        break;
    case Item::Name::PhysicsForBots:
        ntb("You learn that moving objects have \"enjury\", so they exert a \"smoosh-poosh\" force and therefore result in a \"jimpulse\", causing enemies to receive more damage. You don't completely understand what the book's trying to tell you, but after reading it you learn that projectiles are generally supposed to do more damage than they're currently doing.");
        break;
    case Item::Name::FloppyDisk:
        switch(randz(0, 2))
        {
        case 0:
            ntb("You find things on the disk that are censored here to preserve the PG-13ness of this game. You are happy, so you gain +15% MS.");
            unshownItems.push_back(Item::Name::FloppyDisk_1);
            break;
        case 1:
            ntb("The floppy disk is broken. What a surprise.");
            break;
        case 2:
            ntb("The floppy disk turns out to not actually be a floppy disk but is actually a rare Indo-European artifact. You sell it for $200.");
            addMoney(200);
            break;
        }
        break;
//FLOOR 8
    case Item::Name::ProfessionalRioter:
        switch(randz(0, 2))
        {
        case 0:
            ntb("The rioter is beaten up by angry shop owners and goes home without having accomplished anything.");
            break;
        case 1:
            ntb("The rioter is shot and killed by an annoyed weapon shop manager and the "
                "rioter's family successfully sues you for a quarter of your net worth. The weapon shop manager manages to not get sued "
                "by the rioter's family because he's the rioter's dad.");
            money *= 0.75;
            break;
        case 2:
            ntb("The rioter successfully scares off potential customers. "
                "The prices of all weapons drop by 25% due to decreased demand.");
            game_state.weapon_cost_mult -= 0.25;
            break;
        }
        break;
    case Item::Name::Attorney:
        if(randf() < 0.3)
        {
            ntb("Your attorney does a terrible job due to the lack of a commission. "
                "You are fined 15% of your net worth for wasting court time.");
            money *= 0.85;
        }
        else
        {
            int winnings = randz(1, 600);
            ntb("Your attorney wins the case and you get $" + to_str(winnings) + ".");
            addMoney(winnings);
        }
        break;
    case Item::Name::PistolEnhancementIII:
        ntb("BIG damage up on the Poofy Pistol. Well, big relative to its base damage.");
        break;
    case Item::Name::Barber:
        if(randf() < 0.1)
        {
            ntb("The barber gives you a mohawk and you get crippling depression. "
                "Due to this, you are unable to fire weapons as fast as you used to.");
            unshownItems.emplace_back(Item::Name::Barber_1);
        }
        else if(randf() < 0.5)
        {
            ntb("The barber shaves you bald and you get depression. Fire rate down.");
            unshownItems.emplace_back(Item::Name::Barber_2);
        }
        else
        {
            ntb("The barber decides to not cut your hair and glues an afro onto your head "
                "instead. You feel extra cool and this coolness enables you to fire weapons faster.");
            unshownItems.emplace_back(Item::Name::Barber_3);
        }
        break;
    case Item::Name::NeighborsGardenHose:
        ntb("Your neighbors notices you using his hose, but he doesn't care because he thinks water is free. Free water for life (more like until you move)!");
        break;
    case Item::Name::MahouShoujoTshirt:
        ntb("Simpler shapes move slower around you.");
        break;
    case Item::Name::Aspirin:
        HP += 50;
        ntb("The pill does what you'd expect. +max HP.");
        break;
    case Item::Name::YellowPill:
        ntb("The yellow pill makes you think you now fire shots that have a random speed bonus.");
        break;
    case Item::Name::ShowerPass:
        ntb("After developing a new habit of showering weekly, you find out that malicious shapes have a much harder time detecting you. And no, it isn’t disgusting to shower less frequently than once a day.");
        break;
    case Item::Name::ArcheryLesson:
        switch(randz(0, 4))
        {
        case 0:
            ntb("An angry person fires an arrow at you during the lesson. Most of the damage heals quickly, but you develop a small permanent injury which negates any archery skills you gained. You file a lawsuit but it is unsuccessful. -10 maxHP.");
            unshownItems.emplace_back(Item::Name::ArcheryLesson_1);
            break;
        case 1:
            {
            int m = randz(200, 600);
            addMoney(m);
            ntb("An angry person fires an arrow at you during the lesson. Most of the damage heals quickly, but you develop a small permanent injury, which negates any archery skills you gained. However, you successfully sue for $" + to_str(m) + ".  -10 maxHP.");
            unshownItems.emplace_back(Item::Name::ArcheryLesson_2);
            }
            break;
        case 2:
            ntb("You only learn about the history of bows during the lesson. The boredom lowers your IQ by 5 points.");
            unshownItems.emplace_back(Item::Name::ArcheryLesson_3);
            break;
        case 3:
        case 4:
            ntb("You successfully absorb knowledge of archery and become a god at using bows.");
            unshownItems.emplace_back(Item::Name::ArcheryLesson_4);
            break;
        }
        break;
    case Item::Name::LaissezFaireManifesto:
        ntb("+ a moderate amount of flat damage on projectiles");
        break;
    case Item::Name::Fentanyl:
        ntb("+max HP");
        HP += 80;
        break;
    case Item::Name::IndecisivePersonsManifesto:
        ntb("The author finally manages to decide on something at the end of the book: to teach you how to modify your gun so that projectiles rip through the first shape and are able to hit a second.");
        break;
//FLOOR 10
    case Item::Name::XrayGlasses:
        ntb("As soon as you get these glasses, you start realizing your projectiles actually travel through walls and you just didn't notice it before. Or maybe they didn’t before, and now they do. Or maybe they didn’t before and still don't, but merely appear to now. At any rate, perception is reality.");
        break;
    case Item::Name::WizardHat:
        ntb("You are now a wizard. Wizards don't know physics so their projectiles don’t die due to air resistance and gravity. Wizards owe their fighting prowess to their ability to turn sticks into wands.");
        break;
    case Item::Name::TheCommunistManifesto:
        ntb("Projectiles deal a small percentage of enemies' max health");
        break;
    case Item::Name::AnIntroductionToRelativity:
        ntb("Projectiles no longer gain velocity based on your current velocity. Seems more like classical mechanics than relativity.");
        break;
    case Item::Name::LaggyRouter:
        ntb("Your movement speed randomly hovers between a bit slower and a lot faster. It's 100% worth if you get used to it.");
        break;
    case Item::Name::PhysicsMechanics:
        ntb("You flip to the \"Easy Warmup\" section and find: \"There are three discs of masses M1, M2, and M3 and radii R1, R2, and R3, respectively, lying on a frictionless surface. The first one spins at x radians per second, while the others are at rest. None are initially in contact with the others. The second and third discs are then made to touch the first wheel, but the second and third discs still do not touch each other. Find the final angular velocity of the third disc.\" You skip the easy problems and decide to do the hard problems, learning a lot about kinematics. +10 shot speed on every projectile.");
        break;
    case Item::Name::CSGOSteamCode:
        ntb("After playing CS:GO, you realize that units are supposed to move slower when damaged. Of course, class Player inherits Unit. You also learn many new phrases, which boosts your IQ by 50 points.");
        mana += 50;
        break;
    default:
        break;
    }
}
/*void Player::updateMinimap(GameMap &game_map)
{
    int minimapW, minimapH;
    game_map.getMinimapSize(&minimapW, &minimapH);
    double eppt = game_map.getExploredPixelsPerTile();
    int xmin = max(0, (int)((getX() - visionDistance) * eppt));
    int xmax = min(minimapW-1, (int)((getX() + visionDistance) * eppt));
    int ymin = max(0, (int)((getY() - visionDistance) * eppt));
    int ymax = min(minimapH-1, (int)((getY() + visionDistance) * eppt));
    SDL_SetRenderTarget(getRenderer(), game_map.explored_texture);
    setColor(0, 0, 0);
    SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_NONE);
    for(int i=xmin; i<=xmax; i++)
    {
        for(int j=ymin; j<=ymax; j++)
        {
            if(!game_map.explored[i][j])
            {
                game_map.explored[i][j] = true;
                drawPoint(i, j, 0, 0, 0, 0); //drawPoint is slow so only do it when we have to
            }
        }
    }
    SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(getRenderer(), NULL);
}*/
/*void Player::operate(GameState &game_state)
{
    Unit::operate(game_state);
    if(curWeapon != NOT_SET)
        weapons[curWeapon].update(game_state, this, isMouseButtonPressed(SDL_BUTTON_LEFT)); //operate weapons
    //updateMinimap(game_state.game_map);
}*/

void Player::handleInput(GameState &game_state)
{
    const uint8_t *keystate = SDL_GetKeyboardState(NULL);
    updateKeyPressTimer(keystate, pressTimer[0], moveUp);
    updateKeyPressTimer(keystate, pressTimer[1], moveDown);
    updateKeyPressTimer(keystate, pressTimer[2], moveLeft);
    updateKeyPressTimer(keystate, pressTimer[3], moveRight);
    if(game_state.game_map.gravity == 0)
    {
        if((moveX==0 && moveY==0) || !shape->collidesWithTile(game_state.game_map, MapTile::slip)) //you can't control yourself if you're on a slip tile and currently moving
        {
            moveX = moveY = 0;
            if(pressTimer[0]>0 && (pressTimer[1]==0 || pressTimer[0]<pressTimer[1])) //MOVE UP
                moveY--;
            if(pressTimer[1]>0 && (pressTimer[0]==0 || pressTimer[1]<pressTimer[0])) //MOVE DOWN
                moveY++;
            if(pressTimer[2]>0 && (pressTimer[3]==0 || pressTimer[2]<pressTimer[3])) //MOVE LEFT
                moveX--;
            if(pressTimer[3]>0 && (pressTimer[2]==0 || pressTimer[3]<pressTimer[2])) //MOVE RIGHT
                moveX++;
        }
    }
    else
    {
        if(moveY == 0)
        {
            if(pressTimer[0]>0 && (pressTimer[1]==0 || pressTimer[0]<pressTimer[1])) //MOVE UP
                moveY -= 33;
        }
        moveX = 0;
        moveY += game_state.game_map.gravity / BASE_FPS;
        if(pressTimer[2]>0 && (pressTimer[3]==0 || pressTimer[2]<pressTimer[3])) //MOVE LEFT
            moveX -= getMovementSpeed(game_state);
        if(pressTimer[3]>0 && (pressTimer[2]==0 || pressTimer[3]<pressTimer[2])) //MOVE RIGHT
            moveX += getMovementSpeed(game_state);
    }
    //moveToNewPosition(game_state.game_map, dX, dY);
    int ppt = getPixelsPerTile();
    //set angle
    setAngle(atan2(getMouseY() - ((getY() - game_state.game_map.getCameraY())*ppt), getMouseX() - ((getX() - game_state.game_map.getCameraX())*ppt)));
    //manage weapons
    for(auto &i: game_state.inputEvents)
    {
        switch(i.type)
        {
        case SDL_MOUSEWHEEL:
            {
                auto &w = weapons;
                if(i.wheel.y < 0)
                    curWeapon = (curWeapon + 1) % weapons.size();
                else curWeapon = (curWeapon + weapons.size() - 1) % weapons.size();
                w[curWeapon].resetFireCounter(this);
            }
            break;
        case SDL_KEYDOWN:
            if(input.key.keysym.sym == SDLK_q)
                curWeapon = (curWeapon + weapons.size() - 1) % weapons.size();
            else if(input.key.keysym.sym == SDLK_e)
                curWeapon = (curWeapon + 1) % weapons.size();
            break;
        }
    }
}
/*void Player::draw(const GameState &game_state) const
{
    using namespace sdl_settings;
    int ppt = game_state.getPixelsPerTile();
    renderCopy(sprites[type], (game_state.getFloorDisplayW() - ppt)/2, (game_state.getFloorDisplayH() - ppt)/2, ppt, ppt);
    if(!weapons.empty())
    {
        double angle = getAngle();
        weapons[curWeapon].draw_wield(game_state, game_state.getFloorDisplayW()/2 + ppt*std::cos(angle)/4, game_state.getFloorDisplayH()/2 + ppt*std::sin(angle)/4, angle);
    }
    else print_warning("Player has no weapons");
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
}*/
void Player::handleCollisionsWithHurtTiles(GameState &game_state) //and collapsing tiles too
{
    shape->handleCollisionsWithCollapsingTiles(game_state.game_map);
    if(shape->collidesWithTile(game_state.game_map, MapTile::hurt))
    {
        double dmg = TemporaryMod::getVarValue(tempMods, TemporaryMod::Mod::HurtTileDamage, 2 * TICK_SPEED * game_state.currentFloor);
        dealDamage(nullptr, this, dmg);
    }
}
bool Player::isOnPortal(GameState &game_state) const
{
    for(int i=0; i<game_state.game_map.getNumColumns(); i++)
    {
        for(int j=0; j<game_state.game_map.getNumRows(); j++)
        {
            if(game_state.game_map.tiles[i][j] == MapTile::portal)
            {
                auto portal_shape = std::make_unique<Circle>(i, j, 0.4);
                return shape->collidesWith(portal_shape.get());
            }
        }
    }
    return shape->collidesWithTile(game_state.game_map, MapTile::portal);
}
