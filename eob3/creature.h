#pragma once

#include "item.h"
#include "pointc.h"

typedef bool (*fncfilter)(unsigned char v);

enum messagen : unsigned char;
enum monstern : unsigned char;
enum resn : unsigned char;
enum spelln : unsigned char;

enum groupn : unsigned char {
	Warriors, Priests, Rogues, Wizards,
};
enum classn : unsigned char {
	Monster,
	Fighter, Ranger, Paladin, Mage, Cleric, Theif,
	FighterCleric, FighterMage, FighterTheif,
	MageTheif,
	FighterMageTheif,
};
enum racen : unsigned char {
	Human, Dwarf, Elf, HalfElf, Halfling,
	Goblinoid, Animal,
};
enum alignmentn : unsigned char {
	TrueNeutral,
	LawfulGood, NeutralGood, ChaoticGood,
	LawfulNeutral, ChaoticNeutral,
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
	Hits, LastAbility = Hits,
	Level, Experience,
};
enum monstern : unsigned char {
	NoMonster,
	Kobold, Leech, DwarfWarrior, Spider,
	LastMonster = Spider
};
enum speechn : unsigned char {
	CantUseItem, CantRead, ThisIsUndefinedObject, ThisIsObject, ThisIsNotItem, CantPutItemHere,
	MustBeUseInHand, MustBeWearing, MustBeQuver,
	WhereIsKeyhole, ThisIsWrongKey,
	CantUseInSettlement,
	LastSpeech = CantUseInSettlement
};

extern const char* ability_names[Experience + 1];
extern const char* ability_short[Experience + 1];
extern const char* alignment_names[ChaoticEvil + 1];
extern const char* class_names[FighterMageTheif + 1];
extern const char* gender_names[Female + 1];
extern const char* fatigue_status[4];
extern const char* monster_names[LastMonster + 1];
extern const char* name_names[50 * 4];
extern const char* race_names[Halfling + 1];
extern const char* speech_names1[LastSpeech + 1];
extern const char* speech_names2[LastSpeech + 1];
extern const char* speech_names3[LastSpeech + 1];

int get_class_count(classn v);
int get_class_index(classn base, classn type);

bool is_large(resn v);
bool is_large(monstern v);

classn get_class(classn v, int index);

struct racenc {
	unsigned char data = 0;
	racenc() = default;
	template<typename... Ts> constexpr racenc(racen v, Ts... args) : racenc(args...) { set(v); }
	bool is(racen v) const { return (data & (1 << v)) != 0; }
	void set(racen v) { data |= (1 << v); }
};

struct monsteri {
	resn			res; // Main graphic resource
	racen			race; // Main race of creature
	char			overlays[4]; // Graphics ovelays with different weapons or faces.
	char			hd, ac; // Combat statistic.
	int				exp; // Experience award for killing.
	alignmentn		alignment; // Default behaivor. Evil is aggressive.
	itemn			items[4]; // This items will be equip and some time looted.
	featc			feats; // Special feats
};
extern monsteri monsters[LastMonster + 1];

struct statable {
	char			abilities[LastAbility + 1];
	featc			feats;
};

struct npci {
	alignmentn		alignment;
	racen			race;
	gendern			gender;
	classn			type;
	monstern		monster;
	unsigned char	avatar, name_id;
	char			levels[3];
	const char* name() const { return (name_id == 0xFF) ? race_names[race] : name_names[name_id]; }
	int level() const { return levels[0]; }
	void say(messagen id, ...) const;
	void say(speechn id, ...) const;
	void sayv(const char* id, const char* format) const;
};

struct creature : npci, posable, statable, wearable {
	statable		basic;
	unsigned		experience;
	short			hp, hpm, hpr, food;
	unsigned char	pallette;
	constexpr explicit operator bool() const { return hp > 0; }
	const char* strvalue(abilityn id) const;
	combati getattack(wearn id, bool large_enemy) const;
	int get(abilityn v) const { return abilities[v]; }
	int get(classn v) const { auto n = get_class_index(type, v); return (n == -1) ? 0 : levels[n]; }
	int getfood() const { return 6 * 10; } // Each turn make con test or decrease food.
	int gethp() const { return hpm; }
	void add(abilityn n, int v);
	bool allow(itemn type) const;
	bool allow(itemn type, speechn speech) const;
	bool canread() const { return true; }
	void clear();
	void damage(damagen type, int value) {}
	void equip(item& v);
	void equip(const item& v) { item cv = v; equip(cv); }
	bool is(abilityn v) const { return abilities[v] > 0; }
	bool is(classn v) const { return get_class_index(type, v) != -1; }
	bool is(featn v) const { return feats.is(v); }
	bool is(racen v) const { return race == v; }
	bool isactable() const;
	bool isdisabled() const { return false; }
	bool isdead() const { return hp <= -10; }
	bool islarge() const { return is_large(monsters[monster].res); }
	void kill() {}
	bool roll(abilityn v, int bonus = 0) const;
	void joinparty() { /*TODO: Join party later.*/ }
	void setframe(short* frames, short index) const;
	void update();
};
extern creature characters[32]; // All characters in game
extern creature* adventurers[6]; // Party of characters
extern creature* player;
extern creature* opponent;

creature* get_creature(void* pointer);
creature* new_character();

wearn get_wear(void* pointer);

unsigned char random_avatar(racen race, gendern gender, classn type);
unsigned char random_name(racen race, gendern gender);

monstern get_minions(monstern v);
racen get_race(monstern v);

int get_hit_die(classn type);
int get_party_index(const creature* player);
int select_avatars(unsigned char* result, racen race, gendern gender, classn type, fncfilter filter);
int select_names(unsigned char* result, racen race, gendern gender, fncfilter filter);

bool allow(alignmentn type, classn v);
bool allow(classn type, racen v);
void create_charater(racen race, gendern gender, classn class_type, alignmentn alignment);
void create_monster(monstern type);
void create_monster_pallette();
void finish_character();
bool no_party_avatar(unsigned char v);
void reroll_ability();
void reroll_character();
void reroll_hits();
void update_player();
void use_item(creature* player, item* last_item, wearn wear);