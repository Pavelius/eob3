#include "action.h"
#include "answers.h"
#include "creature.h"
#include "game.h"
#include "pushvalue.h"
#include "rand.h"

static bool is_dwarf_or_warrior() {
	return player->iswarrior() || player->is(Dwarf);
}

static void great_pick_pockets() {
	if(player->roll(PickPockets)) {
		last_number = roll_dice(2, 6) * 10 * gp;
		auto jackpot = create_item(RandomTavernPickPocket, 20, 20);
		show_message(GreatPickPocketsSuccess, jackpot.name());
		addv(Coins, last_number);
		player->additem(jackpot);
	} else {
		last_number = roll_dice(1, 6) * 10 * gp;
		show_message(GreatPickPocketsFail);
	}
}

static void gambling_cheat() {
	last_number = roll_dice(1, 6) * 10 * gp;
	if(player->roll(Charisma, -2)) {
		show_message(GamblingÑheatSuccess);
		addv(Coins, last_number);
		addv(Reputation, 1);
	} else {
		show_message(GamblingÑheatFail);
		addv(Coins, -last_number);
		addv(Reputation, -1);
	}
}

static void drink_contest() {
	if(player->roll(Constitution, -1)) {
		last_number = 20 * gp;
		show_message(DrinkContestSuccess);
		addv(Coins, last_number);
		addv(Reputation, 1);
	} else {
		last_number = 10 * gp;
		show_message(DrinkContestFail);
		addv(Coins, -last_number);
		player->add(DiseaseLevel, 1);
	}
}

static void nude_night_ride() {
	last_number = 30 * gp;
	if(player->roll(Dexterity, -2)) {
		show_message(NudeNightRideSuccess);
		addv(Coins, last_number);
		addv(Reputation, 1);
		player->addexp(25);
	} else {
		show_message(NudeNightRideFail);
		addv(Reputation, -1);
	}
}

static void priest_and_golden_chalise() {
	last_number = 20;
	if(player->roll(Wisdow, -1)) {
		show_message(PriestAndGoldenChaliseSuccess);
		addv(Coins, last_number);
		addv(Blessing, roll_dice(1, 4, 1));
		player->addexp(25);
	} else {
		show_message(PriestAndGoldenChaliseFail);
		addv(Coins, roll_dice(3, 6) * 5);
	}
}

static actioni carousing_actions[] = {
	{GreatPickPockets, {}, {Theif}, great_pick_pockets},
	{GamblingÑheat, {}, {}, gambling_cheat},
	{DrinkContest, {}, {}, drink_contest, is_dwarf_or_warrior},
	{NudeNightRide, {}, {}, nude_night_ride},
	{PriestAndGoldenChalise, {}, {}, priest_and_golden_chalise},
};
const unsigned carouse_size = lengthof(carousing_actions);

static void select_actions(actioni** result) {
	auto p = result;
	for(auto& e : carousing_actions)
		*p++ = &e;
	zshuffle(result, carouse_size);
}

void carousing() {
	actioni* actions[carouse_size]; select_actions(actions);
	pushvalue push(player);
	pushvalue push_picture(answer_picture);
	for(auto p : adventurers) {
		if(!p || p->isdisabled())
			continue;
		player = p;
		auto pa = pick_action(actions, carouse_size);
		if(!pa)
			break;
		setv(get_picture(pa->action));
		show_message(pa->action);
		if(pa->proc)
			pa->proc();
	}
	pass_time(roll_dice(1, 2, 2) * 60);
}
