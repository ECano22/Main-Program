#include <map>
#include <string>
#include <type_traits>
#include <algorithm>
#include <functional>
#include <nlohmann/json.hpp>
#include "classes.h"

AllyAttack mrc_atk = { "Cleave", 1.5 };
AllySupport clr_heal = { "Heal", 10 };

inline CharClass hkn_class = { 0, "Heavy Knight", 30, 5, 8, 5, 0, {} };
inline CharClass mrc_class = { 1, "Mercenary", 20, 10, 11, 3, 5, {mrc_atk} };
inline CharClass clr_class = { 2, "Cleric", 15, 15, 8, 2, 5, {clr_heal} };

// Returns a reference to the static database map
const CharClass GetClass(int index) {
	static const std::map<int, CharClass> class_map = {
		{ 0, hkn_class },
		{ 1, mrc_class },
		{ 2, clr_class }
	};
	return class_map.at(index);
}

//populates the provided EnemyArray with enemies
void GetEnemies(std::array<EnemyChar, MAX_PARTY_MEMBERS>& enemy_array)
{
	const CharClass stats = GetClass(1);
	for (EnemyChar& enemy : enemy_array)
	{
		enemy.name = stats.class_name;
		enemy.atk = stats.atk;
		enemy.def = stats.def;
		enemy.spd = stats.spd;
		enemy.MaxHP = stats.HP;
		enemy.HP = stats.HP;
	}
}

void GetSkills(std::vector<std::string>& list, int class_ID)
{
	list.clear();
	for (const auto& skill : GetClass(class_ID).skills)
	{
		list.push_back(std::visit([](const auto& value) {
			return value.name;
			}, skill));
	}
}

void TurnOrder(std::vector<std::variant<PartyChar*, EnemyChar*>>& turn_order,
			   std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members,
			   std::array<EnemyChar, MAX_PARTY_MEMBERS>& enemy_members)
{
	turn_order.clear();
	for (auto& member : party_members) turn_order.push_back(&member);
	for (auto& member : enemy_members) turn_order.push_back(&member);
	std::ranges::sort(turn_order, std::greater<>{}, [](const auto& x)
		{
			return std::visit([](const auto* value)
				{
					return value->spd;
				}, x);
		});
}

//returns how much HP was removed
int SubtractHP(int& HP, int value)
{
	int initHP = HP;
	HP -= value;
	if (HP < 0) HP = 0;
	return initHP - HP;
}
//returns how much HP was added
int AddHP(int& HP, int maxHP, int value)
{
	int initHP = HP;
	HP += value;
	if (HP < maxHP) HP = maxHP;
	return HP - initHP;
}
int ExecuteAllySupport(PartyChar target, AllySupport skill)
{
	return AddHP(target.HP, target.MaxHP, skill.heal_amount);
}
int ExecuteEnemyAttack(EnemyChar attacker, std::array<PartyChar, MAX_PARTY_MEMBERS>& party_members,
	ZMQConnection& weighted_service)
{
	std::vector<float> weighted_vector;
	for (auto& member : party_members)
	{
		if (member.HP > 0 && member.is_used)
		{
			if (member.class_ID == 0) weighted_vector.push_back(2);
			else weighted_vector.push_back(1);
		}
		else weighted_vector.push_back(0);
	}
	nlohmann::json weighted_json = weighted_vector;
	auto target_idx = weighted_service.send_json(weighted_json)["value"];
	return SubtractHP(party_members[target_idx].HP, attacker.atk);
}