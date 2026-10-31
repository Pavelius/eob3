#include "game.h"
#include "creature.h"

monsteri monsters[LastMonster + 1] = {
	{},
	{KOBOLD, Goblinoid, {}, 0, 7, 7, LawfulEvil, {Dagger}},
	{GOBLIN, Goblinoid, {}, 1, 8, 15, LawfulEvil, {Axe, Shield}, {}, {0, 0, 6}},
	{LEECH, Animal, {}, 4, 9, 175, TrueNeutral, {}, {DiseaseAttack, VampiricAttack}},
	{WOLF, Animal, {}, 3, 7, 65, TrueNeutral, {Claws1d4}, {}, {16}},
	{DWARF, Dwarf, {}, 1, 7, 30, LawfulGood, {BattleAxe, DwarvenHelm}, {}},
	{SPIDER1, Animal, {}, 3, 7, 30, LawfulGood, {Bite1d6}, {PoisonAttack}},
	{SKELETON, Human, {}, 1, 7, 65, TrueNeutral, {Axe, Helm}, {ResistPiercing, ResistSlashing, ImmuneDisease, ImmunePoison, Undead}},
	{ZOMBIE, Human, {}, 2, 8, 65, TrueNeutral, {Slam1d8}, {ImmuneDisease, ImmunePoison, SlowMove, Undead}},
	{GHOUL, Human, {}, 2, 6, 175, ChaoticEvil, {Claws1d3, Bite1d6}, {ParalizeAttack, ImmuneDisease, ImmunePoison, Undead}},
	{SHGHOUL, Human, {}, 4, 6, 650, ChaoticEvil, {Claws1d4, Bite1d8}, {ParalizeAttack, ImmuneDisease, ImmunePoison, Undead}},
	{WIGHT, Human, {}, 4, 6, 650, ChaoticEvil, {Slam1d4}, {ParalizeAttack, ImmuneDisease, ImmunePoison, Undead}},
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