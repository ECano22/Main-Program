#include <ftxui/ftxui.hpp>
#include <array>
#include <zmq.hpp>
#include "screens.h"
#include "classes.h"
#include "global.h"
#include "zmq_classes.h"
using namespace ftxui;

int main()
{
    // -- initializing microservices
    zmq::context_t context(1);
    // random name generator socket
    zmq::socket_t rng_socket(context, zmq::socket_type::req);
    zmq::socket_t weighted_socket(context, zmq::socket_type::req);
    zmq::socket_t exchange_socket(context, zmq::socket_type::req);
    zmq::socket_t score_socket(context, zmq::socket_type::req);
    ZMQConnection rng_connection(context, rng_socket, "tcp://localhost:5555");
    ZMQConnection weighted_connection(context, weighted_socket, "tcp://localhost:39102");
    ZMQConnection exchange_connection(context, exchange_socket, "tcp://localhost:49190");
    ZMQConnection score_connection(context, score_socket, "tcp://localhost:52222");


    // -- initializing party members
    std::array<PartyChar, MAX_PARTY_MEMBERS> party_members;
    int member_count = 0;
    std::array<EnemyChar, MAX_PARTY_MEMBERS> enemy_members;


    // -- Initializing screens
    auto screen = ScreenInteractive::Fullscreen();
    int current_screen = 0;
    MainMenu main_menu;
    QuitConfirm quit_confirm(screen.ExitLoopClosure());
    CharacterCreator character_creator;
    AdvCharacterCreator adv_character_creator;
    ReadyScreen ready_screen;
    BattleScreen battle_screen;

    // -- Setting up the router for the screens
    auto screen_router = Container::Tab({
        main_menu.MakeScreen(current_screen),
        quit_confirm.MakeScreen(current_screen),
        //the new party members will be added to the first slot
        character_creator.MakeScreen(current_screen, party_members[0], party_members, member_count, ready_screen, rng_connection),
        adv_character_creator.MakeScreen(current_screen, party_members[0], party_members, member_count),
        ready_screen.MakeScreen(current_screen, party_members, member_count, character_creator),
        battle_screen.MakeScreen(current_screen, party_members, enemy_members)
        }, &current_screen);

    screen.Loop(screen_router);

    return 0;
}

