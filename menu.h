#pragma once
#include "basic_game_funcs.h"
#include <string>
#include <vector>
#include <stack>
#include <memory>
struct MenuOption
{
    enum class InputType{left, right, enter};
    void (*funcOnPress)(InputType);
    std::string (*displayVar)();
    std::string text, description;
    MenuOption(std::string text, std::string description, void (*funcOnPress)(InputType) = nullptr, std::string (*displayVar)() = nullptr);
};
class GameState;
class Menu
{
protected:
    Menu() = default;
public:
    static GameState *game_state;
    virtual void reset() = 0;
    virtual void operate() = 0;
    virtual void draw(uint8_t alpha = 255, bool clearScreen = true) = 0;
};
class StandardMenu: public Menu
{
    double textSizeMult, widthF;
    std::string title;
    std::vector<MenuOption> options;
    void (*customDrawingFunc)(int y);
    void add_back_button();
public:
    int curOption;
    StandardMenu(std::string title);
    static void init();
    void operate() override;
    void draw(uint8_t alpha = 255, bool clearScreen = true) override;
    void reset() override;
};
struct ConsoleWindow: public Menu
{
    static double drawingLetterPos;
    double textSizeMult;
    struct ConsoleText
    {
        fast_map<SDL_Keycode, std::shared_ptr<ConsoleText> > children;
        std::shared_ptr<ConsoleText> next;
        std::string text;
        void reset();
        void (*customDrawingFunc)();
        void (*onSpace)(ConsoleText*, ConsoleWindow&);
        void (*onZ)(ConsoleText*, ConsoleWindow&);
        void (*onX)(ConsoleText*, ConsoleWindow&);
        ConsoleText(std::string text,
                    void (*onSpace)(ConsoleText*, ConsoleWindow&),
                    void (*onZ)(ConsoleText*, ConsoleWindow&) = nullptr,
                    void (*onX)(ConsoleText*, ConsoleWindow&) = nullptr,
                    void (*customDrawingFunc)() = nullptr);
    };
    std::shared_ptr<ConsoleText> root;
    ConsoleWindow();
    static void init();
    std::shared_ptr<ConsoleText> get_cur_console_text();
    void operate() override;
    void draw(uint8_t alpha = 255, bool clearScreen = true) override;
    void reset() override;
};
void default_on_space(ConsoleWindow::ConsoleText *cur, ConsoleWindow &c);
void init_console_screen(std::string name);
void draw_console_screen(std::string name);
void addToMenuStack(std::shared_ptr<Menu> menu);
extern std::stack<std::shared_ptr<Menu> > menuStack;
extern std::shared_ptr<StandardMenu> main_menu, death_menu, pause_menu;
