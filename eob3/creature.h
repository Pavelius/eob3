#pragma once

#include "item.h"
#include "pointc.h"
#include "spell.h"

typedef bool (*fncfilter)(unsigned char v);

const int encounter_table_maximum = 19;
const int party_size = 6;

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
	Goblinoid, Demon, Animal,
	LastRace = Animal
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
enum reactions : unsigned char {
	Indifferent, Friendly, Careful, Hostile,
};
enum speechn : unsigned char {
	CantUseItem, CantRead, ThisIsUndefinedObject, ThisIsObject, ThisIsNotItem, CantPutItemHere,
	SomeKindOfP1, ItemNotFit, NothingToGrab,
	MustBeUseInHand, MustBeWearing, MustBeQuver,
	WhereIsKeyhole, ThisIsWrongKey,
	SecrectButtonFound,
	IAmScarry, IAmTired, ISeeSomething,
	IHearSomething, IHearSomethingLarge, BehideThisDoorIsNoOne,
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
extern const char* language_names[LastRace + 1];
extern const char* name_names[50 * 4];
extern const char* race_names[Halfling + 1];
extern const char* speech_names1[LastSpeech + 1];
extern const char* speech_names2[LastSpeech + 1];
extern const char* speech_names3[LastSpeech + 1];

int get_class_count(classn v);
int get_class_index(classn base, classn type);

bool is_chaotic(alignmentn v);
bool is_large(resn v);
bool is_large(monstern v);
bool is_lawful(alignmentn v);

classn get_class(classn v, int index);

struct racenc {
	unsigned short data = 0;
	racenc() = default;
	template<typename... Ts> constexpr racenc(racen v, Ts... args) : racenc(args...) { set(v); }
	void clear() { data = 0; }
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
	const char* name() const { return monster ? monster_names[monster] : name_names[name_id]; }
	int level() const { return levels[0]; }
	void say(messagen id, ...) const;
	void say(speechn id, ...) const;
	void say(const char* foramt, ...) const;
	void sayv(const char* format, const char* format_param) const;
};

struct creature : npci, posable, statable, wearable, spellbook {
	racenc			languages;
	statable		basic;
	unsigned		experience;
	short			hp, hpm, hpr, hp_aid, food;
	unsigned char	pallette;
	char			initiative;
	reactions		reaction;
	constexpr explicit operator bool() const { return hp > 0; }
	const char* strvalue(abilityn id) const;
	combati getattack(wearn id, bool large_enemy) const;
	static bool allow(spelln spell, int level);
	int expaward() const;
	int get(abilityn v) const { return abilities[v]; }
	int get(classn v) const { auto n = get_class_index(type, v); return (n == -1) ? 0 : levels[n]; }
	int getfood() const { return 6 * 10; } // Each turn make con test or decrease food.
	int gethitpenalty(int bonus) const;
	int gethp() const { return hpm; }
	int level() const { return npci::level(); }
	int level(spelln spell) const;
	void add(abilityn n, int v);
	void add(featn feat, unsigned duration) {}
	void addexp(unsigned v) { experience += v; checklevel(); }
	bool allow(itemn type) const;
	bool allow(itemn type, speechn speech) const;
	bool apply(spelln spell, int level, bool run);
	bool canread() const { return true; }
	bool cast(spelln spell, bool run);
	bool charmable() const { return get(Intellegence) >= 4; }
	void clear();
	void damage(damagen type, int value, bool magic_weapon);
	void damage(damagen type, int value) { damage(type, value, true); }
	void damage(damagen type, int value, abilityn save, bool save_negate);
	void equip(item& v);
	void equip(const item& v) { item cv = v; equip(cv); }
	void heal(int hits) {}
	bool is(abilityn v) const { return abilities[v] > 0; }
	bool is(alignmentn v) const { return alignment == v; }
	bool is(classn v) const { return get_class_index(type, v) != -1; }
	bool is(featn v) const { return feats.is(v); }
	bool is(racen v) const { return race == v; }
	bool is(const item & weapon, featn v) const { return is(v) || weapon.power == v; }
	bool isactable() const;
	bool isdisabled() const { return hp <= 0; }
	bool isdead() const { return hp <= -10; }
	bool islarge() const { return is_large(monsters[monster].res); }
	bool isready() const { return !isdisabled() && !is(Paralizing); }
	bool isunderstand(racen v) const { return languages.is(v); }
	bool specialized(const item& weapon) const;
	void kill();
	bool roll(abilityn v, int bonus = 0) const;
	void joinparty() { /*TODO: Join party later.*/ }
	void remove(featn v) { feats.remove(v); }
	void set(featn v) { feats.set(v); }
	void setframe(short* frames, short index) const;
	void update();
private:
	void apply(spelln spell, fnevent value, unsigned duration);
	void apply(spelln spell, featn value, unsigned duration);
	void apply(spelln spell, itemn value, unsigned duration);
	void checklevel();
};
extern creature characters[32]; // All characters in game
extern creature* adventurers[party_size]; // Party of characters
extern creature* player;
extern creature* opponent;

creature* get_creature(void* pointer);
creature* get_leader(creature** source);
creature* new_character();

wearn get_wear(void* pointer);

item* get_item(void* pointer);

unsigned char random_avatar(racen race, gendern gender, classn type);
unsigned char random_name(racen race, gendern gender);

monstern get_minions(monstern v);
racen get_race(monstern v);

bool is_character(const creature* p);

int get_hit_die(classn type);
int party_index(const creature* player);
int party_median(creature** source, abilityn v);
int select_avatars(unsigned char* result, racen race, gendern gender, classn type, fncfilter filter);
int select_names(unsigned char* result, racen race, gendern gender, fncfilter filter);

bool allow(alignmentn type, classn v);
bool allow(classn type, racen v);
void check_reaction(creature** creatures, int bonus);
void create_character(racen race, gendern gender, classn class_type, alignmentn alignment);
void create_monster(monstern type);
void create_monster_pallette();
void finish_character();
void learn_spells(creature* player, int level, int spell_type);
bool no_party_avatar(unsigned char v);
bool party_have(creature** source, alignmentn type);
bool party_have(creature** source, classn type);
bool party_roll(abilityn v, int bonus);
void party_set(creature** source, featn v, bool apply = true);
void party_set(creature** source, reactions v);
void reroll_ability();
void reroll_character();
void reroll_hits();
void update_player();
void use_item(creature* player, item* last_item, wearn wear);