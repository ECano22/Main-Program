#include <ftxui/ftxui.hpp>
#include <iostream>
#include "screens.h"
using namespace ftxui;

Element spacer(int lines)
{
    return text("") | size(HEIGHT, EQUAL, lines);
}

Component MainMenu::MakeScreen(int& current_screen)
{
    auto title = Renderer([] {
        return vbox({
            text("Turn-Based Game")
            });
        });
    auto menu = Menu(&entries, &selection) | border;
    // -- input handling
    auto menu_hotkeys = CatchEvent(menu, [&](Event event)
        {
            if (event == Event::Character('z'))
            {
                if (selection == 0)
                {
                    current_screen = 2;
                }
                else if (selection == 1)
                {
                    current_screen = 1;
                }
                return true;
            }
            return false;
        });
    // -- Layout
    auto layout = Container::Vertical({
        menu_hotkeys,
        });

    // -- Rendering
    auto main_menu = Renderer(layout, [=] {
        return vbox({
            spacer(2),
            title->Render() | center,
            filler(),
            menu->Render() | size(WIDTH, EQUAL, 30),
            spacer(3),
            });
        });

    return main_menu;
}

QuitConfirm::QuitConfirm(std::function<void()> quit_function)
{
    on_quit = quit_function;
}
Component QuitConfirm::MakeScreen(int& current_screen)
{
    // -- text
    auto title = Renderer([] {
        return vbox({
            text("Are you sure?")
            });
        });

    // -- setting up menu
    auto menu = Menu(&entries, &selection, MenuOption::Horizontal());
    auto menu_hotkeys = CatchEvent(menu, [&](Event event)
        {
            if (event == Event::Character('z'))
            {
                if (selection == 0)
                {
                    on_quit();
                }
                if (selection == 1)
                {
                    current_screen = 0;
                }
                return true;
            }
            return false;
        });
    // -- Layout
    auto layout = Container::Horizontal({
        menu_hotkeys,
        });
    // -- Renderer
    auto renderer = Renderer(layout, [=] {
        return vbox({
            filler(),
            title->Render() | center,
            spacer(1),
            menu->Render() | center,
            filler(),
            });
        });
    return renderer;
}

Component CharacterCreator::MakeScreen(int& current_screen)
{
    // -- static text
    auto name_text = Renderer([] {
        return vbox({
            text("What is this character's name? Enter to confirm.")
            });
        });
    auto cnd_name_text = Maybe(name_text, [this] { return section == 0; });
    Component input_name = Input(&name, "Name...");
    auto cnd_input_name = Maybe(input_name, [this] { return section == 0; });
    auto class_text = Renderer([] {
        return vbox({
            text("What is this charater's class?")
            });
        });
    auto cnd_class_text = Maybe(class_text, [this] { return section == 1; });
    auto adv_text = Renderer([] {
        return vbox({
            text("Configure stats? This may take longer.")
            });
        });
    auto cnd_adv_text = Maybe(adv_text, [this] { return section == 2; });
    // -- class descriptions
    auto hkn_desc = Renderer([] {
        return vbox ({
            paragraph("The heavy knight is the protecter of the group. While they are slow to move, they can take many hits and provoke enemies.")
            });
        });
    auto cnd_hkn_desc = Maybe(hkn_desc, [this] { return char_class == 0 && section == 1; });
    auto mrc_desc = Renderer([]{
        return vbox({
            paragraph("The mercenary has all-around balanced stats, but favors doing damage more than anything.")
            });
        });
    auto cnd_mrc_desc = Maybe(mrc_desc, [this] { return char_class == 1 && section == 1; });
    auto clr_desc = Renderer([] {
        return vbox({
            paragraph("The cleric cannot deal great physical damage, but has skills to heal and a high mana pool.")
            });
        });
    auto cnd_clr_desc = Maybe(clr_desc, [this] { return char_class == 2 && section == 1; });
    // -- input & menu handling
    auto menu = Menu(&class_entries, &char_class);
    auto cnd_menu = Maybe(menu, [this] { return section == 1; });
    auto confirm = Menu(&confirm_entries, &selection);
    auto cnd_confirm = Maybe(confirm, [this] { return section == 2; });
    auto cnd_input_name_hotkeys = CatchEvent(cnd_input_name, [this, cnd_menu](Event event)
        {
            if (event == Event::Return)
            {
                section = 1;
                cnd_menu->TakeFocus();
                return true;
            }
            return false;
        });
    auto cnd_menu_hotkeys = CatchEvent(cnd_menu, [this, cnd_confirm, cnd_input_name](Event event)
        {
            if (event == Event::Character('z'))
            {
                section = 2;
                cnd_confirm->TakeFocus();
                return true;
            }
            if (event == Event::Character('x'))
            {
                section = 0;
                cnd_input_name->TakeFocus();
                return true;
            }
            return false;
        });
    auto cnd_confirm_hotkeys = CatchEvent(cnd_confirm, [this, cnd_menu](Event event)
        {
            if (event == Event::Character('z'))
            {
                if (selection == 0)
                {
                    //current_screen = 3;
                }
                if (selection == 1)
                {
                    //current_screen = 4;
                }
            }
            if (event == Event::Character('x'))
            {
                section = 1;
                return true;
            }
            return false;
        });
    // -- layout & rendering
    auto layout = Container::Vertical({
        cnd_input_name_hotkeys,
        cnd_menu_hotkeys,
        cnd_confirm_hotkeys
        });
    auto render = Renderer(layout, [=] {
        return hbox({
            vbox({
                spacer(2),
                cnd_name_text->Render(),
                cnd_class_text->Render(),
                cnd_adv_text->Render(),
                spacer(1),
                cnd_input_name->Render(),
                cnd_menu->Render(),
                cnd_confirm->Render(),

            }) | flex,
            vbox({
                cnd_hkn_desc->Render(),
                cnd_mrc_desc->Render(),
                cnd_clr_desc->Render(),
            }) | border | size(WIDTH, EQUAL, 40)
        });
    });
    return render;
}