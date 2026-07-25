#include <ftxui/ftxui.hpp>
#include "screens.h"
using namespace ftxui;

int main()
{
    // -- Initializing screens
    auto screen = ScreenInteractive::Fullscreen();
    int current_screen = 0;
    MainMenu main_menu;
    QuitConfirm quit_confirm(screen.ExitLoopClosure());
    CharacterCreator character_creator;

    // -- Setting up the router for the screens
    auto screen_router = Container::Tab({
        main_menu.MakeScreen(current_screen),
        quit_confirm.MakeScreen(current_screen),
        character_creator.MakeScreen(current_screen),
        }, &current_screen);

    screen.Loop(screen_router);

    return 0;
}

