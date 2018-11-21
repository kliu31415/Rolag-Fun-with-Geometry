#include "basic_game_funcs.h"
#include "game_state.h"
///Affiliation
bool areEnemies(Affiliation a, Affiliation b, bool recursed)
{
    if(a==Affiliation::player && b==Affiliation::generic_enemy)
        return true;
    if(!recursed)
        return areEnemies(b, a, true);
    return false;
}
///DisappearingObject
DisappearingObject::DisappearingObject(SDL_Texture *t, int timeLeft, double x, double y, double angle)
{
    this->t = t;
    this->timeLeft = this->maxTime = timeLeft;
    this->x = x;
    this->y = y;
    this->angle = angle;
}
void DisappearingObject::draw(const GameState &game_state) const
{
    int ppt = game_state.getPixelsPerTile();
    int rw, rh;
    SDL_QueryTexture(t, NULL, NULL, &rw, &rh);
    int rx = (x - game_state.game_map.getCameraX())*ppt - rw * SPRITE_SCALE /2;
    int ry = (y - game_state.game_map.getCameraY())*ppt - rh * SPRITE_SCALE /2;
    uint8_t origA;
    SDL_GetTextureAlphaMod(t, &origA);
    SDL_SetTextureAlphaMod(t, 255 * timeLeft / maxTime);
    renderCopyEx(t, rx, ry, rw, rh, TO_DEG(angle));
    SDL_SetTextureAlphaMod(t, origA);
}
void DisappearingObject::update()
{
    timeLeft--;
}
bool DisappearingObject::shouldRemove() const
{
    return timeLeft <= 0;
}
///VerticalTextDrawer
VerticalTextDrawer::VerticalTextDrawer(int x, int y, int h, int maxx)
{
    this->x = x;
    this->y = y;
    this->h = h;
    this->maxx = maxx;
}
void VerticalTextDrawer::draw(std::string text, Justify justify, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    int w;
    switch(justify)
    {
    case Justify::right:
        getTextSize(text, h, &w, NULL);
        drawText(text, maxx - w, y, h, r, g, b);
        break;
    case Justify::left:
        drawText(text, x, y, h, r, g, b);
        break;
    }
    y += h;
}
void VerticalTextDrawer::drawOnSameLine(std::string text, Justify justify, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    y -= h;
    draw(text, justify, r, g, b, a);
}
void VerticalTextDrawer::fillRect(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const
{
    ::fillRect(x, y, maxx-x, h, r, g, b, a);
}
void VerticalTextDrawer::drawRect(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const
{
    ::drawRect(x, y, maxx-x, h, r, g, b, a);
}
