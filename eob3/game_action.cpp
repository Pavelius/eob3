#include "action.h"
#include "answers.h"
#include "creature.h"
#include "game.h"
#include "pushvalue.h"
#include "rand.h"
#include "stringbuilder.h"

variablei game;

static messagen get_confirm(actionn v) {
	switch(v) {
	case Carousing: return ConfirmCarousing;
	case Inn: return ConfirmRentRoom;
	case EatFoodAndDrink: return ConfirmEatAndDrink;
	default: return (messagen)0;
	}
}

picturen get_picture(actionn v) {
	switch(v) {
	case Blacksmith: return PicForge;
	case Carousing: return PicTavern2;
	case EatFoodAndDrink: return PicTavern2;
	case Gambling: return PicGambling;
	case Inn: return PicInn;
	case PickPocketsAction: return PicPickpockets;
	case Tavern: return PicTavern;
	default: return PicCity;
	}
}

soundn get_music(actionn v) {
	switch(v) {
	case NoAction: return MusKvirasim;
	case Blacksmith: return MusSmith;
	case Inn: return MusInn;
	case Tavern: return MusTavern;
	case Temple: return MusTemple;
	case WizardTower: return MusHealer;
	default: return NoMusic;
	}
}

bool need_activity(actionn v) {
	switch(v) {
	case PickPocketsAction: case Gambling: case ScribleScrolls:
		return true;
	default:
		return false;
	}
}

bool indoor(actionn v) {
	return v >= Tavern && v <= WizardTower;
}

bool check_activity() {
	if(player->food <= 6) {
		show_message(PlayerExhaused);
		return false;
	}
	pass_time(xrand(20, 40));
	auto roll = d20();
	auto efforts = 10; // Maximum effort consumed
	// AD&D standart success roll must be lesser that ability
	if(roll < player->abilities[Constitution]) {
		efforts -= player->abilities[Constitution] - roll;
		if(efforts < 1)
			efforts = 1;
	}
	player->food -= efforts;
	if(player->food < 0)
		player->food = 0;
	return true;
}

bool enough(const variablei& v1, const variablei& v2) {
	for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1))
		if(v1.variables[i] < v2.variables[i])
			return false;
	return true;
}

void pass_time(unsigned minutes) {
	game.variables[Time] += minutes;
}

int get_hour() {
	return (game.get(Time) / 60) % 24;
}

bool pass_payment(actionn action, const variablei& required) {
	if(required.variables[Coins]) {
		auto message = get_confirm(action);
		if(message) {
			if(!confirm_message(message, required.variables[Coins]))
				return false;
		}
		game.variables[Coins] -= required.variables[Coins];
	}
	return true;
}

long choose_player_action(const char* cancel) {
	char temp[32]; stringbuilder sb(temp);
	sb.add(message_names[WhatPlayerDo], player->name());
	return choose_large_menu(temp, cancel);
}

bool confirm_message(messagen header, int value) {
	char temp[512]; stringbuilder sb(temp);
	sb.add(getnm(header), value);
	an.add(1, getnm(Agree));
	return show_message(temp, true, getnm(Decline), 0) != 0;
}

void for_each_party(fnevent proc) {
	pushvalue push(player);
	for(auto p : adventurers) {
		if(!p)
			continue;
		player = p; proc();
	}
}

void show_message(actionn id, ...) {
	XVA_FORMAT(id);
	char temp[512]; stringbuilder sb(temp);
	sb.addv(getnm(id), format_param);
	show_message(temp, false, getnm(Continue), '\n');
}

long choose_message(actionn id, bool can_cancel) {
	return show_message(getnm(id), true, can_cancel ? getnm(Cancel) : 0, 27);
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

void setv(picturen v) {
	if(!v)
		return;
	answer_picture = v;
	// Day/Night 
	if(answer_picture == PicCity) {
		auto n = get_hour();
		if(n < 7 || n > 22)
			answer_picture = PicCityNight;
	}
}

bool apply_action(const actioni* p) {
	pushvalue push(answer_picture);
	setv(get_picture(p->action));
	if(!pass_payment(p->action, p->required))
		return false;
	if(p->proc)
		p->proc();
	return true;
}

const actioni* choose_location(const actioni* source) {
	if(!source)
		return 0;
	for(auto p = source; *p; p++) {
		if(p->required && !enough(game, p->required))
			continue; // Can't pay or other reputation
		if(p->action >= Tavern && p->action <= WizardTower)
			an.add((long)p, getnm(VisitBuilding), getnm(p->action));
		else
			an.add((long)p, getnm(p->action));
	}
	return (actioni*)choose_large_menu_no_player(getnm(WhichWayToGo), getnm(Cancel));
}

const actioni* choose_building_action(const actioni* source) {
	if(!source)
		return 0;
	for(auto p = source; *p; p++) {
		if(p->required && !enough(game, p->required))
			continue; // Can't pay or other reputation
		if(!pass_restriction(p->restriction))
			continue; // Not pass restriction
		an.add((long)p, getnm(p->action));
	}
	return (actioni*)choose_player_action(getnm(Cancel));
}