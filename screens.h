#pragma once
#include <ftxui/ftxui.hpp>
#include <vector>
#include <string>
#include <functional>
#include <array>
#include "classes.h"
#include "global.h"

class MainMenu
{
	public:
		int selection = 0;
		const std::vector<std::string> entries =
		{
			"New Game",
			"Exit Game",
		};
		ftxui::Component MakeScreen(int& current_screen);
};

class QuitConfirm
{
	public:
		int selection = 0;
		const std::vector<std::string> entries =
		{
			"Yes",
			"No",
		};
		std::function<void()> on_quit;
		QuitConfirm(std::function<void()> quit_function);
		ftxui::Component MakeScreen(int& current_screen);
};
class ReadyScreen;

class CharacterCreator
{
	public:
		std::string name;
		int char_class = 0;
		int selection = 0;
		int section = 0;
		std::vector<std::string> class_stats;
		const std::vector<std::string> class_entries =
		{
			"Heavy Knight",
			"Mercenary",
			"Cleric",
		};
		const std::vector<std::string> confirm_entries =
		{
			"Yes",
			"No",
		};
		ftxui::Component MakeScreen(int& current_screen, PartyChar& party_member, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, int& member_count, ReadyScreen& ready_screen);
		void ClearData();
};

class AdvCharacterCreator
{
	public:
		const std::vector<std::string> left_entries =
		{
			"<",
			"<",
			"<",
			"<",
			"<",
		};
		const std::vector<std::string> right_entries =
		{
			">",
			">",
			">",
			">",
			">",
		};
		int stat_pool = 0;
		int dec_inc = 0;
		int stat_modified = 0;
		ftxui::Component MakeScreen(int& current_screen, PartyChar& party_member, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, int& member_count);
};

class ReadyScreen
{
	public:
		int selection = 0;
		int member_selection = 0;
		int is_selecting_member = 0;
		const std::vector<std::string> entries =
		{
			"New Character",
			"Edit Character",
			"Remove Character",
		};
		std::vector<std::string> member_list;
		ftxui::Component MakeScreen(int& current_screen, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members, int& member_count, CharacterCreator& character_creator);
		void PopulatePartyList(std::vector<std::string>& list, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members);
		void ClearData();
};