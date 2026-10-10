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

#include "item.h"

itemi item_data[LastItem + 1] = {
	{}, // No item
	{RightHand, 5, {3, 4}, {Deadly}, {0, 1, 7, Slashing, {1, 8}}}, // BattleAxe
	{RightHand, 5, {7, 4}, {Deadly}, {0, 1, 4, Slashing, {1, 6}}}, // Axe
	{RightHand, 5, {116, 12}, {}, {0, 1, 5, Bludgeon, {1, 6}}}, // Club
	{RightHand, 2, {15, 3}, {Precise}, {0, 1, 2, Piercing, {1, 4}}}, // Dagger
	{RightHand, 15, {5, 2}, {}, {0, 1, 7, Bludgeon, {1, 6, 1}}}, // Flail
	{RightHand, 10, {9, 5}, {TwoHanded, Deadly}, {0, 1, 9, Slashing, {1, 10}}}, // Halberd
	{RightHand, 5, {115, 13}, {}, {0, 1, 4, Bludgeon, {1, 4, 2}}}, // WarHammer
	{RightHand, 10, {4, 1}, {}, {0, 1, 7, Bludgeon, {1, 6, 1}}}, // Mace
	{RightHand, 2, {6, 3}, {TwoHanded}, {0, 1, 6, Piercing, {1, 6}}}, // Spear
	{RightHand, 0, {8, 3}, {}, {0, 1, 6, Bludgeon, {1, 6}}}, // Staff
	{RightHand, 0, {1, 0}, {}, {0, 1, 5, Slashing, {1, 8}}}, // Longsword
	{RightHand, 0, {2, 0}, {}, {0, 1, 4, Slashing, {1, 6}}}, // ShortSword
	{RightHand, 0, {42, 0}, {TwoHanded}, {0, 1, 10, Slashing, {1, 10}, {3, 6}}}, // TwoHandedSword
	{RightHand, 0, {10, 6}, {TwoHanded}, {}}, // Bow
	{RightHand, 0, {18, 4}, {TwoHanded}, {}}, // Sling
	{Body, 0, {32, 8}, {}, {}}, // Robe
	{Body, 0, {17, 8}, {}, {}}, // RedCloack
	{Body, 0, {92, 8}, {}, {}}, // BlueCloack
	{Body, 0, {31, 11}, {}, {}}, // LeatherArmor
	{Body, 0, {30, 9}, {}, {}}, // ScaleMail
	{Body, 0, {29, 9}, {}, {}}, // ChainMail
	{Body, 0, {28, 9}, {}, {}}, // BandedMail
	{Body, 0, {26, 9}, {}, {}}, // PlateMail
	{Head, 0, {20, 6}, {}, {}}, // Helm
	{Head, 0, {73, 6}, {}, {}}, // DwarvenHelm
	{LeftHand, 0, {23, 7}, {}, {}}, // Shield
	{LeftHand, 0, {71, 7}, {}, {}}, // DwarvenShield
	{Legs, 0, {9, 9}, {}, {}}, // Boots
	{Elbow, 0, {25, 16}, {}, {}}, // Bracers
	{LeftRing, 0, {78, 15}, {}, {}}, // BlueRing
	{LeftRing, 0, {79, 15}, {}, {}}, // GreenRing
	{LeftRing, 0, {55, 15}, {}, {}}, // RedRing
	{Neck, 0, {33, 26}, {}, {}}, // Amulet
	{Neck, 0, {34, 26}, {}, {}}, // Medalion
	{Drinkable, 0, {40, 19}, {}, {}}, // BluePotion
	{Drinkable, 0, {41, 19}, {}, {}}, // GreenPotion
	{Drinkable, 0, {39, 19}, {}, {}}, // RedPotion
	{Edible, 0, {37, 14}, {}, {2}}, // LargeRation
	{Edible, 0, {38, 14}, {}, {}}, // Ration
	{Readable, 0, {36, 12}, {}, {}}, // MageScroll
	{Readable, 0, {85, 12}, {}, {}}, // PriestScroll
	{Readable, 0, {86, 12}, {}, {}}, // MagicMap
	{Rod, 0, {52, 10}, {}, {}}, // Wand
	{Usable, 0, {54, 1}, {}, {}}, // TheifTools
	{Usable, 0, {117, 14}, {}, {}}, // GrapplingHook
	{Faithable, 0, {53, 20}, {}, {}}, // HolySymbol
	{Faithable, 0, {27, 20}, {}, {}}, // HolySymbolEvil
	{Readable, 0, {35, 11}, {}, {}}, // MageBook
	{Usable, 0, {59, 22}, {}, {}}, // Horn
	{Backpack, 0, {43, 7}, {}, {}}, // Bones
	{Backpack, 0, {51, 7}, {}, {}}, // MantistHead
	{Backpack, 0, {56, 7}, {}, {}}, // MonsterTeeth
	{Backpack, 0, {89, 7}, {}, {}}, // SkullHead
	{Backpack, 0, {90, 7}, {}, {}}, // SkullBone
	{Backpack, 0, {91, 18}, {}, {}}, // FlameSphere
	{Backpack, 0, {100, 18}, {}, {}}, // IceSphere
	{Backpack, 150, {94, 22}, {}, {}}, // BlueGem
	{Backpack, 300, {95, 22}, {}, {}}, // GreenGem
	{Backpack, 500, {93, 22}, {}, {}}, // RedGem
	{Backpack, 1000, {96, 22}, {}, {}}, // PurpleGem
	{Key, 0, {47, 8}}, // IronKey
	{Key, 0, {46, 8}}, // BronzeKey
	{Key, 0, {48, 8}}, // CooperKey
	{Key, 0, {49, 8}}, // BoneKey
	{Key, 0, {50, 8}}, // ManistKey
	{Key, 0, {58, 8}}, // SteelKey
	{Key, 0, {87, 8}}, // SkullKey
	{Key, 0, {88, 8}}, // MoonKey
	{Key, 0, {102, 8}}, // JewelKey
	{Usable, 0, {60, 3}, {}, {}}, // StoneDagger
	{Usable, 0, {57, 22}, {}, {}}, // StoneGem
	{Neck, 0, {64, 26}, {}, {}}, // StoneAmulet
	{Usable, 0, {61, 18}, {}, {}}, // StoneSphere
	{Usable, 0, {63, 20}, {}, {}}, // StoneHolySymbol
	{Usable, 0, {113, 20}, {}, {}}, // StoneCrest
	{Usable, 0, {105, 21}, {}, {}}, // RedCircle
	{RightHand, 0, {80}, {}, {}}, // ChillTouchHand
	{RightHand, 0, {82}, {}, {}}, // FlameBladeHand
	{RightHand, 0, {}, {}, {}}, // Bite1d6
	{RightHand, 0, {119}, {}, {}}, // Claws1d3
	{RightHand, 0, {119}, {}, {}}, // Claws1d4
	{RightHand, 0, {119}, {}, {}}, // Hag2d4
	{RightHand, 0, {51}, {}, {}}, // Mandibules
	{RightHand, 0, {83}, {}, {}}, // Slam1d4
	{RightHand, 0, {83}, {}, {}}, // Slam1d8
	{RightHand, 0, {51}, {}, {}}, // Sting1d8
	{Quiver, 0, {16, 5}, {}, {}}, // Arrow
	{Quiver, 0, {19, 2}, {}, {}}, // Stone
	{Quiver, 0, {14, 0}, {}, {}}, // Dart
};

