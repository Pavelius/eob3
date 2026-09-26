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

enum damagen : unsigned char {
	Bludgeon, Slashing, Piercing,
	Magic, Fire, Cold, Acid, Shock, Poison,
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
	LargeRation, Ration,
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
	RandomItem, RandomSmallItem,
	RandomRation,
	RandomTreasure,
};
enum featn : unsigned char {
	NoPower, Magical, Magical2, Magical3, Magical4, Magical5, Cursed, Delusion,
	Protection, Flaming, Freezing,
	TwoHanded, Deadly, Precise,
	Invisibled, SeeMagical, SeeCursed,
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

extern const char* item_names[LastItem + 1];

struct featc {
	unsigned data = 0;
	featc() = default;
	template<typename... Ts> constexpr featc(featn v, Ts... args) : featc(args...) { set(v); }
	bool is(featn v) const { return (data & (1 << v)) != 0; }
	void set(featn v) { data |= (1 << v); }
};
struct combati {
	char		attack, number_attacks, speed;
	damagen		type;
	dice		damage, large;
	itemn		ammo;
};
struct itemi {
	struct avatari {
		unsigned char pack, ground, thrown;
	};
	struct defencei {
		char	ac;
	};
	wearn		wear;
	int			cost;
	avatari		avatar;
	featc		flags;
	combati		combat;
	defencei	defence;
};
extern itemi item_data[LastItem + 1];

int get_chance_identify(itemn v);
int get_magic(featn v);

bool allow(itemn type, wearn n);

struct item {
	itemn		type = NoItem;
	featn		power = NoPower; // Special additional magical powers
	purposen	purpose = CommonItem; // Special purpose of item (depends on mission or quest)
	union {
		struct {
			unsigned char identified : 1;
			unsigned char hits : 3; // 0 - undamaged, 7 - is almost broken.
		};
		unsigned char count = 0;
	};
	constexpr item() = default;
	constexpr item(itemn type) : type(type) {}
	constexpr explicit operator bool() const { return type != 0; }
	constexpr const itemi& geti() const { return item_data[type]; }
	constexpr bool countable() const { return type >= Arrow; }
	const char*	name() const { return item_names[type]; }
	bool allow(wearn v) const;
	void clear() { type = NoItem; power = NoPower; purpose = CommonItem; count = 0; }
	void consume() { setcount(getcount() - 1); }
	void createpower(char magic_bonus, int chance_magical, int chance_cursed) {}
	void damage(const char* interactive, int use) {}
	void identify(int v) { identified = (v >= 0) ? 1 : 0; }
	bool is(featn v) const { return power == v || geti().flags.is(v); }
	bool is(purposen v) const { return purpose == v; }
	bool is(wearn v) const { return geti().wear == v; }
	bool isartifact() const { return get_magic(power) >= 4; }
	bool iscursed() const { return (power == Cursed || power == Delusion); }
	bool isdamaged() const { return !countable() && count >= 5; }
	bool isidentified() const { return identified != 0; }
	bool ismagical() const { return power != NoPower; }
	bool isranged() const { return geti().avatar.thrown || geti().combat.ammo != NoItem; }
	bool isweapon() const { return geti().combat.damage.c != 0; }
	bool join(item& it);
	int	getcost() const;
	int	getcount() const { return countable() ? count + 1 : 1; }
	int	getmagic() const { return get_magic(power); }
	featn getpower() const { return power; }
	void set(featn v) { power = v; }
	void set(purposen v) { purpose = v; }
	void setcount(int v);
	void usecharge(const char* interactive, int chance = 35, int use = 1); // Maximum charges is always 10
};
extern item* last_item;

struct wearable {
	item		wears[LastBelt + 1];
	void		additem(item& v);
	slice<item> backpack() { return slice<item>(wears + Backpack, wears + LastBackpack + 1); }
	slice<item> beltslots() { return slice<item>(wears + FirstBelt, wears + LastBelt + 1); }
	slice<item> equipment() { return slice<item>(wears + Head, wears + Legs + 1); }
	item*		freebelt();
	item*		freebackpack();
	void		putbelt(item& v);
	void		shrinkbelt();
};

itemn random(itemn v);