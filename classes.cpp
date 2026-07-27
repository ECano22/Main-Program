#include <map>
#include <string>
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