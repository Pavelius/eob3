#include "creature.h"
#include "game.h"

struct portraiti {
	gendern		gender;
	racenc		races;
	classnc		classes;
};

static portraiti portraits[] = {
	{},
	{Male, Human, Fighter}, // 1
	{Male, Human, {Fighter, Cleric, Mage}},
	{Male, Human, {Fighter, Cleric, Mage}},
	{Male, Human, {Theif}},
	{Male, {Elf, HalfElf}, {Fighter, Theif}}, // 5
	{Male, {Elf, HalfElf}, {Mage, Theif}},
	{Male, {Dwarf}, {Fighter, Theif}},
	{Male, {Human, HalfElf}, {Fighter, Theif, Mage}},
	{Male, {Human}, {Fighter, Cleric, Theif}},
	{Male, {Halfling}, {Fighter, Cleric, Theif}}, // 10
	{Male, {Elf, HalfElf}, {Mage, Theif}},
	{Male, {Human, Halfling}, {Fighter, Theif, Mage}},
	{Male, {Human, Dwarf, Halfling}, {Cleric, Mage}},
	{Male, {Human}, {Cleric, Fighter, Paladin}},
	{Male, {Human}, {Mage, Cleric}}, // 15
	{Male, {Human}, {Fighter, Paladin}},
	{Male, {Elf, HalfElf}, {Mage, Fighter, Cleric, Theif}},
	{Male, {Human}, {Fighter, Paladin, Cleric}},
	{Male, {Human}, {Fighter, Paladin, Cleric}},
	{Male, {Human, Elf, HalfElf}, {Fighter, Paladin}}, // 20
	{Male, {Elf, HalfElf}, {Fighter, Paladin, Ranger}},
	{Male, {Dwarf}, {Fighter, Paladin}},
	{Male, {Elf, HalfElf}, {Fighter, Paladin, Ranger}},
	{Male, {Human}, {Fighter}},
	{Male, {Dwarf}, {Fighter}}, // 25
	{Male, {Human}, {Fighter, Cleric}},
	{Female, {Human, Elf, HalfElf}, {Mage}},
	{Female, {Human, Dwarf}, {Mage, Cleric, Theif}},
	{Female, {Human, Dwarf, Halfling}, {Mage, Cleric, Theif}},
	{Female, {Elf, HalfElf}, {Mage, Theif, Cleric}}, // 30
	{Female, {Human, HalfElf}, {Fighter, Theif}},
	{Female, {Human}, {Fighter, Paladin, Cleric}},
	{Female, {Elf}, {Mage, Cleric}},
	{Female, {Elf, HalfElf}, {Fighter, Mage, Cleric}},
	{Female, {Human, HalfElf}, {Fighter, Mage, Cleric}}, // 35
	{Female, {Human, HalfElf}, {Fighter, Paladin, Cleric}},
	{Female, {Human, Dwarf, Halfling}, {Fighter, Mage, Cleric}},
	{Female, {Human, Dwarf, Halfling}, {Fighter, Mage, Cleric, Theif}},
	{Female, {Human, Dwarf}, {Fighter, Mage, Cleric, Theif}},
	{Female, {Human, Halfling}, {Fighter, Theif, Mage, Cleric}}, // 40
	{Female, {Human, HalfElf, Elf}, {Cleric, Mage}},
	{Female, {Human, HalfElf, Elf}, {Cleric, Mage}},
	{Female, {Human, HalfElf}, {Fighter, Paladin, Ranger, Cleric}},
	{Female, {Human, HalfElf, Elf}, {Cleric, Mage}},
	{Female, {Human, HalfElf}, {Fighter, Paladin, Ranger, Cleric}}, // 45
	{Male, {Human, HalfElf, Elf}, {Fighter, Paladin}},
	{Female, {HalfElf, Elf, Halfling}, {Fighter, Mage, Theif, Cleric}},
	{Female, {Human, HalfElf, Elf, Halfling}, {Mage, Theif, Cleric}},
	{Female, {Human, HalfElf}, {Mage, Theif, Cleric}},
	{Male, {Halfling}, {Fighter, Theif}}, // 50
	{Male, {Dwarf}, {Fighter, Cleric}},
	{Male, {Dwarf}, {Fighter, Cleric}},
	{Male, {Dwarf}, {Fighter, Cleric, Theif}},
	{Male, {Halfling}, {Fighter, Cleric, Theif}},
	{Female, {Human}, {Fighter, Ranger, Mage, Theif, Cleric}}, // 55
	{Male, {Dwarf}, {Fighter, Cleric, Theif}},
	{Male, {Elf}, {Mage, Cleric, Theif}},
	{Male, {Dwarf}, {Fighter, Cleric, Theif}},
	{Female, {Human, HalfElf, Elf, Halfling}, {Cleric, Mage, Theif}},
};

static bool match(const classnc& flags, classn type) {
	auto n = get_class_count(type);
	for(auto i = 0; i < n; i++) {
		if(flags.is(get_class(type, i)))
			return true;
	}
	return false;
}

int select_avatars(unsigned char* result, racen race, gendern gender, classn type, fncfilter filter) {
	auto ps = result;
	for(unsigned char i = 0; i < lenghof(portraits); i++) {
		if(portraits[i].gender != gender)
			continue;
		if(!match(portraits[i].classes, type))
			continue;
		if(!portraits[i].races.is(race))
			continue;
		if(filter && !filter(i))
			continue;
		*ps++ = i;
	}
	return ps - result;
}

static int get_name_group(racen race) {
	switch(race) {
	case Elf: case HalfElf: return 50 * 1;
	case Dwarf: return 50 * 2;
	case Halfling: return 50 * 3;
	default: return 0;
	}
}

int select_names(unsigned char* result, racen race, gendern gender, fncfilter filter) {
	auto ps = result;
	auto pb = get_name_group(race);
	auto bs = (gender == Female) ? 1 : 0;
	for(unsigned char i = 0; i < 50 / 2; i++) {
		auto v = pb + i * 2 + bs;
		if(filter && !filter(v))
			continue;
		*ps++ = v;
	}
	return ps - result;
}