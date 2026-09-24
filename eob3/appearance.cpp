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

int select_avatars(unsigned char* result, racen race, gendern gender, classn type) {
	auto ps = result;
	for(unsigned char i = 0; i < lenghof(portraits); i++) {
		if(portraits[i].gender != gender)
			continue;
		if(!portraits[i].classes.is(type))
			continue;
		if(!portraits[i].races.is(race))
			continue;
		*ps++ = i;
	}
	return ps - result;
}