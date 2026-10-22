#pragma once

#include "color.h"

enum abilityn : unsigned char;
enum celln : unsigned char;
enum damagen : unsigned char;
enum featn : unsigned char;

struct creature;
struct item;

typedef void(*fnevent)();

enum spelln : unsigned char {
	Bless, CureLightWound, DetectEvil, DetectMagic, ProtectionFromEvil, PurifyFood,
	Aid, FlameBlade, FindTraps, HoldPerson, ResistFireSpell, ResistColdSpell, SlowPoison,
	CreateFood, CureDisease, MagicalVestment, Prayer, RemoveCurse, RemoveParalize,
	Abjure, CloackOfBravery, CureSeriousWounds, Fear, FreeAction, NeutralizePoison, ProtectionFromEvil10ft,
	CureCriticalWounds, DispellEvil, FlameStrike, RaiseDead, TrueSeeing,
	BladeBarrier, FindPath, HealSpell, HeroesFeast, WorldOfRecall,
	Restoration, FireStorm, Ressurection, SunRay, SymbolPain,
	Armor, BurningHands, ChillTouch, ComprehendLanguages, Friends, Identify, MagicMissile, Mending, ShieldSpell, ShockingGrasp,
	Blinding, Blur, HypnoticPattern, Knock, Invisibility, LocateGems, MelfAcidArrow, MirrorImage, Scare, StrenghtSpell,
	Blink, Clairaudience, DispellMagic, Fireball, Haste, HoldUndead, Suggestion, VampiricTouch,
	ImprovedInvisibility,
	LastSpell = ImprovedInvisibility,
};

enum spellfn : unsigned char {
	You, Ally, AllAlly,
	Enemy, AllEnemy,
	AllAllyItems, YouItem,
	SummonWeapon, ShowArea,
};

extern const char* spell_names[LastSpell + 1];

struct spelli {
	char			levels[2]; // 0 - priest, 1 - mage, 2 - other
	spellfn			type; // Effect type
	spelln			index; // Spell (for presentation)
	color			lighting; // Active color border hilite
	fnevent			apply; // When active use this 
	fnevent			stopping; // When active spell stop
	const char*		name() const { return spell_names[index]; }
};
extern spelli spell_data[LastSpell + 1];

struct targetref {
	unsigned char	type = 0xFF; // 0..250 is dungeon index for monsters, 0xFE is character.
	unsigned char	index = 0; // Index of target in array.
	targetref() = default;
	targetref(const creature* p);
	constexpr explicit operator bool() const { return type != 0xFF; }
	constexpr bool operator ==(const targetref& v) const { return type == v.type && index == v.index; }
	constexpr bool operator !=(const targetref& v) const { return type != v.type || index != v.index; }
	operator creature*() const;
	void clear() { type = 0xFF; index = 0; }
};

struct boost {
	targetref		target; // Spell target
	unsigned		stop; // Stop when time will be this.
	spelln			spell; // Spell index
	featn			feat; // Additional feat
	itemn			summon; // Additional summoned weapon
};
extern boost boosts[256];
extern unsigned char boost_count;

struct spellbook {
	char			spells[LastSpell + 1];
};

struct spella : spellbook {
	unsigned		data[1 + LastSpell / 32];
	void			addspells(int level, int count);
	bool			is(spelln v) const { return (data[v / 32] & (1 << (v % 32))) != 0; }
	void			remove(spelln v) { data[v / 32] &= ~(1 << (v % 32)); }
	void			set(spelln v) { data[v / 32] |= 1 << (v % 32); }
	int				total(int type, int level) const;
};
extern spella spellbooks[32]; // Size exacly equal lenghtof(characters)

extern creature* caster;

spella* get_spellbook(const creature* target);

int get_thrown(spelln spell); // If differ from -1 spell is range.

boost* find_boost(spelln spell, targetref target);

void camp_autocast();
bool can_cast_spell(int type, int level);
bool can_learn_spell(int type, int level);
void check_boost(unsigned stamp);
bool dispell_magic(spelln spell, targetref target);
void prepare_default_spells();
void random_spells(int level, int count);