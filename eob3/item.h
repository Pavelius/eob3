/*
	Copyright 2026 by Pavel Chistyakov

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.77
*/

#pragma once

#include "dice.h"
#include "slice.h"

const int gp = 10; // 1 gold piece = 1 silver coin

enum spelln : unsigned char;

enum featn : unsigned char {
	NoPower, Magical, Magical2, Magical3, Magical4, Magical5, Cursed, Delusion,
	Protection, Flaming, Freezing,
	ControlHuman, ControlGoblinoid, ControlEvil,
	TwoHanded, Deadly, Disease, Poison, Precise,
	Alertness, Healing, Sneaky,
	DrainEnergy, DrainStrenght, DispelEvil, Holy, Vampiric, Vorpal,
	ImmuneIllusion, ImmuneNormalWeapon, ImmuneDisease,
	ResistFire, ResistCold, ResistBludgeon, ResistPiercing, ResistSlashing,
	Blinked, Blind, Blurred, Displaced, Invisible, Paralizing, Regenerated, StoppedPoison,
	SeeMagical, SeeCursed,
	Surprised, Painful, Panic, Moved, SlowMove, Undead,
	LastFeat = Undead,
	SwordPower, MeleeWeaponPower, RangedWeaponPower,
};
enum damagen : unsigned char {
	Bludgeon, Slashing, Piercing,
	FireDamage, ColdDamage, AcidDamage, ShockDamage,
	MindDamage, ForceDamage, PoisonDamage, IllDamage, HealthDamage,
};
enum itemn : unsigned char {
	NoItem,
	BattleAxe, Axe, Club, Dagger, Flail, Halberd, WarHammer, Mace, Spear, Staff,
	Longsword, ShortSword, TwoHandedSword,
	Bow, Sling,
	Robe, RedCloack, BlueCloack,
	LeatherArmor, ScaleMail, ChainMail, BandedMail, PlateMail,
	Helm, DwarvenHelm,
	Shield, DwarvenShield, Boots, Bracers,
	BlueRing, GreenRing, RedRing, Amulet, Medalion,
	BluePotion, GreenPotion, RedPotion,
	RationIron, Ration,
	MageScroll, PriestScroll, MagicMap, Wand,
	TheifTools, GrapplingHook, HolySymbol, HolySymbolEvil, MageBook, Horn,
	Bones, MantistHead, MonsterTeeth, SkullHead, SkullBone,
	FlameSphere, IceSphere,
	BlueGem, GreenGem, RedGem, PurpleGem,
	IronKey, BronzeKey, CooperKey, BoneKey, ManistKey, SteelKey, SkullKey, MoonKey, JewelKey,
	StoneDagger, StoneGem, StoneAmulet, StoneSphere, StoneHolySymbol, StoneCrest,
	RedCircle,
	ChillTouchHand, FlameBladeHand,
	Bite1d6, Claws1d3, Claws1d4, Hag2d4, Mandibules, Slam1d4, Slam1d8, Sting1d8,
	Arrow, Stone, Dart,
	LastItem = Dart,
	RandomItem, RandomSmallItem, RandomWeapon,
	RandomRation, RandomRing,
	RandomTreasure,
};
enum purposen : unsigned char {
	CommonItem, SummonedItem, ToolItem, QuestItem, NaturalItem,
};
enum wearn : unsigned char {
	Backpack, Edible, Drinkable, Readable, Usable, Key, Rod, Faithable, LastBackpack = Backpack + 13,
	Head, Neck, Body, RightHand, LeftHand, RightRing, LeftRing, Elbow, Legs, Quiver,
	FirstBelt, SecondBelt, LastBelt,
	FirstInvertory = Backpack, LastInvertory = LastBelt
};
enum shopn : unsigned char {
	WeaponShop, DwarvenWeaponShop, MagicShop,
	LastShop = MagicShop
};
enum trapn : unsigned char {
	ArrowTrap,
	LastTrap = ArrowTrap
};

extern const char* item_names[LastItem + 1];

bool is_large(itemn type);
bool is_natural(itemn type);
bool is_small(itemn type);

