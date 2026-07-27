#include <ftxui/ftxui.hpp>
#include <iostream>
#include <array>
#include <functional>
#include "screens.h"
#include "classes.h"
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

Component CharacterCreator::MakeScreen(int& current_screen, PartyChar& party_member)
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
    // -- retrieving stats from classes.h
    auto stats = Renderer([&] {
        class_stats.clear();
        const std::vector<std::string> class_prefix = { "HP: ", "SP: ", "Attack: ", "Defense: ", "Speed: " };
        const CharClass class_stats_retrieved = GetClass(char_class);
        const std::vector<std::string> class_stats_truncated = { std::to_string(class_stats_retrieved.HP),
            std::to_string(class_stats_retrieved.SP),
            std::to_string(class_stats_retrieved.atk),
            std::to_string(class_stats_retrieved.def),
            std::to_string(class_stats_retrieved.spd) };
        for (int x = 0; x < 5; x++)
        {
            std::string combined = class_prefix[x] + class_stats_truncated[x];
            class_stats.push_back(combined);

        }
        return vbox({
            text(class_stats[0]),
            text(class_stats[1]),
            text(class_stats[2]),
            text(class_stats[3]),
            text(class_stats[4]),
            });
        });
    auto cnd_stats = Maybe(stats, [this] { return section == 1; });
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
    auto cnd_confirm_hotkeys = CatchEvent(cnd_confirm, [this, cnd_menu, &party_member, &current_screen](Event event)
        {
            if (event == Event::Character('z'))
            {
                const CharClass class_stats_retrieved = GetClass(char_class);
                party_member.HP = class_stats_retrieved.HP;
                party_member.SP = class_stats_retrieved.SP;
                party_member.atk = class_stats_retrieved.atk;
                party_member.def = class_stats_retrieved.def;
                party_member.spd = class_stats_retrieved.spd;
                if (selection == 0)
                {
                    current_screen = 3;
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
                filler(),
                cnd_stats->Render(),
            }) | border | size(WIDTH, EQUAL, 40)
        });
    });
    return render;
}

Component AdvCharacterCreator::MakeScreen(int& current_screen, PartyChar& party_member)
{
    //static text
    auto confirm_text = Renderer([&] {
    return vbox({
        text(std::format("press 'z' when you are finished.", stat_pool))
                });
        });
    //dynamic text
    auto pool_text = Renderer([&] {
        return vbox({
            text(std::format("Curent Pool: {}", stat_pool))
            });
        });
    auto stats_value = Renderer([&party_member]
        {
            return vbox({
                text("HP"),
                text(std::to_string(party_member.HP)),
                text("SP"),
                text(std::to_string(party_member.SP)),
                text("Attack"),
                text(std::to_string(party_member.atk)),
                text("Defense"),
                text(std::to_string(party_member.def)),
                text("Speed"),
                text(std::to_string(party_member.spd)),
                });
        });
    // -- configuring selections
    MenuOption selection = MenuOption::Vertical();
    selection.entries_option.transform = [](const EntryState& state) {
        Element e = text(state.label);

        // -- adding back the highlights
        if (state.active) {
            e = e | inverted;
        }
        if (state.focused) {
            e = e | bold;
        }

        // -- adding spacers so selection lines up with the numbers
        return vbox({
            e,
            spacer(1)
            });
        };
    std::array<std::reference_wrapper<int>, 5> stat_array = { std::ref(party_member.HP),
        std::ref(party_member.SP),
        std::ref(party_member.atk),
        std::ref(party_member.def),
        std::ref(party_member.spd) };
    auto left_menu = Menu(&left_entries, &stat_modified, selection);
    auto left_menu_hotkeys = CatchEvent(left_menu, [this, &current_screen, stat_array](Event event)
        {
            if (event == Event::Character('z'))
            {
                //current_screen = 4;
                return true;
            }
            if (event == Event::Character('x'))
            {
                current_screen = 2;
                return true;
            }
            if (event == Event::ArrowLeft)
            {
                if (stat_array[stat_modified].get() > 0)
                {
                    stat_array[stat_modified].get()--;
                    stat_pool++;
                }
                return true;
            }
            if (event == Event::ArrowRight)
            {
                if (stat_pool > 0)
                {
                    stat_pool--;
                    stat_array[stat_modified].get()++;
                }
                return true;
            }
            return false;
        });
    auto right_menu = Menu(&right_entries, &stat_modified, selection);
    // -- layout & rendering
    auto layout = Container::Horizontal({
        left_menu_hotkeys,
        });
    auto render = Renderer(layout, [=] {
        return hbox({
            vbox({
                spacer(2),
                confirm_text->Render(),
                spacer(1),
                pool_text->Render(),
                spacer(1),
                hbox({
                    left_menu->Render(),
                    stats_value->Render() | center,
                    right_menu->Render(),
                }) | center

            }) | flex,
            vbox({
                spacer(2)
            }) | border | size(WIDTH, EQUAL, 40)
            });
        });
    return render;
}