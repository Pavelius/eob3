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
	{Action, LeaveOutside},
	{}};
static actioni city_actions[] = {
	{Action, GoAdventure},
	{Action, Tavern},
	{Action, Blacksmith},
	{Action, Temple},
	{Action, Inn},
	{Action, WizardTower},
	{}};

static picturen get_picture(actionn v) {
	switch(v) {
	case Tavern: return PicTavern;
	case Inn: return PicInn;
	case EatFoodAndDrink: return PicTavern2;
	default: return PicCity;
	}
}

static actioni* get_actions(actionn v) {
	switch(v) {
	case Tavern: return tavern_actions;
	case MainCity: return city_actions;
	default: return 0;
	}
}

static messagen get_confirm(actionn v) {
	switch(v) {
	case EatFoodAndDrink: return ConfirmEatAndDrink;
	default: return (messagen)0;
	}
}

static bool indoor(actionn v) {
	return v >= Tavern && v <= WizardTower;
}

static soundn get_music(actionn v) {
	switch(v) {
	case Inn: return MusInn;
	case MainCity: return MusKvirasim;
	default: return NoMusic;
	}
}

long choose_player_action(const char* cancel) {
	char temp[32]; stringbuilder sb(temp);
	sb.add(message_names[WhatPlayerDo], player->name());
	return choose_large_menu(temp, cancel);
}

bool confirm_message(messagen header, int value) {
	char temp[256]; stringbuilder sb(temp);
	sb.add(getnm(header), value);
	an.add(1, getnm(Agree));
	return show_message(temp, true, getnm(Decline), 0) != 0;
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

static const char* ask_action(actionn v) {
	return getnm(v);
}

static actioni* choose_location(actionn city) {
	for(auto p = get_actions(city); p && *p; p++) {
		if(p->type != Action)
			continue;
		if(p->required && p->required > game)
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

static resultn apply_action(actioni* p) {
	if(p->action == LeaveOutside)
		return ReturnToParent;
	pushvalue push(answer_picture);
	auto picture = get_picture(p->action);
	if(picture)
		answer_picture = picture;
	// Check coins need to expend
	if(p->required.variables[Coins]) {
		auto message = get_confirm(p->action);
		if(message) {
			if(!confirm_message(message, p->required.variables[Coins]))
				return Failed;
		}
		game.variables[Coins] -= p->required.variables[Coins];
	}
	return NoResult;
}

static actioni* choose_building_action(actionn building) {
	for(auto p = get_actions(building); p && *p; p++) {
		if(p->required && p->required > game)
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
			location = p->action;
		}
	}
}