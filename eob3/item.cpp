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
	{RightHand, 5, {}, {Deadly}, {}}, // BattleAxe
	{RightHand, 5, {}, {Deadly}, {}}, // Axe
	{RightHand, 5, {}, {}, {}}, // Club
	{RightHand, 2, {}, {Precise}, {}}, // Dagger
	{RightHand, 15, {}, {}, {}}, // Flail
	{RightHand, 10, {}, {TwoHanded}, {}}, // Halberd
	{RightHand, 5, {}, {}, {}}, // WarHammer
	{RightHand, 10, {}, {}, {}}, // Mace
	{RightHand, 2, {}, {}, {}}, // Spear
	{RightHand, 0, {}, {}, {}}, // Staff
	{RightHand, 0, {}, {}, {}}, // Longsword
	{RightHand, 0, {}, {}, {}}, // ShortSword
	{RightHand, 0, {}, {TwoHanded}, {}}, // TwoHandedSword
	{RightHand, 0, {}, {TwoHanded}, {}}, // Bow
	{RightHand, 0, {}, {TwoHanded}, {}}, // Sling
	{Quiver, 0, {}, {}, {}}, // Arrow
	{Quiver, 0, {}, {}, {}}, // Stone
	{Quiver, 0, {}, {}, {}}, // Dart
};

static_assert(sizeof(item) == sizeof(int));

item* last_item;

int get_magic(powern v) {
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