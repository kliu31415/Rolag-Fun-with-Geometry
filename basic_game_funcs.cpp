#include "basic_game_funcs.h"
#include "game_state.h"
#include "sdl_base.h"
#include "fundamentals.h"
global_game_state current_global_game_state;
Background background;
int RAW_DIFFICULTY = 3, RAW_CPU_SETTING = 4;
double DIFFICULTY;
int ITER_PER_FRAME, BASE_FPS = 60;
double SPRITE_SCALE = 1;
double TICK_SPEED = 1.0 / (BASE_FPS * ITER_PER_FRAME);
bool SHOW_EXTRA_INFO = true;
int pixelsPerTile = 100;
bool finished_transition_screen;
bool show_hitboxes = false;
bool TextBox::is_first_draw, TextBox::is_first_update;
std::vector<std::shared_ptr<AestheticEffect> > aestheticEffects;
fast_map<std::string, Mix_Music*> music;
fast_map<std::string, Mix_Chunk*> music_chunks;
audio_chunk::audio_chunk()
{
    chunk = NULL;
}
audio_chunk::audio_chunk(Mix_Chunk *c, double v)
{
    this->chunk = c;
    this->volMult = v;
}
audio_chunk::audio_chunk(const char *name, double v)
{
    this->chunk = loadMixChunk(name);
    this->volMult = v;
}
void audio_chunk::play() const
{
    if(chunk != NULL)
    {
        Mix_VolumeChunk(chunk, sdl_settings::sfxVolume * this->volMult);
        Mix_PlayChannel(-1, chunk, 0);
    }
}
void switch_to_music(std::string name)
{
    static std::string currently_playing = "@";
    if(currently_playing == name)
        return;
    currently_playing = name;
    Mix_FadeOutMusic(400);
    if(name == "")
        return;
    if(!music.count(name))
        print_warning("could not find music with name " + name);
    else
    {
        currently_playing = name;
        Mix_FadeInMusic(music[name], -1, 400);
    }
}
void initMusic()
{
    music["main menu"] = loadMixMusic("data/audio/main_menu.mp3");
    music["floor 1"] = loadMixMusic("data/audio/floor_1.mp3");
    music["floor 3"] = loadMixMusic("data/audio/floor_3.mp3");
    music["floor 5"] = loadMixMusic("data/audio/floor_5.mp3");
    music["floor 7"] = loadMixMusic("data/audio/floor_7.mp3");
    music["floor 13"] = loadMixMusic("data/audio/floor_13.mp3");
    music["floor 15"] = loadMixMusic("data/audio/floor_15.mp3");
    music["floor 18"] = loadMixMusic("data/audio/floor_18.mp3");
}
void calculateGameVars()
{
    switch(RAW_CPU_SETTING)
    {
    case 0:
        ITER_PER_FRAME = 1;
        break;
    case 1:
        ITER_PER_FRAME = 2;
        break;
    case 2:
        ITER_PER_FRAME = 4;
        break;
    case 3:
        ITER_PER_FRAME = 7;
        break;
    case 4:
        ITER_PER_FRAME = 11;
        break;
    case 5:
        ITER_PER_FRAME = 18;
        break;
    case 6:
        ITER_PER_FRAME = 30;
        break;
    case 7:
        ITER_PER_FRAME = 50;
        break;
    case 8:
        ITER_PER_FRAME = 80;
        break;
    case 9:
        ITER_PER_FRAME = 125;
        break;
    case 10:
        ITER_PER_FRAME = 200;
        break;
    default:
        print_error("Unknown RAW_DIFFICULTY value");
        break;
    }
    switch(RAW_DIFFICULTY)
    {
    case 0:
        DIFFICULTY = 0.63;
        break;
    case 1:
        DIFFICULTY = 0.75;
        break;
    case 2:
        DIFFICULTY = 0.87;
        break;
    case 3:
        DIFFICULTY = 1;
        break;
    case 4:
        DIFFICULTY = 1.13;
        break;
    case 5:
        DIFFICULTY = 1.25;
        break;
    case 6:
        DIFFICULTY = 1.36;
        break;
    case 7:
        DIFFICULTY = 1.47;
        break;
    default:
        print_error("Unknown RAW_DIFFICULTY value");
        break;
    }
}
int ticks_in_s(double s)
{
    return std::max(1, (int)(s / TICK_SPEED));
}
int getPixelsPerTile()
{
    return pixelsPerTile;
}
void setPixelsPerTile(int p)
{
    pixelsPerTile = p;
    SPRITE_SCALE = p / 100.0;
}
//Affiliation
bool areEnemies(Affiliation a, Affiliation b, bool recursed)
{
    if(a==Affiliation::player && b==Affiliation::generic_enemy)
        return true;
    if(!recursed)
        return areEnemies(b, a, true);
    return false;
}
//DisappearingObject
DisappearingObject::DisappearingObject(SDL_Texture *t, double timeLeft, double x, double y, double angle, bool destroyTexture)
{
    this->t = t;
    this->duration = this->timeLeft = timeLeft;
    this->x = x;
    this->y = y;
    this->angle = angle;
    this->destroyTexture = destroyTexture;
}
void DisappearingObject::draw(const GameState &game_state) const
{
    int ppt = getPixelsPerTile();
    int rw, rh;
    SDL_QueryTexture(t, NULL, NULL, &rw, &rh);
    int rx = (x - game_state.game_map.getCameraX())*ppt - rw * SPRITE_SCALE /2;
    int ry = (y - game_state.game_map.getCameraY())*ppt - rh * SPRITE_SCALE /2;
    uint8_t origA;
    SDL_GetTextureAlphaMod(t, &origA);
    SDL_SetTextureAlphaMod(t, 255 * timeLeft / duration);
    renderCopyEx(t, rx, ry, rw * SPRITE_SCALE, rh * SPRITE_SCALE, TO_DEG(angle));
    SDL_SetTextureAlphaMod(t, origA);
}
void DisappearingObject::update(const GameState &game_state)
{
    timeLeft -= 1.0 / std::max(getFPS(), 1); //getFPS is a good approximation for frame length
}
bool DisappearingObject::shouldRemove() const
{
    return timeLeft <= 0;
}
DisappearingObject::~DisappearingObject()
{
    if(destroyTexture)
        SDL_DestroyTexture(t);
}
//TextBox
TextBox::TextBox(std::string text, double duration, double x, double y, double w, double h)
{
    this->text = text;
    this->duration = this->timeLeft = duration;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}
