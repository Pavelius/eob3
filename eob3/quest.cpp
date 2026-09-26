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

	QUEST MESSAGES:

	  0		Introducion npc can tell about quest. In the end button `Agreet`.
	  1		When use press `Agreet`.
	  2		Adventures to quest dungeon location.
	  3		Arrives to quest dungeon location.
	  4		Arrives to quest dungeon location.

*/

#include "dungeon.h"
#include "game.h"
#include "quest.h"

static sitei flooded_collectors[] = {
	{BRICK, Human, 2, {Kobold, Leech}, NoMonster, {Lair, TrappedCorridor}},
	{BRICK, Human, 1, {Kobold, Leech}, NoMonster, {GreatLair, Lair}},
};

questi quests[] = {
	{0, {GoldCoins, 500, Reputation, 1}, flooded_collectors},
};

int quest_count(questfn v) {
	auto result = 0;
	for(auto& e : quests) {
		if(e.state.is(v))
			result++;
	}
	return result;
}