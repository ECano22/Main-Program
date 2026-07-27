#pragma once
#include <map>

struct CharClass
{
	public:
		int ID;
		std::string class_name;
		int HP, SP, atk, def, spd;
};
struct PartyChar
{
	public:
		std::string name;
		std::string class_name;
		int HP, SP, atk, def, spd;
};

const CharClass GetClass(int index);