#include "creature.h"
#include "game.h"
#include "spell.h"

static int roll(int c, int d, int m) {
	for(auto i = 0; i < c; i++)
		m += roll_dice(d);
	return m;
}

static bool if_wounded() {
	return player->hp < player->hpm;
}

static void cure_wounds(int level) {
	auto n = roll(1, 8, 0) + level;
	player->heal(n);
}

spelli spells[] = {
	{"Cure light wound", {1}, cure_wounds, if_wounded},
};