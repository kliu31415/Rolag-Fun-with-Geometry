#include "item.h"
#include "sdl_base.h"
#include <string>
SDL_Texture *Item::sprites[Item::NUM_ITEMS];
std::string Item::BASE_NAME[Item::NUM_ITEMS], Item::BASE_DESC[Item::NUM_ITEMS];
int Item::BASE_COST[Item::NUM_ITEMS];
bool Item::IS_SHOWN[Item::NUM_ITEMS];
Item::Item(int type)
{
    this->type = type;
}
Item::Item(Name name)
{
    this->type = (int)name;
}