bool TextBox::shouldRemove() const
{
    return timeLeft <= 0;
}
void TextBox::draw(const GameState &game_state) const
{
    if(!is_first_draw)
        return;
    is_first_draw = true;
    uint8_t alpha;
    if(timeLeft > duration - 0.5) //relatively new
        alpha = 460 * (duration - timeLeft);
    else if(timeLeft < 0.5) //oldish
        alpha = 460 * timeLeft;
    else alpha = 230;
    int W = getWindowW(), H = getWindowH();
    fillRect(x*W, y*H, w*W, h*H, 200, 200, 200, alpha);
    int fsz = getFontSize(0);
    drawMultilineTextUnbroken(text, x*W, y*H, w*W, fsz, 0, 0, 0, alpha);
    static std::string close_text = "[Z] to close";
    fsz *= 0.7;
    drawText(close_text, (x+w)*W - close_text.size() * fsz/2, (y+h)*H - fsz, fsz, 0, 0, 0, alpha);
}
void TextBox::update(const GameState &game_state)
{
    if(!is_first_update)
        return;
    is_first_update = false;
    timeLeft -= 1.0 / std::max(getFPS(), 1); //getFPS is a good approximation for frame length
    for(auto &i: game_state.inputEvents)
        if(i.type==SDL_KEYDOWN && i.key.keysym.sym==SDLK_z)
        {
            if(timeLeft > duration - 0.5) //more smooth transitions for text boxes that are closed quickly
                timeLeft = duration - timeLeft;
            else timeLeft = 0.5;
        }

}
//VerticalTextDrawer
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
        drawText(text, maxx - w, y, h, r, g, b, a);
        break;
    case Justify::left:
        drawText(text, x, y, h, r, g, b, a);
        break;
    }
    y += h;
}
void VerticalTextDrawer::drawMultilineUnbroken(std::string text, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    y += h * drawMultilineTextUnbroken(text, x, y, maxx - x, h, r, g, b, a);
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
//CPULoadCalculator
CPULoadCalculator::CPULoadCalculator(ll time_range)
{
    this->time_range = time_range;
    this->cpuTimeSum = 0;
    this->t1 = -NOT_SET; //to help debug
}
void CPULoadCalculator::begin_counting()
{
    ll t = getTicksNs();
    this->t1 = t;
}
void CPULoadCalculator::end_counting()
{
    if(t1 == -NOT_SET)
        print_warning("CPULoadCalculator::end_counting was called before begin_counting");
    ll t2 = getTicksNs();
    cpuTimes.emplace(t1, t2 - t1);
    cpuTimeSum += t2 - t1;
    t1 = -NOT_SET;
}
double CPULoadCalculator::get_load()
{
    ll t = getTicksNs();
    while(!cpuTimes.empty() && cpuTimes.front().A + time_range < t)
    {
        cpuTimeSum -= cpuTimes.front().B;
        cpuTimes.pop();
    }
    if(cpuTimes.size() > 2)
    {
        double div = cpuTimes.back().A + cpuTimes.back().B - cpuTimes.front().A;
        if(div != 0)
            return cpuTimeSum / div;
    }
    return 0;
}
//Func
Background::Func::Func(double priority, double tim, int xo, int yo, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    this->priority = priority;
    this->timeLeft = this->maxTime = tim;
    this->xoffset = xo;
    this->yoffset = yo;
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
void Background::Func::update()
{
    timeLeft -= 1.0 / BASE_FPS;
}
bool Background::Func::operator < (const Func &other) const
{
    return priority < other.priority;
}
bool Background::Func::shouldRemove() const
{
    return timeLeft <= 0;
}
//FillRect
Background::FillRect::FillRect(double priority, double tim, int xo, int yo, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a):
    Func(priority, tim, xo, yo, r, g, b, a)
{
    this->w = w;
    this->h = h;
}
void Background::FillRect::draw(int x, int y) const //priority simulates depth/distance from player
{
    fillRect(xoffset + x / priority, yoffset + y / priority, w, h, r, g, b, a * std::min(1.0, std::min(maxTime - timeLeft, timeLeft)));
}
//ClearScreen
Background::ClearScreen::ClearScreen(double priority, double tim, uint8_t r, uint8_t g, uint8_t b):
    Func(priority, tim, 0, 0, r, g, b, 255)
{

}
void Background::ClearScreen::draw(int x, int y) const
{
    fillRect(0, 0, getDisplayW(), getDisplayH(), r, g, b, 255);
}
//Background
bool Background::Func::comp_by_priority(const std::unique_ptr<Func> &a, const std::unique_ptr<Func> &b)
{
    return a->priority < b->priority;
}
void Background::addRect(double priority, double tim, int xo, int yo, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    funcs.emplace_back(std::make_unique<FillRect>(priority, tim, xo, yo, w, h, r, g, b, a));
    std::sort(funcs.begin(), funcs.end(), Func::comp_by_priority);
    std::reverse(funcs.begin(), funcs.end());
}
void Background::addClearScreen(double priority, double tim, uint8_t r, uint8_t g, uint8_t b)
{
    funcs.emplace_back(std::make_unique<ClearScreen>(priority, tim, r, g, b));
    std::sort(funcs.begin(), funcs.end(), Func::comp_by_priority);
    std::reverse(funcs.begin(), funcs.end());
}
void Background::update()
{
    for(int i=0; i<(int)funcs.size(); i++)
    {
        funcs[i]->update();
        if(funcs[i]->shouldRemove())
        {
            funcs.erase(funcs.begin() + i);
            i--;
        }
    }
}
void Background::draw(int x, int y) const
{
    for(auto &i: funcs)
        i->draw(x, y);
}
void Background::clear()
{
    funcs.clear();
}
