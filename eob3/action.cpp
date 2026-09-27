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

actioni* find_action(actioni* p, resultn result) {
	if(!p || p->type == NoResult)
		return 0;
	for(p++; p->type != NoResult && p->type != Action; p++) {
		if(p->type == result)
			return p;
	}
	return 0;
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