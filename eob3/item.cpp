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
	{RightHand, 5, {3}, {Deadly}, {0, 1, 7, Slashing, {1, 8}}}, // BattleAxe
	{RightHand, 5, {7}, {Deadly}, {0, 1, 6, Slashing, {1, 6}}}, // Axe
	{RightHand, 5, {116}, {}, {}}, // Club
	{RightHand, 2, {15}, {Precise}, {}}, // Dagger
	{RightHand, 15, {5}, {}, {}}, // Flail
	{RightHand, 10, {9}, {TwoHanded}, {}}, // Halberd
	{RightHand, 5, {115}, {}, {}}, // WarHammer
	{RightHand, 10, {4}, {}, {}}, // Mace
	{RightHand, 2, {6}, {}, {}}, // Spear
	{RightHand, 0, {8}, {}, {}}, // Staff
	{RightHand, 0, {1}, {}, {}}, // Longsword
	{RightHand, 0, {2}, {}, {}}, // ShortSword
	{RightHand, 0, {42}, {TwoHanded}, {}}, // TwoHandedSword
	{RightHand, 0, {10}, {TwoHanded}, {}}, // Bow
	{RightHand, 0, {18}, {TwoHanded}, {}}, // Sling
	{Body, 0, {32}, {}, {}}, // Robe
	{Body, 0, {17}, {}, {}}, // RedCloack
	{Body, 0, {92}, {}, {}}, // BlueCloack
	{Body, 0, {31}, {}, {}}, // LeatherArmor
	{Body, 0, {30}, {}, {}}, // ScaleMail
	{Body, 0, {29}, {}, {}}, // ChainMail
	{Body, 0, {28}, {}, {}}, // BandedMail
	{Body, 0, {26}, {}, {}}, // PlateMail
	{Head, 0, {20, 6}, {}, {}}, // Helm
	{Head, 0, {73}, {}, {}}, // DwarvenHelm
	{LeftHand, 0, {23}, {}, {}}, // Shield
	{LeftHand, 0, {71}, {}, {}}, // DwarvenShield
	{Legs, 0, {9}, {}, {}}, // Boots
	{Elbow, 0, {25}, {}, {}}, // Bracers
	{LeftRing, 0, {78}, {}, {}}, // BlueRing
	{LeftRing, 0, {79}, {}, {}}, // GreenRing
	{LeftRing, 0, {55}, {}, {}}, // RedRing
	{Neck, 0, {33}, {}, {}}, // Amulet
	{Neck, 0, {34}, {}, {}}, // Medalion
	{Drinkable, 0, {40}, {}, {}}, // BluePotion
	{Drinkable, 0, {41}, {}, {}}, // GreenPotion
	{Drinkable, 0, {39}, {}, {}}, // RedPotion
	{Edible, 0, {37}, {}, {}}, // LargeRation
	{Edible, 0, {38}, {}, {}}, // Ration
	{Readable, 0, {36}, {}, {}}, // MageScroll
	{Readable, 0, {85}, {}, {}}, // PriestScroll
	{Readable, 0, {86}, {}, {}}, // MagicMap
	{Rod, 0, {52}, {}, {}}, // Wand
	{Usable, 0, {54}, {}, {}}, // TheifTools
	{Usable, 0, {117}, {}, {}}, // GrapplingHook
	{Faithable, 0, {53}, {}, {}}, // HolySymbol
	{Faithable, 0, {27}, {}, {}}, // HolySymbolEvil
	{Readable, 0, {35}, {}, {}}, // MageBook 
	{Usable, 0, {59}, {}, {}}, // Horn
	{Backpack, 0, {43}, {}, {}}, // Bones
	{Backpack, 0, {7}, {}, {}}, // MantistHead
	{Backpack, 0, {56}, {}, {}}, // MonsterTeeth
	{Backpack, 0, {89}, {}, {}}, // SkullHead
	{Backpack, 0, {90}, {}, {}}, // SkullBone
	{Backpack, 0, {91}, {}, {}}, // FlameSphere
	{Backpack, 0, {100}, {}, {}}, // IceSphere
	{Backpack, 150, {94}, {}, {}}, // BlueGem
	{Backpack, 300, {95}, {}, {}}, // GreenGem
	{Backpack, 500, {93}, {}, {}}, // RedGem
	{Backpack, 1000, {96}, {}, {}}, // PurpleGem
	{Key, 0, {47}}, // IronKey
	{Key, 0, {46}}, // BronzeKey
	{Key, 0, {48}}, // CooperKey
	{Key, 0, {49}}, // BoneKey
	{Key, 0, {50}}, // ManistKey
	{Key, 0, {58}}, // SteelKey
	{Key, 0, {87}}, // SkullKey
	{Key, 0, {88}}, // MoonKey
	{Key, 0, {102}}, // JewelKey
	{Usable, 0, {60}, {}, {}}, // StoneDagger
	{Usable, 0, {57}, {}, {}}, // StoneGem
	{Neck, 0, {64}, {}, {}}, // StoneAmulet
	{Usable, 0, {61}, {}, {}}, // StoneSphere
	{Usable, 0, {63}, {}, {}}, // StoneHolySymbol
	{Usable, 0, {113}, {}, {}}, // StoneCrest
	{Usable, 0, {105}, {}, {}}, // RedCircle
	{RightHand, 0, {80}, {}, {}}, // ChillTouchHand
	{RightHand, 0, {82}, {}, {}}, // FlameBladeHand
	{RightHand, 0, {}, {}, {}}, // Bite1d6
	{RightHand, 0, {}, {}, {}}, // Claws1d3
	{RightHand, 0, {}, {}, {}}, // Claws1d4
	{RightHand, 0, {}, {}, {}}, // Hag2d4
	{RightHand, 0, {}, {}, {}}, // Mandibules
	{RightHand, 0, {}, {}, {}}, // Slam1d4
	{RightHand, 0, {}, {}, {}}, // Slam1d8
	{RightHand, 0, {}, {}, {}}, // Sting1d8
	{Quiver, 0, {}, {}, {}}, // Arrow
	{Quiver, 0, {}, {}, {}}, // Stone
	{Quiver, 0, {}, {}, {}}, // Dart
};

static_assert(sizeof(item) == sizeof(int));

item* last_item;

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

bool allow(itemn type, wearn n) {
	auto v = item_data[type].wear;
	switch(v) {
	case LeftRing:
	case RightRing:
		return n == LeftRing
			|| n == RightRing;
	case LeftHand:
		return type==Dagger
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