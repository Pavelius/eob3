#pragma once

#include "color.h"

const unsigned maximum_spells = 120;

enum abilityn : unsigned char;
enum celln : unsigned char;
enum damagen : unsigned char;
enum featn : unsigned char;
enum spelln : unsigned char;

struct creature;
struct item;

typedef bool(*fncondition)();
typedef void(*fnevent)();
typedef void(*fninstant)(int value);

enum spellfn : unsigned char {
	You, Ally, AllAlly,
	Enemy, AllEnemy,
	AllyItems, AllAllyItems,
	SummonWeapon,
};

/*struct spellfc {
	unsigned data = 0;
	spellfc() = default;
	template<typename... Ts> constexpr spellfc(spellfn v, Ts... args) : spellfc(args...) { set(v); }
	bool is(spellfn v) const { return (data & (1 << v)) != 0; }
	void set(spellfn v) { data |= (1 << v); }
};*/

struct spelli {
	char			levels[2]; // 0 - priest, 1 - mage, 2 - other
	spellfn			type; // Effect type 
	fninstant		instant; // When spell use
	fncondition		test; // Target test
	color			lighting; // Active color border hilite
};
extern spelli spells[maximum_spells];

struct targetref {
	unsigned char	type = 0xFF; // 0..250 is dungeon index for monsters, 0xFE is character.
	unsigned char	index = 0; // Index of target in array.
	targetref() = default;
	targetref(const creature* p);
	constexpr explicit operator bool() const { return type != 0xFF; }
	constexpr bool operator ==(const targetref& v) const { return type == v.type && v.index == v.index; }
	constexpr bool operator !=(const targetref& v) const { return type != v.type || v.index != v.index; }
	operator creature*() const;
	void clear() { type = 0xFF; index = 0; }
};

struct spellboost {
	spelln			spell; // Spell index
	targetref		target;
	unsigned		stop; // Stop when time will be this.
	fnevent			proc; // Boost wearing function
	featn			feat; // Additional feat
};
extern spellboost spellboosts[256];
extern unsigned char spellboost_count;

struct spellbook {
	char			spells[maximum_spells];
};

void apply(fnevent proc, unsigned duration, featn feat = (featn)0);
void apply(damagen type, int value, abilityn save, bool save_ignore = false);
void apply(damagen type, int value);
bool cast(spelln spell);
bool cast(spelln spell, creature* target);
void summon(itemn type, unsigned duration);