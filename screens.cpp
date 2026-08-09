#include <ftxui/ftxui.hpp>
#include <iostream>
#include <array>
#include <functional>
#include "screens.h"
#include "classes.h"
#include "global.h"
#include "zmq_classes.h"
using namespace ftxui;

// -- functions useful for this file

Element spacer(int lines)
{
    return text("") | size(HEIGHT, EQUAL, lines);
}

void InsertMember(std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, PartyChar new_member)
{
    
    for (int x = MAX_PARTY_MEMBERS-2; x >= 0; x--)
    {
        party_members[x + 1] = party_members[x];
    }
    party_members[0] = new_member;
}

void ClearMembers(std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, int& member_count)
{
    for (auto& member : party_members)
    {
        member.is_used = 0;
    }
    member_count = 0;
}

void DeleteMember(std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, int& member_count, int index)
{
    for (int x = index; x < MAX_PARTY_MEMBERS - 1; x++)
    {
        party_members[x] = party_members[x + 1];
    }
    party_members[MAX_PARTY_MEMBERS - 1].is_used = false;
    member_count--;
}

// -- defining .h 

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

Component CharacterCreator::MakeScreen(int& current_screen, PartyChar& party_member, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, int& member_count, ReadyScreen& ready_screen, ZMQConnection& rng_connection)
{
    // -- static text
    auto return_text = Renderer([] {
        return vbox({
            text("press ESC to go back to the main menu (all changes will be lost)")
            });
        });
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
            text("What is this character's class?")
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
    auto cnd_input_name_hotkeys = CatchEvent(cnd_input_name, [this, cnd_menu, &rng_connection](Event event)
        {
            if (event == Event::Return)
            {
                if (name == "")
                {
                    name = rng_connection.send_string("RandomName, 8");
                }
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
    auto cnd_confirm_hotkeys = CatchEvent(cnd_confirm, [this, cnd_menu, &party_member, &current_screen, &ready_screen, cnd_input_name](Event event)
        {
            if (event == Event::Character('z'))
            {
                const CharClass class_stats_retrieved = GetClass(char_class);
                party_member.name = name;
                party_member.HP = class_stats_retrieved.HP;
                party_member.MaxHP = class_stats_retrieved.HP;
                party_member.SP = class_stats_retrieved.SP;
                party_member.MaxSP = class_stats_retrieved.SP;
                party_member.atk = class_stats_retrieved.atk;
                party_member.def = class_stats_retrieved.def;
                party_member.spd = class_stats_retrieved.spd;
                party_member.is_used = 1;
                ready_screen.ClearData();
                
                if (selection == 0)
                {
                    ClearData();
                    current_screen = 3;
                }
                if (selection == 1)
                {
                    ClearData();
                    current_screen = 4;
                    
                }
            }
            if (event == Event::Character('x'))
            {
                section = 1;
                cnd_menu->TakeFocus();

                return true;
            }
            return false;
        });
    // -- layout & rendering
    auto layout = Container::Tab({
        cnd_input_name_hotkeys,
        cnd_menu_hotkeys,
        cnd_confirm_hotkeys
        }, &section);
    auto global_hotkeys = CatchEvent(layout, [this, &party_members, &member_count, &current_screen](Event event)
        {
            if (event == Event::Escape)
            {
                ClearMembers(party_members, member_count);
                ClearData();
                current_screen = 0;
                return true;
            }
            return false;
        });
    auto render = Renderer(global_hotkeys, [=] {
        return hbox({
            vbox({
                return_text->Render(),
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

void CharacterCreator::ClearData()
{
    name = "";
    section = 0;
}

Component AdvCharacterCreator::MakeScreen(int& current_screen, PartyChar& party_member, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, int& member_count)
{
    //static text
    auto hp_desc = Renderer([] {
        return vbox({
            paragraph("HP determines how much damage a party member can take before they die.")
            });
        });
    auto cnd_hp_desc = Maybe(hp_desc, [this] { return stat_modified == 0; });
    auto sp_desc = Renderer([] {
        return vbox({
            paragraph("SP determines how many skills they can use.")
            });
        });
    auto cnd_sp_desc = Maybe(sp_desc, [this] { return stat_modified == 1; });
    auto atk_desc = Renderer([] {
        return vbox({
            paragraph("Attack determines how much damage an attack deals.")
            });
        });
    auto cnd_atk_desc = Maybe(atk_desc, [this] { return stat_modified == 2; });
    auto def_desc = Renderer([] {
        return vbox({
            paragraph("Defense determines how much damage is negated.")
            });
        });
    auto cnd_def_desc = Maybe(def_desc, [this] { return stat_modified == 3; });
    auto spd_desc = Renderer([] {
        return vbox({
            paragraph("Speed determines the turn order of battles.")
            });
        });
    auto cnd_spd_desc = Maybe(spd_desc, [this] { return stat_modified == 4; });
    auto return_text = Renderer([] {
        return vbox({
            text("press ESC to go back to the main menu (all changes will be lost)")
            });
        });
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
                text("HP") | hcenter,
                text(std::to_string(party_member.MaxHP)) | hcenter,
                text("SP") | hcenter,
                text(std::to_string(party_member.MaxSP)) | hcenter,
                text("Attack") | hcenter,
                text(std::to_string(party_member.atk)) | hcenter,
                text("Defense") | hcenter,
                text(std::to_string(party_member.def)) | hcenter,
                text("Speed") | hcenter,
                text(std::to_string(party_member.spd)) | hcenter,
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
    std::array<std::reference_wrapper<int>, 5> stat_array = { std::ref(party_member.MaxHP),
        std::ref(party_member.MaxSP),
        std::ref(party_member.atk),
        std::ref(party_member.def),
        std::ref(party_member.spd) };
    auto left_menu = Menu(&left_entries, &stat_modified, selection);
    auto left_menu_hotkeys = CatchEvent(left_menu, [this, &current_screen, &party_member, stat_array](Event event)
        {
            if (event == Event::Character('z'))
            {
                party_member.HP = party_member.MaxHP;
                party_member.SP = party_member.MaxSP;
                current_screen = 4;
                return true;
            }
            if (event == Event::Character('x'))
            {
                current_screen = 2;
                return true;
            }
            if (event == Event::ArrowLeft)
            {
                if (stat_array[stat_modified].get() > 1)
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
    auto global_hotkeys = CatchEvent(layout, [this, &party_members, &member_count, &current_screen](Event event)
        {
            if (event == Event::Escape)
            {
                ClearMembers(party_members, member_count);
                current_screen = 0;
                return true;
            }
            return false;
        });
    auto render = Renderer(global_hotkeys, [=] {
        return hbox({
            vbox({
                return_text->Render(),
                spacer(2),
                confirm_text->Render(),
                spacer(1),
                pool_text->Render(),
                spacer(1),
                hbox({
                    left_menu->Render(),
                    stats_value->Render() | center,
                    right_menu->Render(),
                }) | flex | center

            }) | flex,
            vbox({
                spacer(2),
                cnd_hp_desc->Render(),
                cnd_sp_desc->Render(),
                cnd_atk_desc->Render(),
                cnd_def_desc->Render(),
                cnd_spd_desc->Render(),

            }) | border | size(WIDTH, EQUAL, 40)
            });
        });
    return render;
}

Component ReadyScreen::MakeScreen(int& current_screen, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, int& member_count, CharacterCreator& character_creator)
{
    // -- static text
    auto return_text = Renderer([] {
        return vbox({
            text("press ESC to go back to the main menu (all changes will be lost)")
            });
        });
    auto ready_text = Renderer([] {
        return vbox({
            text("Does this look good?"),
            });
        });

    // -- menus
    auto menu = Menu(&entries, &selection);
    auto menu_hotkeys = CatchEvent(menu, [this, &current_screen, &member_count, &character_creator, &party_members](Event event)
        {
            if (event == Event::Character('z'))
            {
                if (!is_selecting_member)
                {
                    if (selection == 0 && member_count < MAX_PARTY_MEMBERS - 1)
                    {
                        PartyChar new_member;
                        InsertMember(party_members, new_member);
                        member_count++;
                        current_screen = 2;
                        return true;
                    }
                    if (selection == 1 || (selection == 2 && member_count > 0))
                    {
                        is_selecting_member = 1;
                        GetNames(member_list, party_members);
                        return true;
                    }
                    if (selection == 3)
                    {
                        current_screen = 5;
                    }
                }
            }
            return false;
        });

    auto member_menu = Menu(&member_list, &member_selection);
    auto member_menu_hotkeys = CatchEvent(member_menu, [this, &member_count, &party_members, &character_creator, &current_screen](Event event)
        {
            if (event == Event::Character('x'))
            {
                is_selecting_member = 0;
                return true;
            }
            if (event == Event::Character('z'))
            {
                if (selection == 1)
                {
                    PartyChar tempMember = party_members[member_selection];
                    party_members[member_selection] = party_members[0];
                    party_members[0] = tempMember;
                    character_creator.name = party_members[0].name;
                    current_screen = 2;
                    return true;
                }
                if (selection == 2 && member_count > 0)
                {
                    DeleteMember(party_members, member_count, member_selection);
                    is_selecting_member = 0;
                    return true;
                }
            }
            return false;
        });

    // -- Layout and Rendering
    auto layout = Container::Tab({
        menu_hotkeys,
        member_menu_hotkeys
        }, &is_selecting_member);
    auto global_hotkeys = CatchEvent(layout, [this, &party_members, &member_count, &current_screen](Event event)
        {
            if (event == Event::Escape)
            {
                ClearMembers(party_members, member_count);
                current_screen = 0;
                return true;
            }
            return false;
        });
    auto render = Renderer(global_hotkeys, [=, &party_members] {
        std::vector<Element> member_cards;
        int current_index = -1;

        for (const auto& member : party_members)
        {
            current_index++;
            if (!member.is_used) continue;
            float hp_ratio = static_cast<float>(member.HP) / static_cast<float>(member.MaxHP);
            float sp_ratio = static_cast<float>(member.SP) / static_cast<float>(member.MaxSP);
            auto card = hbox({
                vbox({
                    text(member.name),
                    text(std::format("HP: {}/{}", member.HP, member.MaxHP)),
                    gauge(hp_ratio) | color(Color::Green) | size(WIDTH, EQUAL, 8),
                    text(std::format("SP: {}/{}", member.SP, member.MaxSP)),
                    gauge(sp_ratio) | color(Color::Blue) | size(WIDTH, EQUAL, 8),
                    }),
                spacer(2),
                vbox({
                    text(std::format("Atk: {}", member.atk)),
                    text(std::format("Def: {}", member.def)),
                    text(std::format("Spd: {}", member.def)),
                    })
                }) | border;
            member_cards.push_back(card);
        }

        return vbox({
            return_text->Render(),
            spacer(2),
            ready_text->Render() | center,
            layout->Render() | center,
            filler(),
            hbox(member_cards) | center,
            spacer(2),
            });
        });
    return render;
}


void ReadyScreen::ClearData()
{
    is_selecting_member = 0;
}

Component BattleScreen::MakeScreen(int& current_screen, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members,std::array<EnemyChar, MAX_PARTY_MEMBERS>& enemy_members)
{
    //populating enemies
    GetEnemies(enemy_members);
    TurnOrder(turn_order, party_members, enemy_members);
    auto choice_menu = Menu(&choice_list, &selection, MenuOption::Horizontal());
    auto choice_menu_hotkeys = CatchEvent(choice_menu, [this, &enemy_members](Event event)
        {
            if (event == Event::Character('z'))
            {
                //selected attack
                if (selection == 0)
                {
                    GetNames(enemy_list, enemy_members);
                    section = 1;
                }
                //selected skills
                else if (selection == 1)
                {
                    // need to implement basic combat before tackling skills
                    // GetSkills(skill_list, )
                    section = 2;
                }
                return true;
            }
            return false;
        });
    auto enemy_menu = Menu(&enemy_list, &enemy_selection, MenuOption::Horizontal());
    auto enemy_menu_hotkeys = CatchEvent(enemy_menu, [this](Event event)
        {
            if (event == Event::Character('z'))
            {
                //attack enemy at index
                return true;
            }
            else if (event == Event::Character('x'))
            {
                section = 0;
                return true;
            }
            return false;
        });
    auto skill_menu = Menu(&skill_list, &skill_selection, MenuOption::Horizontal());
    auto skill_menu_hotkeys = CatchEvent(skill_menu, [this](Event event)
        {
            if (event == Event::Character('z'))
            {
                //set attack to skill
                //bring up ally/enemy list depending on skill type
                return true;
            }
            else if (event == Event::Character('x'))
            {
                section = 0;
                return true;
            }
            return false;
        });
    auto layout = Container::Tab({
        choice_menu_hotkeys,
        enemy_menu_hotkeys,
        //skill_hotkeys,
        //party_menu_hotkeys,
        //enemy_attack
        }, &section);
    auto render = Renderer(layout, [=, &party_members, &enemy_members] {
        std::vector<Element> member_cards;

        for (const auto& member : party_members)
        {
            if (!member.is_used) continue;
            float hp_ratio = static_cast<float>(member.HP) / static_cast<float>(member.MaxHP);
            float sp_ratio = static_cast<float>(member.SP) / static_cast<float>(member.MaxSP);
            auto card = hbox({
                vbox({
                    text(member.name),
                    text(std::format("HP: {}/{}", member.HP, member.MaxHP)),
                    gauge(hp_ratio) | color(Color::Green) | size(WIDTH, EQUAL, 8),
                    text(std::format("SP: {}/{}", member.SP, member.MaxSP)),
                    gauge(sp_ratio) | color(Color::Blue) | size(WIDTH, EQUAL, 8),
                    }),
                spacer(2),
                vbox({
                    text(std::format("Atk: {}", member.atk)),
                    text(std::format("Def: {}", member.def)),
                    text(std::format("Spd: {}", member.def)),
                    })
                }) | border;
            member_cards.push_back(card);
        }
        std::vector<Element> enemy_cards;
        for (const auto& member : enemy_members)
        {
            // if no HP, the enemy is dead
            if (!member.HP) continue;
            float hp_ratio = static_cast<float>(member.HP) / static_cast<float>(member.MaxHP);
            auto card = hbox({
                vbox({
                    text(member.name),
                    text(std::format("HP: {}/{}", member.HP, member.MaxHP)),
                    gauge(hp_ratio) | color(Color::Green) | size(WIDTH, EQUAL, 8),
                    }),
                spacer(2),
                vbox({
                    text(std::format("Atk: {}", member.atk)),
                    text(std::format("Def: {}", member.def)),
                    text(std::format("Spd: {}", member.def)),
                    })
                }) | border;
            enemy_cards.push_back(card);
        }
        return vbox({
            spacer(2),
            hbox(enemy_cards) | center,
            filler(),
            layout->Render() | center,
            spacer(2),
            hbox(member_cards) | center,
            spacer(2),
            });
        });
    return render;
}