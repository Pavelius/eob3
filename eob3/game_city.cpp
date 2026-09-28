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

static actioni tavern_actions[] = {
	{EatFoodAndDrink, {Coins, 20}, {}, eat_and_drink},
	{PickPocketsSomeone, {}, {Theif}},
	{Gambling, {}, {Theif, Fighter, Ranger, Mage}},
	{Carousing, {Coins, 200}, {}},
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni inn_actions[] = {
	{RestParty, {}, {}, rest_party_inn},
	{ScribleScrolls, {}, {Mage}},
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
	default: return 0;
	}
}

void play_city_actions() {
	auto basic_location = NoAction;
	auto location = basic_location;
	while(true) {
		answer_picture = get_picture(location);
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