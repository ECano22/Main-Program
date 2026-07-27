#include <ftxui/ftxui.hpp>
#include <array>
#include "screens.h"
#include "classes.h"
#include "global.h"
using namespace ftxui;

int main()
{
    // -- initializing party members
    std::array<PartyChar, MAX_PARTY_MEMBERS> party_members;
    int current_member_index = 0;

    // -- Initializing screens
    auto screen = ScreenInteractive::Fullscreen();
    int current_screen = 0;
    MainMenu main_menu;
    QuitConfirm quit_confirm(screen.ExitLoopClosure());
    CharacterCreator character_creator;
    AdvCharacterCreator adv_character_creator;
    ReadyScreen ready_screen;

    // -- Setting up the router for the screens
    auto screen_router = Container::Tab({
        main_menu.MakeScreen(current_screen),
        quit_confirm.MakeScreen(current_screen),
        character_creator.MakeScreen(current_screen, party_members[current_member_index]),
        adv_character_creator.MakeScreen(current_screen, party_members[current_member_index]),
        ready_screen.MakeScreen(current_screen, party_members, current_member_index, character_creator)
        }, &current_screen);

    screen.Loop(screen_router);

    return 0;
}

