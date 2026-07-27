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
		int class_ID;
		int HP, MaxHP, SP, MaxSP, atk, def, spd;
		int is_used = 0;
};

const CharClass GetClass(int index);