#include "action.h"
#include "answers.h"
#include "creature.h"
#include "game.h"
#include "sound.h"

static picturen get_picture(messagen v) {
	switch(v) {
	case Tavern: return PicTavern;
	case Inn: return PicInn;
	default: return PicCity;
	}
}

static soundn get_music(messagen v) {
	return MusKvirasim;
}

long choose_player_action(const char* cancel) {
	char temp[32]; stringbuilder sb(temp);
	sb.add(message_names[WhatPlayerDo], player->name());
	return choose_large_menu(temp, cancel);
}

static int make_payment(messagen header, messagen ask, int multiply, int maximum, const char* cancel) {
	for(auto i = 1; i < maximum; i++) {
		auto need_coins = i * multiply;
		if(game.get(GoldCoins) >= need_coins)
			an.add(1, getnm(ask), i, need_coins, multiply);
	}
	auto result = choose_large_menu(getnm(header), cancel);
	if(!result)
		return 0;
	auto need_coins = result * multiply;
	game.add(GoldCoins, -need_coins);
	return result;
}

static actioni tavern_actions[] = {
	{BasicAction, EatFoodAndDrink, {}, {}},
	{BasicAction, {}, {Theif}},
	{BasicAction, {}, {Theif, Fighter, Ranger, Mage}},
};

static messagen choose_city_location() {
	an.add(GoAdventure, getnm(GoAdventure));
	an.add(Tavern, getnm(Tavern));
	an.add(Blacksmith, getnm(Blacksmith));
	an.add(Temple, getnm(Temple));
	an.add(Inn, getnm(Inn));
	an.add(WizardTower, getnm(WizardTower));
	return (messagen)choose_large_menu_no_player(getnm(WhichWayToGo), getnm(Cancel));
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

static void apply_action(messagen message) {
}

static messagen choose_action(messagen building) {
	//for(auto& e : actions) {
	//	if(e.required && e.required > game)
	//		continue; // Can't pay or other reputation
	//	if(!pass_restriction(e.restriction))
	//		continue; // Not pass restriction
	//	an.add(e.action, getnm(e.action));
	//}
	return (messagen)choose_player_action(getnm(Cancel));
}

void play_city_actions() {
	auto building = Cancel;
	while(true) {
		answer_picture = get_picture(building);
		current_music = get_music(building);
		play_city();
		if(!building) {
			auto result = choose_city_location();
			if(!result)
				continue;
			building = result;
		} else {
			auto result = choose_action(building);
			if(!result)
				continue;
			apply_action(result);
		}
	}
}