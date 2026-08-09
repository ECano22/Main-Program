#include <map>
#include <string>
#include <type_traits>
#include "classes.h"

inline CharClass hkn_class = { 0, "Heavy Knight", 30, 5, 8, 5, 0 };
inline CharClass mrc_class = { 1, "Mercenary", 20, 10, 11, 3, 5 };
inline CharClass clr_class = { 2, "Cleric", 15, 15, 8, 2, 5 };

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