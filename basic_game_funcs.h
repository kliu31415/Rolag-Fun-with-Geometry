#pragma once
#include <string>
#include <SDL2/SDL.h>
enum class Affiliation{player, generic_enemy};
bool areEnemies(Affiliation a, Affiliation b, bool recursed = false);
class GameState;
const int ITER_PER_FRAME = 5;
const double PARTITION_DIST = 2; //determines how far we have to look when partitioning. should be equal to MAX_UNIT_SIZE/2 + 1.
class DisappearingObject
{
    int timeLeft, maxTime;
    SDL_Texture *t;
    double x, y;
public:
    DisappearingObject(SDL_Texture *t, int timeLeft, double x, double y);
    bool shouldRemove() const;
    void render(const GameState &game_state) const;
    void update();
};

struct VerticalTextDrawer
{
    int x, y, h, maxx;
    enum class Justify{left, right};
    VerticalTextDrawer(int x, int y, int h, int maxx = 0);
    void render(std::string text = "", Justify justify = Justify::left, uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=0);
    void renderOnSameLine(std::string text, Justify justify = Justify::left, uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=0);
    void fillRect(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) const;
    void drawRect(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) const;
};
