#include "action.h"
#include "answers.h"
#include "creature.h"
#include "game.h"
#include "stringbuilder.h"

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
	case Carousing: return PicTavern2;
	case EatFoodAndDrink: return PicTavern2;
	case Inn: return PicInn;
	case PickPocketsSomeone: return PicPickpockets;
	case Tavern: return PicTavern;
	default: return PicCity;
	}
}

soundn get_music(actionn v) {
	switch(v) {
	case NoAction: return MusKvirasim;
	case Inn: return MusInn;
	case Tavern: return MusTavern;
	case Temple: return MusTemple;
	default: return NoMusic;
	}
}

bool indoor(actionn v) {
	return v >= Tavern && v <= WizardTower;
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