#include "creature.h"
#include "game.h"

struct portraiti {
	gendern		gender;
	racenc		races;
	classnc		classes;
};

static portraiti portraits[] = {
	{},
	{Male, Human, Fighter},
	{Male, Human, {Fighter, Cleric, Mage}},
	{Male, Human, {Fighter, Cleric, Mage}},
	{Male, Human, {Theif}},
	{Male, {Elf, HalfElf}, {Fighter, Theif}},
	{Male, {Elf, HalfElf}, {Mage, Theif}},
	{Male, {Dwarf}, {Fighter, Theif}},
	{Male, {Human, HalfElf}, {Fighter, Theif, Mage}},
	{Male, {Human}, {Fighter, Cleric, Theif}},
	{Male, {Halfling}, {Fighter, Cleric, Theif}},
	{Male, {Elf, HalfElf}, {Mage, Theif}},
	{Male, {Human, Halfling}, {Fighter, Theif, Mage}},
};

int select_avatars(unsigned char* result, racen race, gendern gender, classn type, fncfilter filter) {
	auto ps = result;
	for(unsigned char i = 0; i < lenghof(portraits); i++) {
		if(portraits[i].gender != gender)
			continue;
		if(!portraits[i].classes.is(type))
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
	auto pe = pb + 50;
	auto bs = (gender == Female) ? 1 : 0;
	for(unsigned char i = 0; i < 50 / 2; i++) {
		auto v = i * 2 + bs;
		if(filter && !filter(v))
			continue;
		*ps++ = v;
	}
	return ps - result;
}