#pragma once
#include <ftxui/ftxui.hpp>
#include <vector>
#include <string>
#include <functional>

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
		ftxui::Component MakeScreen(int& current_screen);
};