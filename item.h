#pragma once
#include <string>
#include <SDL2/SDL.h>
class Item
{
public:
    static constexpr int NUM_ITEMS = 11;
    enum Name{SpikedVest = 0, Bullseye = 1, DirtySerratedShirt = 2, BlueBoots = 3, RedBoots = 4,
    SadistsMachete = 5, GreenBoots = 6, AmuletOfAir = 7, DaltonsAtom = 8, Toothbrush = 9,
    RedGloves = 10, None = NOT_SET} name;
    static SDL_Texture *sprites[NUM_ITEMS];
    static std::string BASE_NAME[NUM_ITEMS], BASE_DESC[NUM_ITEMS];
    static int BASE_COST[NUM_ITEMS];
    static void init();
    Item();
    Item(int name);
    Item(Name name);
};
