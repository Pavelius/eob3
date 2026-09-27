#include "action.h"
#include "answers.h"
#include "creature.h"
#include "game.h"
#include "pushvalue.h"
#include "sound.h"

static actioni tavern_actions[] = {
	{Action, EatFoodAndDrink, {Coins, 20}, {}},
	{Action, PickPocketsSomeone, {}, {Theif}},
	{Action, Gambling, {}, {Theif, Fighter, Ranger, Mage}},
	{Action, Carousing, {Coins, 200}, {}},
	{Action, LeaveOutside},
	{}};
static actioni inn_actions[] = {
	{Action, RestParty},
	{Action, ScribleScrolls, {}, {Mage}},
	{}};
static actioni city_actions[] = {
	{Action, GoAdventure},
	{Action, Tavern},
	{Action, Blacksmith},
	{Action, Temple},
	{Action, Inn, {Coins, 30}, {}},
	{Action, WizardTower},
	{}};

static picturen get_picture(actionn v) {
	switch(v) {
	case Carousing: return PicTavern2;
	case EatFoodAndDrink: return PicTavern2;
	case Inn: return PicInn;
	case PickPocketsSomeone: return PicPickpockets;
	case Tavern: return PicTavern;
	default: return PicCity;
	}
}

static soundn get_music(actionn v) {
	switch(v) {
	case Inn: return MusInn;
	case MainCity: return MusKvirasim;
	case Tavern: return MusTavern;
	case Temple: return MusTemple;
	default: return NoMusic;
	}
}

static actioni* get_actions(actionn v) {
	switch(v) {
	case Tavern: return tavern_actions;
	case Inn: return inn_actions;
	case MainCity: return city_actions;
	default: return 0;
	}
}

static bool indoor(actionn v) {
	return v >= Tavern && v <= WizardTower;
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
		if(p->type != Action)
			continue;
		if(p->required && !game.enough(p->required))
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

static void rest_party() {
	pass_time(60 * 8);
}

static resultn apply_action(actioni* p) {
	pushvalue push(answer_picture);
	auto result = Successed;
	while(p) {
		auto picture = get_picture(p->action);
		if(picture)
			answer_picture = picture;
		if(!pass_payment(p->action, p->required))
			return Failed;
		switch(p->action) {
		case LeaveOutside: return ReturnToParent;
		case RestParty: rest_party(); return ReturnToParent;
		default: break;
		}
		break;
	}
	return result;
}

static actioni* choose_building_action(actionn building) {
	for(auto p = get_actions(building); p && *p; p++) {
		if(p->required && !game.enough(p->required))
			continue; // Can't pay or other reputation
		if(!pass_restriction(p->restriction))
			continue; // Not pass restriction
		an.add((long)p, getnm(p->action));
	}
	return (actioni*)choose_player_action(getnm(Cancel));
}

void play_city_actions() {
	auto basic_location = MainCity;
	auto location = basic_location;
	while(true) {
		answer_picture = get_picture(location);
		auto music = get_music(location);
		if(music)
			current_music = music;
		play_city();
		if(indoor(location)) {
			auto p = choose_building_action(location);
			if(!p)
				continue;
			auto result = apply_action(p);
			if(result == ReturnToParent)
				location = basic_location;
		} else {
			auto p = choose_location(location);
			if(!p)
				continue;
			if(!pass_payment(p->action, p->required))
				continue;
			location = p->action;
		}
	}
}