combati traps[LastTrap + 1] = {
	{0, 1, 13, Piercing, {1, 6}, {1, 6}, Arrow},
};

static_assert(sizeof(item) == sizeof(int));

int get_magic(featn v) {
	switch(v) {
	case NoPower: return 0;
	case Magical: return 1;
	case Magical2: return 2;
	case Magical3: return 3;
	case Magical4: return 4;
	case Magical5: return 5;
	case Cursed: return -1;
	case Delusion: return -2;
	default: return 1;
	}
}

int get_chance_identify(itemn v) {
	return 0; // No additional bonuses
}

bool is_large(itemn type) {
	switch(type) {
	case Longsword: case ShortSword: case TwoHandedSword:
	case Mace: case Flail: case Staff: case Spear:
	case Axe: case BattleAxe: case Halberd:
	case Shield: case DwarvenShield:
	case LeatherArmor: case ChainMail: case PlateMail: case ScaleMail: case BandedMail: case Robe:
	case WarHammer: case Club: case GrapplingHook:
		return true;
	default:
		return false;
	}
}

bool is_small(itemn type) {
	switch(type) {
	case TheifTools:
	case MageBook: case PriestScroll: case MageScroll: case MagicMap:
	case BlueGem: case GreenGem: case RedGem: case PurpleGem:
	case Wand: case BlueRing: case RedRing: case GreenRing: case Amulet:
	case IceSphere: case FlameSphere:
	case IronKey: case BronzeKey: case CooperKey: case BoneKey: case SteelKey: case SkullKey: case MoonKey: case JewelKey:
	case Ration: case RationIron: case Stone: case Arrow: case Dart:
		return true;
	default:
		return false;
	}
}

bool is_natural(itemn type) {
	return type >= ChillTouchHand && type <= Sting1d8;
}

bool is_identified(const void* object) {
	return ((item*)object)->identified != 0;
}

bool allow(itemn type, wearn n) {
	auto v = item_data[type].wear;
	switch(v) {
	case LeftRing:
	case RightRing:
		return n == LeftRing
			|| n == RightRing;
	case LeftHand:
		return type == Dagger
			|| n == LeftHand
			|| n == Rod
			|| n == Readable
			|| n == Faithable
			|| n == Drinkable;
	case FirstBelt: case SecondBelt: case LastBelt:
		return n == RightHand;
	default:
		if(v >= Backpack && v <= LastBackpack)
			return true;
		return n == v;
	}
}

void addv(item* shop, item& it) {
	for(size_t i = 0; i < lengthof(shops[0]); i++) {
		if(!it)
			break;
		if(!shop[i]) {
			shop[i] = it;
			it.clear();
		}
	}
}

itemn itemi::index() const {
	return (itemn)(this - item_data);
}

void wearable::addgear(itemn type, purposen purpose, featn power) {
	item e(type);
	e.create(0, 0);
	e.purpose = purpose;
	e.power = power;
	additem(e);
}

void wearable::additem(item& it) {
	for(auto& e : backpack()) {
		if(!e) {
			e = it;
			it.clear();
			break;
		}
	}
}

bool item::allow(wearn v) const {
	auto n = geti().wear;
	switch(v) {
	case LeftRing:
	case RightRing:
		return n == LeftRing
			|| n == RightRing;
	case LeftHand:
		return (n == RightHand && is(Precise))
			|| n == LeftHand
			|| n == Rod
			|| n == Readable
			|| n == Faithable
			|| n == Drinkable;
	case FirstBelt: case SecondBelt: case LastBelt:
		return n == RightHand;
	default:
		if(v >= Backpack && v <= LastBackpack)
			return true;
		return n == v;
	}
}