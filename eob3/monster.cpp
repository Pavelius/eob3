#include "game.h"
#include "creature.h"

monsteri monsters[LastMonster + 1] = {
	{},
	{KOBOLD, Goblinoid, {}, 0, 7, 7, LawfulEvil, {Dagger}},
	{LEECH, Animal, {}, 4, 9, 175, TrueNeutral, {}},
};

bool is_large(resn v) {
	switch(v) {
	case LEECH: return true;
	default: return false;
	}
}

monstern get_minions(monstern v) {
	return NoMonster;
}