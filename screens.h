#pragma once
#include <ftxui/ftxui.hpp>
#include <vector>
#include <string>
#include <functional>
#include <array>
#include "classes.h"

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
		ftxui::Component MakeScreen(int& current_screen, PartyChar& party_member);
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
		ftxui::Component MakeScreen(int& current_screen, PartyChar& party_member);
};

class StatsScreen
{
	public:
		int selection;
		const std::vector<std::string> entries =
		{
			"New Character"
			"Edit Character"
			"Remove Character"
		};
		ftxui::Component MakeScreen(int& current_screen)
};