#pragma once
#include <map>
#include <array>
#include <variant>
#include <vector>
#include "global.h"

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
struct EnemyChar
{
public:
	std::string name;
	int HP, MaxHP, atk, def, spd;
};

const CharClass GetClass(int index);
void GetEnemies(std::array<EnemyChar, MAX_PARTY_MEMBERS>& enemy_array);

//retrieves names from an array of PartyChar or EnemyChar
template <typename T>
void GetNames(std::vector<std::string>& list, std::array<T, MAX_PARTY_MEMBERS>& member_list)
{
	list.clear();
	for (const auto& member : member_list)
	{
		if constexpr (std::is_same_v<T, PartyChar>)
		{
			if (member.is_used) list.push_back(member.name);
		}
		else if constexpr (std::is_same_v<T, EnemyChar>) list.push_back(member.name);
	}
}