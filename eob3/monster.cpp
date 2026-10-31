#include "game.h"
#include "creature.h"

monsteri monsters[LastMonster + 1] = {
	{},
	{KOBOLD, Goblinoid, {}, 0, 7, 7, LawfulEvil, {Dagger}},
	{LEECH, Animal, {}, 4, 9, 175, TrueNeutral, {}, {DiseaseAttack, VampiricAttack}},
	{SKELETON, Human, {}, 1, 7, 65, TrueNeutral, {Axe, Helm}, {ResistPiercing, ResistSlashing, Undead}},
	{WOLF, Animal, {}, 3, 7, 65, TrueNeutral, {Claws1d4}, {}, {16}},
};

bool is_large(resn v) {
	switch(v) {
	case LEECH: case WOLF:
		return true;
	default: return false;
	}
}

bool is_large(monstern v) {
	return is_large(monsters[v].res);
}

monstern get_minions(monstern v) {
	return NoMonster;
}