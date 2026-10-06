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

#include "action.h"
#include "creature.h"
#include "dice.h"
#include "math.h"
#include "rand.h"
#include "stringbuilder.h"
#include "stringvar.h"

static void addv(stringbuilder& sb, const dice& v) {
	sb.add("%1id%2i", v.c, v.d);
	if(v.b)
		sb.add("%+1i", v.b);
}

static void player_name(stringbuilder& sb) {
	sb.add(player->name());
}

static void opponent_name(stringbuilder& sb) {
	sb.add(opponent->name());
}

static void player_class(stringbuilder& sb) {
	sb.add(class_names[player->type]);
}

static void player_weapon(stringbuilder& sb) {
	sb.add(player->wears[RightHand].name());
}

static void player_gold(stringbuilder& sb) {
	sb.add("%1i", game.variables[Coins]);
}

void stringbuilder_custom(stringbuilder& sb, const char* id) {
	if(stringvar_identifier(sb, id))
		return;
	default_string(sb, id);
}

stringvari stringvars[] = {
	{"Class", player_class},
	{"Gold", player_gold},
	{"Name", player_name},
	{"Opponent", opponent_name},
	{"Weapon", player_weapon},
	{}};