#pragma once
#include <string>
#include <SDL2/SDL_mixer.h>
#include <queue>
#include <vector>
#include "fundamentals.h"
#define GAME_VERSION_MAJOR 0
#define GAME_VERSION_MINOR 0
#define GAME_VERSION_PATCH 2
#define GAME_VERSION_DESC "alpha"
enum class global_game_state{in_game, menu};
extern global_game_state current_global_game_state;
enum class Affiliation{player, generic_enemy};
class GameState;
extern int ITER_PER_FRAME;
const double MAX_RADIUS = 3; //max size of a unit/projectile
 //determines how far we have to look when partitioning. should be equal to MAX_RADIUS/2 + (MAX UNIT MS/FRAME =~ 0) + 1.
const double PARTITION_DIST = MAX_RADIUS/2 + 1;
extern double SPRITE_SCALE;
extern int BASE_FPS; //I programmed the game assuming this is the fps
extern double TICK_SPEED;
extern int RAW_DIFFICULTY, RAW_CPU_SETTING;
extern int pixelsPerTile;
extern double DIFFICULTY; //higher = more difficult
extern bool SHOW_EXTRA_INFO;
extern bool finished_transition_screen; //has the transition fade out/in completed yet?
extern bool show_hitboxes;
extern fast_map<std::string, Mix_Music*> music;
extern fast_map<std::string, Mix_Chunk*> music_chunks;
bool areEnemies(Affiliation a, Affiliation b, bool recursed = false);
void calculateGameVars();
//returns the number of game ticks that elapse in s seconds, rounded to the nearest integer
int ticks_in_s(double s); //always returns at least 1 to avoid div by 0
int getPixelsPerTile();
void setPixelsPerTile(int p);
void updateExtraAesthetics();
void drawExtraAesthetics();
void clearExtraAesthetics();
void initMusic();
void switch_to_music(std::string name);
struct audio_chunk
{
    Mix_Chunk *chunk;
    double volMult;
    audio_chunk();
    audio_chunk(Mix_Chunk *c, double v = 1);
    audio_chunk(const char *name, double v = 1);
    void play() const;
};
class AestheticEffect
{
public:
    virtual bool shouldRemove() const = 0;
    virtual void update(const GameState &game_state) = 0;
    virtual void draw(const GameState &game_state) const = 0;
};
extern std::vector<std::shared_ptr<AestheticEffect> > aestheticEffects;
class DisappearingObject: public AestheticEffect
{
    SDL_Texture *t;
    double x, y, angle;
    double duration, timeLeft;
public:
    bool destroyTexture;
    DisappearingObject(SDL_Texture *t, double timeLeft, double x, double y, double angle, bool destroyTexture = false);
    bool shouldRemove() const override;
    void draw(const GameState &game_state) const override;
    void update(const GameState &game_state) override;
    ~DisappearingObject();
};

class TextBox: public AestheticEffect //fades in and out
{
    std::string text;
    double x, y, w, h;
    double duration, timeLeft;
public:
    static bool is_first_draw, is_first_update; //only operate one text box at a time
    TextBox(std::string text, double duration, double x, double y, double w, double h);
    bool shouldRemove() const override;
    void draw(const GameState &game_state) const override;
    void update(const GameState &game_state) override;
};

struct VerticalTextDrawer
{
    int x, y, h, maxx;
    enum class Justify{left, right};
    VerticalTextDrawer(int x, int y, int h, int maxx = 0);
    void draw(std::string text = "", Justify justify = Justify::left, uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=255);
    void drawMultilineUnbroken(std::string text, uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=255);
    void drawOnSameLine(std::string text, Justify justify = Justify::left, uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=255);
    void fillRect(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) const;
    void drawRect(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) const;
};

class CPULoadCalculator
{
    std::queue<std::pair<ll, ll> > cpuTimes; //(time when this was recorded, cpu interval)
    ll cpuTimeSum, time_range, t1;
public:
    CPULoadCalculator(ll time_range);
    void begin_counting();
    void end_counting();
    double get_load();
};

class Background
{
    class Func
    {
    protected:
        int xoffset, yoffset, r, g, b, a;
        double priority, timeLeft, maxTime;
    public:
        Func(double priority, double tim, int xo, int yo, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        void update();
        virtual void draw(int x, int y) const = 0;
        bool shouldRemove() const;
        bool operator < (const Func &other) const;
        static bool comp_by_priority(const std::unique_ptr<Func> &a, const std::unique_ptr<Func> &b);
    };
    class FillRect: public Func
    {
        int w, h;
    public:
        FillRect(double priority, double tim, int xo, int yo, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        void draw(int x, int y) const override;
    };
    class ClearScreen: public Func
    {
    public:
        ClearScreen(double priority, double tim, uint8_t r, uint8_t g, uint8_t b);
        void draw(int x, int y) const override;
    };
    std::vector<std::unique_ptr<Func> > funcs;
public:
    //note: higher priority = drawn earlier. priority is essentially the distance from the player / depth of object
    //priority affects how xoffset and yoffset work
    void addRect(double priority, double tim, int xo, int yo, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void addClearScreen(double priority, double tim, uint8_t r, uint8_t g, uint8_t b);
    void update();
    void clear();
    void draw(int x, int y) const;
};
extern Background background;
