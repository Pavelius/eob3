#include "action.h"
#include "answers.h"
#include "creature.h"
#include "game.h"
#include "pushvalue.h"
#include "sound.h"

static actionn action_outcome;

static void leave_outside() {
	action_outcome = LeaveOutside;
}

static void rest_party() {
	pass_time(60 * 8);
}

static void rest_party_inn() {
	if(!confirm(ConfirmRestParty))
		return;
	rest_party();
	leave_outside();
}

static void remove_hunger() {
	player->food = player->getfood();
}

static void eat_and_drink() {
	for_each_party(remove_hunger);
	show_message(EatAndDrinkSuccess);
}

static void gambling() {
	static long source[] = {20, 50, 100, 500, 1000};
	while(running_scene()) {
		if(!check_activity())
			return;
		for(auto n : source) {
			if(game.get(Coins) >= n)
				an.add(n, "%1i", n);
		}
		auto bet = choose_message(GamblingIntro, true);
		if(!bet)
			break;
		pass_activity();
		if(player->roll(Charisma, -4)) {
			game.add(Coins, bet);
			show_message(GamblingWin, bet);
		} else {
			game.add(Coins, -bet);
			show_message(GamblingLose, bet);
		}
	}
}

static void pick_pockets() {
}

static void scrible_scrolls() {
}

static void carousing() {
}

static void repair_weapons() {
	for(auto& e : player->wears) {
		if(!e || !e.isweapon())
			continue;
		if(e.hits > 1) // Repair only if state is not trade-in.
			e.hits = 0;
	}
}

static bool allow_repair_weapons() {
	for(auto& e : player->wears) {
		if(!e || !e.isweapon())
			continue;
		if(e.hits > 1) // Repair only if state is not trade-in.
			return true;
	}
	return false;
}

static void buy_weapons() {
	buy_item(WeaponShop, BuyWeaponsEmpty);
}

static actioni tavern_actions[] = {
	{EatFoodAndDrink, {Coins, 20}, {}, eat_and_drink},
	{PickPocketsAction, {}, {Theif}, pick_pockets},
	{Gambling, {}, {Theif, Fighter, Ranger, Mage}, gambling},
	{Carousing, {Coins, 200}, {}, carousing},
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni inn_actions[] = {
	{RestParty, {}, {}, rest_party_inn},
	{ScribleScrolls, {}, {Mage}, scrible_scrolls},
	{}};
static actioni blacksmith_actions[] = {
	{BuyWeapons, {}, {}, buy_weapons},
	{RepairWeapons, {Coins, 20}, {}, repair_weapons, allow_repair_weapons},
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni temple_actions[] = {
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni wizard_tower_actions[] = {
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni city_actions[] = {
	{GoAdventure},
	{Tavern},
	{Blacksmith},
	{Temple},
	{Inn, {Coins, 30}, {}},
	{WizardTower},
	{}};

static actioni* get_actions(actionn v) {
	switch(v) {
	case NoAction: return city_actions;
	case Tavern: return tavern_actions;
	case Inn: return inn_actions;
	case Blacksmith: return blacksmith_actions;
	case Temple: return temple_actions;
	case WizardTower: return wizard_tower_actions;
	default: return 0;
	}
}

void play_city_actions() {
	auto basic_location = NoAction;
	auto location = basic_location;
	while(true) {
		setv(get_picture(location));
		auto music = get_music(location);
		if(music)
			current_music = music;
		play_city();
		action_outcome = NoAction;
		if(indoor(location)) {
			auto p = choose_building_action(get_actions(location));
			if(!p)
				continue;
			apply_action(p);
		} else {
			auto p = choose_location(get_actions(location));
			if(!p)
				continue;
			if(!pass_payment(p->action, p->required))
				continue;
			location = p->action;
		}
		if(action_outcome == LeaveOutside)
			location = basic_location;
	}
}