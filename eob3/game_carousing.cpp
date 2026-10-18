#include "action.h"
#include "answers.h"
#include "creature.h"
#include "game.h"
#include "pushvalue.h"
#include "rand.h"

static bool is_dwarf_or_warrior() {
	return player->iswarrior() || player->is(Dwarf);
}

static item create_item(itemn type, int chance_magic = 0, int chance_cursed = 0) {
	item it(random(type));
	it.create(chance_magic, chance_cursed);
	return it;
}

static void great_pick_pockets() {
	if(player->roll(PickPockets)) {
		auto coins = xrand(4, 24) * 20 * gp;
		auto jackpot = create_item(RandomTavernPickPocket, 20, 20);
		addv(Coins, coins);
		player->additem(jackpot);
		show_message(GreatPickPocketsSuccess, coins, jackpot.name());
	} else {
		auto coins = xrand(1, 6) * 20 * gp;
		show_message(GreatPickPocketsFail, coins);
	}
}

static void great_gambling() {
	if(player->roll(Charisma, -2)) {
		auto coins = xrand(4, 24) * 20 * gp;
	} else {
	}
}

static void drink_contest() {
	if(player->roll(Constitution, -1)) {
	} else {
	}
}

static actioni carousing_actions[] = {
	{GreatPickPockets, {}, {Theif}, great_pick_pockets},
	{GreatGambling, {}, {}, great_gambling},
	{DrinkContest, {}, {}, drink_contest, is_dwarf_or_warrior},
};

static void select_actions(actioni** result) {
	auto p = result;
	for(auto& e : carousing_actions)
		*p++ = &e;
	zshuffle(result, lengthof(carousing_actions));
}

void carousing() {
	actioni* actions[lengthof(carousing_actions)];
	select_actions(actions);
	pushvalue push(player);
	pushvalue push_picture(answer_picture);
	unsigned index = 0;
	for(auto p : adventurers) {
		if(!p || p->isdisabled())
			continue;
		player = p;
		auto pa = pick_action(actions, lengthof(carousing_actions), index);
		if(!pa)
			continue;
		setv(get_picture(pa->action));
		show_message(pa->action);
		if(pa->proc)
			pa->proc();
	}
}
