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

static void eat_and_drink() {
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

static int make_payment(messagen header, messagen ask, int multiply, int maximum, const char* cancel) {
	for(auto i = 1; i < maximum; i++) {
		auto need_coins = i * multiply;
		if(game.get(Coins) >= need_coins)
			an.add(1, getnm(ask), i, need_coins, multiply);
	}
	auto result = choose_large_menu(getnm(header), cancel);
	if(!result)
		return 0;
	auto need_coins = result * multiply;
	game.add(Coins, -need_coins);
	return result;
}

static actioni* choose_location(actionn city) {
	for(auto p = get_actions(city); p && *p; p++) {
		if(p->required && !enough(game, p->required))
			continue; // Can't pay or other reputation
		if(p->action >= Tavern && p->action <= WizardTower)
			an.add((long)p, getnm(VisitBuilding), getnm(p->action));
		else
			an.add((long)p, getnm(p->action));
	}
	return (actioni*)choose_large_menu_no_player(getnm(WhichWayToGo), getnm(Cancel));
}

static bool pass_restriction(const classnc& v) {
	if(!v)
		return true;
	auto n = get_class_count(player->type);
	for(auto i = 0; i < n; i++) {
		if(v.is(get_class(player->type, i)))
			return true;
	}
	return false;
}

static void apply_action(actioni* p) {
	pushvalue push(answer_picture);
	auto picture = get_picture(p->action);
	if(picture)
		answer_picture = picture;
	if(!pass_payment(p->action, p->required))
		return;
	if(p->success)
		p->success();
}

static actioni* choose_building_action(actionn building) {
	for(auto p = get_actions(building); p && *p; p++) {
		if(p->required && !enough(game, p->required))
			continue; // Can't pay or other reputation
		if(!pass_restriction(p->restriction))
			continue; // Not pass restriction
		an.add((long)p, getnm(p->action));
	}
	return (actioni*)choose_player_action(getnm(Cancel));
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
			auto p = choose_building_action(location);
			if(!p)
				continue;
			apply_action(p);
		} else {
			auto p = choose_location(location);
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