struct featc {
	constexpr static const unsigned b = 32;
	unsigned data[(LastFeat + b - 1) / b] = {};
	featc() = default;
	template<typename... Ts> constexpr featc(featn v, Ts... args) : featc(args...) { set(v); }
	constexpr bool is(featn v) const { return (data[v / b] & (1 << (v % b))) != 0; }
	constexpr void remove(featn v) { data[v / b] &= ~(1 << (v % b)); }
	constexpr void set(featn v) { data[v / b] |= (1 << (v % b)); }
};

struct combati {
	char		attack, number_attacks, speed;
	damagen		type;
	dice		damage, large;
	itemn		ammo;
	featn		effect; // Additional effect (for traps)
};
extern combati traps[LastTrap + 1];

struct itemi {
	struct avatari {
		short	pack = -1, ground = -1, thrown = -1;
	};
	struct defencei {
		char	ac = 0;
		char	deflect = 0;
	};
	wearn		wear = Backpack;
	int			cost = 0;
	avatari		avatar = {};
	featc		flags = {};
	combati		combat = {};
	defencei	defence = {};
	itemn index() const;
};
extern itemi item_data[LastItem + 1];

int get_chance_identify(itemn v);
int get_magic(featn v);

bool allow(itemn type, wearn n);

featn get_powers(itemn type);

struct item {
	itemn			type = NoItem;
	featn			power = NoPower; // Special additional magical powers
	purposen		purpose = CommonItem; // Special purpose of item (depends on mission or quest)
	unsigned char	identified : 1 = 0;
	unsigned char	hits : 3 = 0; // 0 - undamaged, 7 - is almost broken.
	item() = default;
	constexpr item(itemn type) : type(type) {}
	constexpr explicit operator bool() const { return type != 0; }
	constexpr const itemi& geti() const { return item_data[type]; }
	const char*	name() const { return item_names[type]; }
	bool allow(wearn v) const;
	bool allow(spelln v, int level);
	bool apply(spelln v, int level, bool run);
	void clear() { type = NoItem; power = NoPower; purpose = CommonItem; hits = 0; identified = 0; }
	void consume() {}
	void create(int chance_magical, int chance_cursed = 5);
	void damage(const char* interactive, int use) {}
	void identify(int v) { identified = (v >= 0) ? 1 : 0; }
	bool is(featn v) const { return power == v || geti().flags.is(v); }
	bool is(purposen v) const { return purpose == v; }
	bool is(wearn v) const { return geti().wear == v; }
	bool is(itemn v) const { return type == v; }
	bool isartifact() const { return get_magic(power) >= 4; }
	bool iscursed() const { return (power == Cursed || power == Delusion); }
	bool isdamaged() const { return hits >= 5; }
	bool isidentified() const { return identified != 0; }
	bool ismagical() const { return power != NoPower; }
	bool isranged() const { return geti().avatar.thrown != -1 || geti().combat.ammo != NoItem; }
	bool isweapon() const { return geti().combat.damage.c != 0; }
	bool natural() const { return is_natural(type); }
	int	getcount() const { return 1; }
	int	getcost() const { return geti().cost; }
	int	getmagic() const { return get_magic(power); }
	featn getpower() const { return power; }
	void set(featn v) { power = v; }
	void set(purposen v) { purpose = v; }
	void setcount(int v) {}
	void usecharge(const char* interactive, int chance = 35);
};

extern item shops[LastShop + 1][6];

struct wearable {
	item		wears[LastBelt + 1];
	void		addgear(itemn type, purposen purpose, featn power = NoPower);
	void		additem(item& it);
	slice<item> backpack() { return slice<item>(wears + Backpack, wears + LastBackpack + 1); }
	slice<item> beltslots() { return slice<item>(wears + FirstBelt, wears + LastBelt + 1); }
	slice<item> equipment() { return slice<item>(wears + Head, wears + Legs + 1); }
	item*		freebelt();
	item*		freebackpack();
	void		putbelt(item& v);
	void		shrinkbelt();
	bool		haveitem(const void* p) const { return p >= wears && p <= wears + sizeof(wears) / sizeof(wears[0]); }
	bool		haveitem(itemn v) const { for(auto& e : wears) if(e.type == v) return true; return false; }
};

itemn random(itemn v);
featn random(featn v);

void addv(item* shop, item& v);
bool allow(shopn v);
bool is_identified(const void* object);
void normalize_shop(shopn id);
void refresh_shop(shopn id);
void refresh_shops();