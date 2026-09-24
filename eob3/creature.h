#pragma once

#include "item.h"

enum groupn : unsigned char {
	Warriors, Priests, Rogues, Wizards,
};
enum classn : unsigned char {
	Fighter, Ranger, Paladin, Mage, Cleric, Theif,
	FighterCleric, FighterMage, FighterTheif,
	MageTheif,
	FighterMageTheif,
};
enum racen : unsigned char {
	Human, Dwarf, Elf, HalfElf, Halfling,
};
enum alignmentn : unsigned char {
	LawfulGood, NeutralGood, ChaoticGood,
	LawfulNeutral, TrueNeutral, ChaoticNeutral,
	LawfulEvil, NeutralEvil, ChaoticEvil,
};
enum gendern : unsigned char {
	Male, Female,
};
enum abilityn : unsigned char {
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
	SaveVsParalization, SaveVsPoison, SaveVsTraps, SaveVsMagic,
	ClimbWalls, HearNoise, MoveSilently, OpenLocks, PickPockets, RemoveTraps, ReadLanguages,
	LearnSpell,
	ResistMagic,
	CriticalDeflect, DetectSecrets,
	AC, AttackMelee, AttackRange, DamageMelee, DamageRange,
	Speed, TurnUndeadBonus, Backstab, AdditionalAttacks,
	Spell1, Spell2, Spell3, Spell4, Spell5, Spell6, Spell7, Spell8, Spell9, Spells,
	BonusExperience, ReactionBonus,
	ExeptionalStrenght,
	AcidD1Level, AcidD2Level, PoisonLevel, DiseaseLevel, DuplicateIllusion,
	DrainedStrenght, DrainedConstitution, DrainedLevels,
	Hits
};

extern const char* ability_names[Hits + 1];
extern const char* ability_short[Hits + 1];
extern const char* alignment_names[ChaoticEvil + 1];
extern const char* class_names[FighterMageTheif + 1];
extern const char* gender_names[Female + 1];
extern const char* race_names[Halfling + 1];
extern const char* name_names[];

int get_class_count(classn v);
int get_class_index(classn base, classn type);

classn get_class(classn v, int index);

struct npci {
	alignmentn		alignment;
	racen			race;
	gendern			gender;
	classn			type;
	unsigned char	avatar, name_id;
	char			levels[3];
	const char* name() const { return (name_id==0xFF) ? race_names[race] : name_names[name_id]; }
	int hd() const { return levels[0]; }
};

struct statable {
	char			abilities[Hits + 1];
};

struct creature : npci, statable, wearable {
	unsigned		experience;
	short			hp, hpm, food;
	int get(abilityn v) const { return abilities[v]; }
	int get(classn v) const { auto n = get_class_index(type, v); return (n == -1) ? 0 : levels[n]; }
	int getfood() const { return get(Constitution) * 20; }
	int gethp() const { return hpm; }
	bool is(classn v) const { return get_class_index(type, v) != -1; }
	bool isdisabled() const { return false; }
	bool isdead() const { return hp <= -10; }
};
extern creature characters[32]; // All characters in game
extern creature* party[6]; // Party of characters
extern creature* player;

int get_party_index(const creature* player);

bool allow(alignmentn type, classn v);
bool allow(classn type, racen v);
void update_player();