#include "item.h"
#include "sdl_base.h"
#include <string>
SDL_Texture *Item::sprites[Item::NUM_ITEMS];
std::string Item::BASE_NAME[Item::NUM_ITEMS], Item::BASE_DESC[Item::NUM_ITEMS];
int Item::BASE_COST[Item::NUM_ITEMS];
Item::Item(){};
Item::Item(int name)
{
    this->name = (Item::Name)name;
}
Item::Item(Item::Name name)
{
    this->name = name;
}
