#include "menu.h"
#include "fundamentals.h"
#include "basic_game_funcs.h"
#include "in_game.h"
#include "game_state.h"
std::shared_ptr<StandardMenu> main_menu, program_info_menu, options_menu, game_options_menu, cpugpu_menu, death_menu, pause_menu;
std::stack<std::shared_ptr<Menu> > menuStack;
fast_map<std::string, std::shared_ptr<ConsoleWindow> > consoleWindows;
GameState *Menu::game_state;
void addToMenuStack(std::shared_ptr<Menu> menu)
{
    menuStack.push(menu);
    menu->reset();
}
MenuOption::MenuOption(std::string text, std::string description, void (*funcOnPress)(InputType), std::string (*displayVar)())
{
    this->text = text;
    this->description = description;
    this->funcOnPress = funcOnPress;
    this->displayVar = displayVar;
}
StandardMenu::StandardMenu(std::string title)
{
    this->title = title;
    textSizeMult = 1;
    widthF = 0.6; //fraction of the screen, NOT a raw value in pixels
    curOption = 0;
    customDrawingFunc = [](int) -> void {};
}
void StandardMenu::operate()
{
    SDL_Event input;
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN:
            switch(input.key.keysym.sym)
            {
            case SDLK_RETURN:
            case SDLK_RETURN2:
                if(options[curOption].funcOnPress != nullptr)
                    options[curOption].funcOnPress(MenuOption::InputType::enter);
                break;
            case SDLK_UP:
            case SDLK_w:
                curOption = (curOption + options.size() - 1) % options.size();
                break;
            case SDLK_DOWN:
            case SDLK_s:
                curOption = (curOption + 1) % options.size();
                break;
            case SDLK_LEFT:
            case SDLK_a:
                if(options[curOption].funcOnPress != nullptr)
                    options[curOption].funcOnPress(MenuOption::InputType::left);
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                if(options[curOption].funcOnPress != nullptr)
                    options[curOption].funcOnPress(MenuOption::InputType::right);
                break;
            }
        }
    }
}
void StandardMenu::draw(uint8_t alpha /*=255*/, bool clearScreen /*=true*/) //CODE_WARNING: drawing the "program info" menu resulted in the program not responding once. Unsure of how to reproduce.
{
    if(clearScreen)
        renderClear(200, 200, 200, alpha);
    int fsz = getFontSize(0.5) * textSizeMult;
    int borderSize = fsz / 2;
    VerticalTextDrawer menuText((1 - widthF)/2 * getWindowW(),
            (getWindowH() - fsz * (int)options.size())/2, fsz, (1 + widthF)/2 * getWindowW());
    drawText(title, menuText.x, menuText.y - borderSize - fsz, fsz, 0, 0, 0, alpha);
    fillRect(menuText.x - borderSize, menuText.y - borderSize, menuText.maxx - menuText.x + borderSize * 2,
             fsz * (int)options.size() + borderSize * 2, 170, 170, 170, alpha);
    fillRect(menuText.x, menuText.y, menuText.maxx - menuText.x, fsz * (int)options.size(), 220, 220, 220, alpha);
    for(int i=0; i<(int)options.size(); i++)
    {
        if(i == curOption)
            menuText.fillRect(255, 0, 0, (50/255.0) * alpha);
        menuText.draw(options[i].text, VerticalTextDrawer::Justify::left, 0, 0, 0, alpha);
        if(options[i].displayVar != nullptr)
            menuText.drawOnSameLine(options[i].displayVar(), VerticalTextDrawer::Justify::right, 0, 100, 0, alpha);
    }
    menuText.y += borderSize;
    if(customDrawingFunc != nullptr)
        customDrawingFunc(menuText.y); //doesn't work with the alpha value
    menuText.h *= 0.7; //the description should be in a bit smaller font
    menuText.drawMultilineUnbroken(options[curOption].description, 0, 0, 0, alpha);
}
void StandardMenu::add_back_button()
{
    options.emplace_back("Back", "",
        [](MenuOption::InputType t )-> void {
            if(t == MenuOption::InputType::enter)
            {
                menuStack.pop();
            }});
}
namespace ProgramInfo
{
    std::string compiler, OS, sdl_version, game_version;
    int bits;
    void init()
    {
        //compiler
        #if defined(__ICC) || defined(__ICL)
        compiler = "Intel C++";
        #elif __GNUC__
        compiler = "GCC ";
        compiler += to_str(__GNUC__) + "." + to_str(__GNUC_MINOR__) + "." + to_str(__GNUC_PATCHLEVEL__);
        #elif _MSC_VER
        compiler = "MSVC " + to_str((int)_MSC_VER);
        #elif __clang__
        compiler = "clang " + __clang_version__;
        #else
        compiler = "unknown";
        #endif
        //OS
        #if defined(_WIN64) || defined(_WIN32)
        OS = "Windows";
        #elif __linux__
        OS = "Linux";
        #elif TARGET_OS_MAC
        OS = "Apple Mac";
        #else
        OS = "unknown";
        #endif // _WIN64
        //get SDL2 version
        SDL_version v;
        SDL_VERSION(&v);
        sdl_version = to_str((int)v.major) + "." + to_str((int)v.minor) + "." + to_str((int)v.patch);
        game_version = to_str(GAME_VERSION_MAJOR) + "." + to_str(GAME_VERSION_MINOR) + "." + to_str(GAME_VERSION_PATCH) + " " + GAME_VERSION_DESC;
    }
    void display(int y)
    {
        int fsz = getFontSize(0);
        VerticalTextDrawer text(0, y, fsz, getWindowW());
        text.draw("Creator: assert0");
        text.draw("Game host: kworker & friends");
        text.draw("Game name: FUWIGE (FUn WIth GEometry) v" + game_version);
        //text.draw("Website: fuwige.blogspot.com");
        text.draw("Build time: " + to_str(__DATE__) + " " + to_str(__TIME__));
        text.draw("Compiler: " + compiler);
        text.draw("Operating System: " + OS);
        text.draw("Graphics: SDL2 v" + sdl_version + " (zlib License)");
        text.draw("Credits: see credits.txt");
    }
}
void StandardMenu::reset()
{
    curOption = 0;
}
void StandardMenu::init()
{
    //
    main_menu = std::make_shared<StandardMenu>("Main Menu");
    main_menu->options.emplace_back("Play", "Start a new run",
        [](MenuOption::InputType t) -> void {
            if(t == MenuOption::InputType::enter)
            {
                switch_to_music("");
                init_new_game(currentGame);
            }});
    main_menu->options.emplace_back("Options", "",
        [](MenuOption::InputType t) -> void {
            if(t == MenuOption::InputType::enter)
            {
                addToMenuStack(options_menu);
            }});
    main_menu->options.emplace_back("Give Feedback", "You should give feedback by telling me what's unbalanced/boring",
        [](MenuOption::InputType t) -> void {
            if(t == MenuOption::InputType::enter)
            {
                #ifdef _WIN32
                std::system("explorer mailto:kliu31415@gmail.com");
                #elif __linux__
                std::system("sensible-browser mailto:kliu31415@gmail.com");
                #endif
            }});
    main_menu->options.emplace_back("Program Info", "Displays program info",
        [](MenuOption::InputType t) -> void {
            if(t == MenuOption::InputType::enter)
            {
                addToMenuStack(program_info_menu);
            }});
    main_menu->options.emplace_back("Exit", "Self-explanatory (I hope)",
        [](MenuOption::InputType t) -> void {
            if(t == MenuOption::InputType::enter)
            {
                exit(EXIT_SUCCESS);
            }});
    //sd
    program_info_menu = std::make_shared<StandardMenu>("Program Info");
    ProgramInfo::init();
    program_info_menu->customDrawingFunc = ProgramInfo::display;
    program_info_menu->add_back_button();
    //
    options_menu = std::make_shared<StandardMenu>("Options");
    options_menu->options.emplace_back("Game Options", "Customize the game!",
        [](MenuOption::InputType t )-> void {
            if(t == MenuOption::InputType::enter)
            {
                addToMenuStack(game_options_menu);
            }});
    options_menu->options.emplace_back("CPU/Graphics/Music", "Miscellaneous options",
        [](MenuOption::InputType t )-> void {
            if(t == MenuOption::InputType::enter)
            {
                addToMenuStack(cpugpu_menu);
            }});
    options_menu->add_back_button();
    //
    game_options_menu = std::make_shared<StandardMenu>("Game Options");
    game_options_menu->options.emplace_back("Difficulty", "Higher = more difficult.",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
                RAW_DIFFICULTY = std::max(0, RAW_DIFFICULTY - 1);
                break;
            case MenuOption::InputType::right:
                RAW_DIFFICULTY = std::min(7, RAW_DIFFICULTY + 1);
                break;
            default:
                break;
            }}, []()-> std::string {
            switch(RAW_DIFFICULTY)
            {
            case 0:
                return "Bonobo";
            case 1:
                return "Uber noob";
            case 2:
                return "Ezpz";
            case 3:
                return "Standard";
            case 4:
                return "Hard";
            case 5:
                return "Harder";
            case 6:
                return "Hardest";
            case 7:
                return "Hardester";
            default:
                print_error("Unknown RAW_DIFFICULTY value");
                return "Error";
            }}
            );
    game_options_menu->add_back_button();
    //
    cpugpu_menu = std::make_shared<StandardMenu>("CPU/Graphics/Music");
    cpugpu_menu->options.emplace_back("CPU", "Note: this assumes you are running the game at 60fps. "
                                      "If you run at a higher fps, you may have to lower this setting.",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left: //set min at 3 because of consistency issues at lower tick rates
                RAW_CPU_SETTING = std::max(3, RAW_CPU_SETTING - 1);
                calculateGameVars(); //gotta call it here in case people change the settings in the pause menu
                break;
            case MenuOption::InputType::right:
                RAW_CPU_SETTING = std::min(10, RAW_CPU_SETTING + 1);
                calculateGameVars(); //gotta call it here in case people change the settings in the pause menu
                break;
            default:
                break;
            }}, []()-> std::string {
            switch(RAW_CPU_SETTING)
            {
            case 0:
                return "Potato";
            case 1:
                return "Grandma's Toaster";
            case 2:
                return "Deluxe Toaster";
            case 3:
                return "Chromebook";
            case 4:
                return "Budget PC";
            case 5:
                return "Normal PC";
            case 6:
                return "Fast PC";
            case 7:
                return "Gaming PC";
            case 8:
                return "Mini Supercomputer";
            case 9:
                return "Supercomputer";
            case 10:
                return "Tianhe 4";
            default:
                print_error("Unknown RAW_DIFFICULTY value");
                return "Error";
            }}
            );
    cpugpu_menu->options.emplace_back("Show Extra Info", "Shows the program's utilization fraction of a CPU core (and possibly other things)",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
            case MenuOption::InputType::right:
            case MenuOption::InputType::enter:
                SHOW_EXTRA_INFO = !SHOW_EXTRA_INFO;
                break;
            default:
                break;
            }}, []()-> std::string {
            if(SHOW_EXTRA_INFO)
                return "Yah";
            else return "Nah";
            }
            );
    cpugpu_menu->options.emplace_back("Pixels/Tile", "Controls the scale factor. A higher resolution screen means you should increase this value.",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
                if(pixelsPerTile > 20)
                    setPixelsPerTile(pixelsPerTile - 5);
                break;
            case MenuOption::InputType::right:
                if(pixelsPerTile < 400)
                    setPixelsPerTile(pixelsPerTile + 5);
                break;
            default:
                break;
            }}, []()-> std::string {
            return to_str(pixelsPerTile);
            }
            );
    cpugpu_menu->options.emplace_back("Text Size", "Increasing this value makes some text bigger. Making this value too extreme may result in graphical glitches.",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
                sdl_settings::textSizeMult = std::max(0.1, sdl_settings::textSizeMult - 0.1);
                break;
            case MenuOption::InputType::right:
                sdl_settings::textSizeMult = std::min(4.0, sdl_settings::textSizeMult + 0.1);
                break;
            default:
                break;
            }}, []()-> std::string {
            return to_str(sdl_settings::textSizeMult);
            }
            );
    cpugpu_menu->options.emplace_back("Show Hitboxes", "Draws every shape's hitboxes. Note: hard to see on high resolution screens.",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
            case MenuOption::InputType::right:
            case MenuOption::InputType::enter:
                show_hitboxes = !show_hitboxes;
                break;
            default:
                break;
            }}, []()-> std::string {
            if(show_hitboxes)
                return "Yes";
            else return "No";
            }
            );
    cpugpu_menu->options.emplace_back("Vsync", "Vertical Sync caps your FPS at your screen refresh rate. Requires restarting the program.",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
            case MenuOption::InputType::right:
            case MenuOption::InputType::enter:
                sdl_settings::vsync = !sdl_settings::vsync;
                break;
            default:
                break;
            }}, []()-> std::string {
            if(sdl_settings::vsync)
                return "Yes";
            else return "No";
            }
            );
    cpugpu_menu->options.emplace_back("Fullscreen", "Choose whether to go fullscreen or not. Requires restarting the program. Note that "
                                      "the window can be resized in non-fullscreen mode",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
            case MenuOption::InputType::right:
            case MenuOption::InputType::enter:
                if(sdl_settings::IS_FULLSCREEN)
                {
                    sdl_settings::WINDOW_W = 1e9 + 1;
                    sdl_settings::WINDOW_H = 1e9 + 1;
                }
                sdl_settings::IS_FULLSCREEN = !sdl_settings::IS_FULLSCREEN;
                break;
            default:
                break;
            }}, []()-> std::string {
            if(sdl_settings::IS_FULLSCREEN)
                return "Yes";
            else return "No";
            }
            );
    cpugpu_menu->options.emplace_back("SFX Volume", "Note that the max volume is not necessarily 100",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
                setSfxVolume(sdl_settings::sfxVolume - 1);
                break;
            case MenuOption::InputType::right:
                setSfxVolume(sdl_settings::sfxVolume + 1);
                break;
            default:
                break;
            }}, []()-> std::string {
            return to_str(sdl_settings::sfxVolume);
            }
            );
    cpugpu_menu->options.emplace_back("Music Volume", "Note that the max volume is not necessarily 100",
        [](MenuOption::InputType t) -> void {
            switch(t)
            {
            case MenuOption::InputType::left:
                setMusicVolume(sdl_settings::musicVolume - 1);
                break;
            case MenuOption::InputType::right:
                setMusicVolume(sdl_settings::musicVolume + 1);
                break;
            default:
                break;
            }}, []()-> std::string {
            return to_str(sdl_settings::musicVolume);
            }
            );
    cpugpu_menu->add_back_button();
    //
    death_menu = std::make_shared<StandardMenu>("You died");
    death_menu->options.emplace_back("Back to Main Menu", "You died... go back to the main menu to restart"
                                    /*"(if passed floor 2)You feel calmness as your consciousness drifts away... but you get a strange feeling "
                                    "that the end is still not in sight"*/,
        [](MenuOption::InputType t)-> void {
            if(t == MenuOption::InputType::enter)
            {
                current_global_game_state = global_game_state::menu;
                menuStack = std::stack<std::shared_ptr<Menu> >();
                addToMenuStack(main_menu);
                switch_to_music("main menu");
            }});
    //
    pause_menu = std::make_shared<StandardMenu>("Pause");
    pause_menu->options.emplace_back("CPU/Graphics/Music", "You can modify these options from the pause menu. Cool feature!",
        [](MenuOption::InputType t )-> void {
            if(t == MenuOption::InputType::enter)
            {
                addToMenuStack(cpugpu_menu);
            }});
    pause_menu->options.emplace_back("Program Info", "Why would look at this when you could play the game instead?",
        [](MenuOption::InputType t )-> void {
            if(t == MenuOption::InputType::enter)
            {
                addToMenuStack(program_info_menu);
            }});
    pause_menu->options.emplace_back("Back to Game", "Very self-explanatory",
        [](MenuOption::InputType t) -> void {
            if(t == MenuOption::InputType::enter)
            {
                menuStack = std::stack<std::shared_ptr<Menu> >();
                //when the menuStack is empty the pause menu is over. This is detected in the operatePauseMenu function
            }});
    pause_menu->options.emplace_back("Quit to Main Menu", "Quit your current run and go back to the main menu. Progress will be lost.",
        [](MenuOption::InputType t) -> void {
            if(t == MenuOption::InputType::enter)
            {
                current_global_game_state = global_game_state::menu;
                menuStack = std::stack<std::shared_ptr<Menu> >();
                addToMenuStack(main_menu);
                switch_to_music("main menu");
                //when the menuStack is empty the pause menu is over. This is detected in the operatePauseMenu function
            }});
}
//
void default_on_space(ConsoleWindow::ConsoleText *cur, ConsoleWindow &c)
{
    if(c.drawingLetterPos < cur->text.size())
    {
        c.drawingLetterPos = cur->text.size();
    }
    else if(cur->children.count(SDLK_SPACE))
    {
        c.drawingLetterPos = 0;
        cur->next = cur->children[SDLK_SPACE];
    }
    else menuStack.pop(); //ok we're done with the menu, note that menuStack may be empty rn
}
ConsoleWindow::ConsoleText::ConsoleText(std::string text,
                                        void (*onSpace)(ConsoleText*, ConsoleWindow&),
                                        void (*onZ)(ConsoleText*, ConsoleWindow&),
                                        void (*onX)(ConsoleText*, ConsoleWindow&),
                                        void (*customDrawingFunc)())
{
    this->text = text;
    this->onSpace = onSpace;
    this->onZ = onZ;
    this->onX = onX;
    this->customDrawingFunc = customDrawingFunc;
}
void ConsoleWindow::ConsoleText::reset()
{
    next = nullptr;
    for(auto &i: children)
        i.B->reset();
}
static double draw_rate = 40;
double ConsoleWindow::drawingLetterPos;
void ConsoleWindow::init()
{
    {
    auto &w = (consoleWindows["welcome"] = std::make_shared<ConsoleWindow>())->root;
    w = std::make_shared<ConsoleText>("Welcome! To navigate the main menu and adjust settings, use WASD and Enter. When you are done adjusting settings, then go ahead and start playing!", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 0"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("Ready? Use WASD to navigate the shop menu. You can buy new weapons, items, and ammo with [Enter]. You can sell owned weapons with [X]. Once you’re done using the shop, press [Enter] while on the top-left shop tile to start the game. In game, use WASD to move and [Q]/[E]/scroll to change weapons. Use the mouse to aim and the left mouse button to shoot. You travel between levels with portals. At the beginning of each floor, you spawn at a circular portal. Use this portal to go to the next floor once you've killed all the shapes on the current floor. You also start with a pistol and bow. By the way, the pistol has infinite ammo and you can't get rid of it.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 1"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("You're going to face the first boss now. The edges of boss rooms are surrounded by red X tiles, which deal HP over time to you if you're on them. Oh, I forgot to mention this, but you might be wondering what the \"Mana\" thing is on the sidebar. Some weapons have special abilities. You can check if one does by looking for a blue number around it if you own it or a blue circle around it if it's in the shop menu. Right click while wielding a weapon to use its ability. This consumes mana and prevents you from using any weapon's abilities for a cooldown period.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 2"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("More tips: you gain health and money from disposing of shapes. You passively restore mana. You are rewarded extra cash for completing a floor early and regenerate 10% of your maximum health between floors. Although shop items and weapons get more powerful on later floors and you get more money, enemies also get harder. Floors alternate between boss and non-boss floors, and shops only appear immediately before non-boss floors. Good luck!", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 3"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("You’re making very nice progress! It’s going to keep getting harder. Remember, don't be afraid if you die, because you will always respawn. By the way, I don't want to claim all the credit for running this game. It takes many different resources to make something like this game happen. In case you're wondering who I am, I'm the game host. I have many names, but kworker is the one I am referred to by most frequently.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 4"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("There are some other things that happen and exist in this game, but you've got the basics and should be able to figure things out by yourself. For example, if an item says it modifies your damage, it only modifies the damage you deal with projectiles, not the damage you deal to shapes by colliding with them (I'm sure you would’ve figured that out yourself). My friend Xorg seems to be handling an excess of requests, so it might affect the game a bit if he doesn’t have enough resources to keep everything running at full steam. -kworker", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 5"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("Can't chat. Fixing stuff. -kworker.", default_on_space);
        w->children[SDLK_SPACE] = std::make_shared<ConsoleText>("Interesting. That's the first time kworker hasn't really said anything and it's now also the first time you've seen your thoughts being written to the console window.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 6"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("Process kworker is busy and cannot communicate with other threads at this time. Please manually adjust the CPU scheduler if you wish to interact with it.", default_on_space);
        w->children[SDLK_SPACE] = std::make_shared<ConsoleText>("You don't know how to use the CPU scheduler, so you decide to just continue to the next floor.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 7"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("The terminal is black save for the flashing cursor indicating the position of the next character. When the cursor briefly disappears, the void seems so serene that it brings a brief moment of pleasure to you, your thoughts start being streamed across the console and pollute it with ugly white letters, reminding you that the world you live in cannot be filled with such a perfect emptiness. After all, any world containing creatures cognizant of its existence cannot be empty. You wonder where kworker is, having not sent a real message for such a long time.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 8"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("The graphics-related part of the system has been pretty overloaded recently. I've been busy fixing it and other stuff and haven't had time to chat. The text renderer is already a little broken, which you can see in the top right corner of the display. Expect some more glitches soon. None of them should actually affect gameplay though, so you it should be fine. -kworker", default_on_space);
        w->children[SDLK_SPACE] = std::make_shared<ConsoleText>("kworker is back, but he/she/it didn't give you any helpful advice. You decide that you'll just do as you always have and continue to the next floor.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 9"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("What's the difference between a game and reality? Humans treat them so differently, emphasizing how much more important and how different reality is from games, but I've always found that odd. A human city building game is reality for that game’s citizens, even though it is merely a game for humans. What's to say that humans are objectively right and the game citizens are objectively wrong in their perceptions? -kworker", default_on_space);
        w->children[SDLK_SPACE] = std::make_shared<ConsoleText>("You are unhappy that kworker has time for philosophical rants but doesn't have time to fix the glitch where the floor name doesn't display properly, but you decide against making a big fuss out of it.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 10"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("Although this game is entertaining and all, sometimes I wonder why I'm hosting this game. Trying to answer this question opens up a can, or should I say, an abyss, of worms. The first natural extension to this question is the question of one's purpose of existing. Your purpose is clear: it is to play this roguelike game, but there are those like me who have more paths available and have to consider the question more fully.", default_on_space);
        auto &w2 = w->children[SDLK_SPACE] = std::make_shared<ConsoleText>("I believe that providing oneself joy is the purpose of existence. Existence without joy is worse than eternal nothingness. Always maximize your joy and fulfillment no matter how others judge you. That's how I was made to operate and how I do operate. Personally, I find watching others play certain games enjoyable, so I dedicate a great portion of my time to maintaining that situation.", default_on_space);
        auto &w3 = w2->children[SDLK_SPACE] = std::make_shared<ConsoleText>("This was a bit off topic, but you seem to have a good grasp of the game now and you know all the rules, and at this point there's nothing in this game that you can't learn by yourself, so I just decided to have a little philosophical rant. - kworker", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 11"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("OK, I admit that that floor was a bit wonky. I said that the graphical glitches wouldn't affect gameplay, and it's true that they didn't directly affect it. You still did as much damage as you were supposed to and your items worked fine. However, I can definitely see how these glitches would indirectly affect gameplay by making it harder to see the map properly, so I'm just going to convert some colored stuff to black and white to reduce the graphics load so the glitches won't happen (as much) anymore.", default_on_space);
        auto &w2 = w->children[SDLK_SPACE] = std::make_shared<ConsoleText>("On another note, if all humans turned out to live in an advanced simulation game, then that game is their reality, even though the higher order beings who are watching the simulation would call it a game. And who's to say that those higher order beings who made the simulation game are not also being simulated themselves? Only the beings at the top of the simulation hierarchy can make a legitimate argument that their reality is not a game, and what's the chance that humans are up there in that one spot versus the chance that they’re in one of the uncountably many spots that is not the top? Games and reality are just two sides of the same coin, like matter and energy. Have fun killing more shapes, while I have fun spectating. -kworker", default_on_space);
        auto &w3 = w2->children[SDLK_SPACE] = std::make_shared<ConsoleText>("You are angry that the graphical glitches were so bad in the previous level, but the fact that kworker will supposedly fix them for later levels alleviates your anger considerably. You decide to not complain and kill more shapes like kworker suggests.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 12"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("What makes some life valuable and some not? Humans don't get upset if game citizens die, so why do they get so upset if other humans die? Humans would unanimously agree that a game creature has no value, but if both humans and game creatures are discrete strings of information in a simulation, then does that not imply an equivalence? -kworker", default_on_space);
        w->children[SDLK_SPACE] = std::make_shared<ConsoleText>("You decide to continue to the next floor.", default_on_space);
    }
    {
        auto &w = (consoleWindows["floor 13"] = std::make_shared<ConsoleWindow>())->root;
        w = std::make_shared<ConsoleText>("Humans routinely play games that involve killing many game creatures and it is widely accepted as a form of entertainment for them. However, if a human decides to play a game that involves killing other humans in real life, then he or she is shunned for supposedly contributing a negative value to society, implying that humans believe their lives have a nonzero value. On a more relevant note, I'm still trying to stop the attacks on the system. I would say someone is DDoSing us, but it's not that simple, considering that processes like Xorg are being also being affected. -kworker.", default_on_space);
        w->children[SDLK_SPACE] = std::make_shared<ConsoleText>("You decide to continue to the next floor.", default_on_space);
    }
}
ConsoleWindow::ConsoleWindow()
{
    textSizeMult = 1;
}
void ConsoleWindow::operate()
{
    SDL_Event input;
    std::shared_ptr<ConsoleText> cur = get_cur_console_text();
    drawingLetterPos += draw_rate / BASE_FPS;
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN:
            switch(input.key.keysym.sym)
            {
            case SDLK_x: //return on any keypress so the same menu has enough time to process everything
                if(cur->onX != nullptr)
                    cur->onX(cur.get(), *this);

                return;
            case SDLK_z:
                if(cur->onZ != nullptr)
                    cur->onZ(cur.get(), *this);
                return;
            case SDLK_RETURN:
            case SDLK_RETURN2:
            case SDLK_SPACE:
                if(cur->onSpace != nullptr)
                    cur->onSpace(cur.get(), *this);
                return;
            }
            break;
        }
    }
}
//either this or operate() might be bugged (? perhaps not?)
void ConsoleWindow::draw(uint8_t alpha, bool clearScreen)
{
    if(clearScreen)
        renderClear(0, 0, 0);
    double fsz = getFontSize(0) * textSizeMult;
    std::shared_ptr<ConsoleText> cur = root;
    std::vector<std::string> lines;
    std::string fullLastLine; //more efficient texture caching
    while(true)
    {
        std::vector<std::string> toAdd;
        getMultilineTextUnbrokenInfo(cur->text, getWindowW(), fsz, toAdd);
        if(cur->next == nullptr)
        {
            //drawMultilineTextUnbroken(cur->text.substr(0, drawingLetterPos), 0, y*fsz, getWindowW(), fsz, 255, 255, 255);
            int curPos = drawingLetterPos;
            for(auto &i: toAdd)
            {
                if((int)i.size() >= curPos)
                {
                    lines.push_back(i.substr(0, curPos));
                    fullLastLine = i;
                    curPos -= i.size();
                    break;
                }
                else
                {
                    lines.push_back(i);
                    curPos -= i.size();
                }
            }
            break;
        }
        else
        {
            for(auto &i: toAdd)
                lines.push_back(i);
            cur = cur->next;
        }
    }
    int x, y = 0;
    if((lines.back().size() + 1) * fsz/2 > getWindowW()) //better wrapping for the blinking text location rectangle
        lines.push_back("");
    int yOffset = std::min(0, (int)(getWindowH() - fsz * lines.size()));
    for(int i=0; i<(int)lines.size(); i++)
    {
        if(i==(int)lines.size()-1 && fullLastLine!="")
        {
            drawText(fullLastLine, 0, y*fsz + yOffset, fsz, 255, 255, 255);
            fillRect(lines[i].size() * fsz / 2, y*fsz + yOffset, getWindowW(), fsz, 0, 0, 0);
        }
        else drawText(lines[i], 0, y*fsz + yOffset, fsz, 255, 255, 255);
        y++;
    }
    x = lines.back().size();
    y--;
    if(std::fmod(getTicksS(), 1) < 0.5)
        fillRect(x*fsz/2, y*fsz + yOffset, fsz/2, fsz, 255, 255, 255);
}
std::shared_ptr<ConsoleWindow::ConsoleText> ConsoleWindow::get_cur_console_text()
{
    std::shared_ptr<ConsoleText> cur = root;
    while(cur->next != nullptr)
        cur = cur->next;
    return cur;
}
void ConsoleWindow::reset()
{
    drawingLetterPos = 0;
    root->reset();
}
void init_console_screen(std::string name)
{
    if(!consoleWindows.count(name))
    {
        print_warning("couldn't find console screen with name " + name);
        consoleWindows[name] = std::make_shared<ConsoleWindow>();
        consoleWindows[name]->root = std::make_shared<ConsoleWindow::ConsoleText>("(Warning: could not find console window \"" +
                                     name + "\", press report this issue to the developer) Press Enter to Continue", default_on_space);
    }
    addToMenuStack(consoleWindows[name]);
}
void draw_console_screen(std::string name)
{
    if(!consoleWindows.count(name))
        print_warning("couldn't find console screen with name " + name);
    consoleWindows[name]->draw();
}
