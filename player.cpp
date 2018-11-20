#include "player.h"
#include "sdl_base.h"
#include "game_map.h"
#include "game_state.h"
#include "fundamentals.h"
#include <algorithm>
#include <cmath>
using namespace std;
Player::Player(){}
Player::Player(double startX, double startY): Unit(0, Affiliation::player, startX, startY)
{
    moveUp = SDL_SCANCODE_W;
    moveDown = SDL_SCANCODE_S;
    moveLeft = SDL_SCANCODE_A;
    moveRight = SDL_SCANCODE_D;
    pressTimer.resize(4, 0);
    money = 100;
    weapons.emplace_back(0);
    weapons.emplace_back(1);
}
void Player::updateKeyPressTimer(const uint8_t *keystate, int &timer, SDL_Scancode key)
{
    if(keystate[key])
        timer++;
    else timer = 0;
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
    moveX = 0;
    moveY = 0;
    if(pressTimer[0]>0 && (pressTimer[1]==0 || pressTimer[0]<pressTimer[1])) //MOVE UP
        moveY--;
    if(pressTimer[1]>0 && (pressTimer[0]==0 || pressTimer[1]<pressTimer[0])) //MOVE DOWN
        moveY++;
    if(pressTimer[2]>0 && (pressTimer[3]==0 || pressTimer[2]<pressTimer[3])) //MOVE LEFT
        moveX--;
    if(pressTimer[3]>0 && (pressTimer[2]==0 || pressTimer[3]<pressTimer[2])) //MOVE RIGHT
        moveX++;
    if(moveX!=0 || moveY!=0)
    {
        double mult = getMovementSpeed() / std::hypot(moveX, moveY);
        moveX *= mult;
        moveY *= mult;
    }
    //moveToNewPosition(game_state.game_map, dX, dY);
    //update weapon ang
    angle = atan2(getMouseY() - game_state.getFloorDisplayH()/2, getMouseX() - game_state.getFloorDisplayW()/2);
    //manage weapons
    for(auto &i: game_state.inputEvents)
    {
        if(i.type == SDL_MOUSEWHEEL)
        {
            auto &w = weapons;
            if(i.wheel.y < 0)
                curWeapon = (curWeapon + 1) % weapons.size();
            else curWeapon = (curWeapon + weapons.size() - 1) % weapons.size();
            w[curWeapon].resetFireCounter();
        }
    }
}
void Player::render(const GameState &game_state) const
{
    using namespace sdl_settings;
    int ppt = game_state.getPixelsPerTile();
    renderCopy(sprites[type], (game_state.getFloorDisplayW() - ppt)/2, (game_state.getFloorDisplayH() - ppt)/2, ppt, ppt);
    if(!weapons.empty())
    {
        weapons[curWeapon].render_wield(game_state, game_state.getFloorDisplayW()/2 + ppt*std::cos(angle)/4, game_state.getFloorDisplayH()/2 + ppt*std::sin(angle)/4, angle);
    }
    if(showHPbar)
    {
        int W = shape->getSize() * ppt;
        int H = W * 0.2;
        int X = (getX() - game_state.game_map.getCameraX()) * ppt - W/2;
        int Y = (getY() - game_state.game_map.getCameraY()) * ppt - W*0.55 - H;
        double hpPercent = HP / maxHP;
        fillRect(X, Y, hpPercent * W, H, 0, 255, 0, 150);
        fillRect(X + hpPercent*W, Y, (1 - hpPercent)*W, H, 255, 0, 0, 150);
        drawRect(X, Y, W, H, 0, 0, 0);
    }
}
