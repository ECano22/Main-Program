#pragma once
#include <map>
#include <array>
#include <variant>
#include <vector>
#include <type_traits>
#include "global.h"
#include "zmq_classes.h"

struct AllyAttack
{
	std::string name;
	float modifier;
};
struct AllySupport
{
	std::string name;
	int heal_amount;
};

struct CharClass
{
	public:
		int ID;
		std::string class_name;
		int HP, SP, atk, def, spd;
		std::vector<std::variant<AllyAttack, AllySupport>> skills;
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
template <typename T>
T GetSkillAtIdx(int class_ID, int idx)
{
	return GetClass(class_ID).skills[idx];
}

void GetSkills(std::vector<std::string>& list, int class_ID);
void TurnOrder(std::vector<std::variant<PartyChar*, EnemyChar*>>& turn_order,
	std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members,
	std::array<EnemyChar, MAX_PARTY_MEMBERS>& enemy_members);
int SubtractHP(int& HP, int value);
int AddHP(int& HP, int maxHP, int value);

template <typename T>
int ExecuteAllyAttack(PartyChar attacker, EnemyChar& target, T attack)
{
	float modifier = 1.0;
	if constexpr (std::is_same_v<T, AllyAttack>)
	{
		modifier = attack.modifier;
	}
	return SubtractHP(target.HP, attacker.atk * modifier);
}
int ExecuteAllySupport(PartyChar target, AllySupport skill);
int ExecuteEnemyAttack(EnemyChar attacker, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members,
	ZMQConnection& weighted_service);

bool AllDead(std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members);
bool AllDead(std::array<EnemyChar, MAX_PARTY_MEMBERS>& enemy_members);