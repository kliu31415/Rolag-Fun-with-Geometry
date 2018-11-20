#pragma once
#include <string>
#include <SDL2/SDL.h>
class Item
{
public:
    static constexpr int NUM_ITEMS = 7;
    static SDL_Texture *sprites[NUM_ITEMS];
    static std::string BASE_NAME[NUM_ITEMS], BASE_DESC[NUM_ITEMS];
    static int BASE_COST[NUM_ITEMS];
    int type;
    static void init();
    Item();
    Item(int type);
};
