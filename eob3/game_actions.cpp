#include "action.h"
#include "answers.h"
#include "console.h"
#include "creature.h"
#include "game.h"
#include "perference.h"
#include "pushvalue.h"
#include "rand.h"
#include "sound.h"

actionn location = NoAction;

static void leave_outside() {
	location = NoAction;
}

static void natural_healing(int bonus) {
	if(bonus >= 0)
		player->heal(xrand(1, 3) + bonus);
	else {
		consolen(getnm(FeelDisease));
		player->damage(HealthDamage, xrand(1, 2) - bonus);
	}
}

static void refresh_memorized_spells() {
	auto ps = get_spellbook(player);
	if(!ps)
		return;
	memcpy(player->spells, ps->spells, sizeof(player->spells));
}

static void satisfy() {
	player->food = player->getfood();
}

void rest_party(int bonus) {
	pass_time(60 * 8);
	if(bonus < 0)
		// Cursed food make bad sleep
		all_party(natural_healing, true, bonus);
	else {
		all_party(natural_healing, true, bonus);
		all_party(camp_autocast, false);
		all_party(refresh_memorized_spells, false);
		all_party(satisfy, false);
	}
}

static void rest_party_inn() {
	if(!confirm(ConfirmRestParty))
		return;
	rest_party(3);
	leave_outside();
	game_serial("autosave", true, false);
	consolen(getnm(GameSaved));
}

static void remove_hunger() {
	player->food = player->getfood();
}

static void eat_and_drink() {
	all_party(remove_hunger);
	show_message(EatAndDrinkSuccess);
}

static void gambling() {
	static long source[] = {20, 50, 100, 500, 1000};
	while(running_scene()) {
		if(!check_suspecious())
			return;
		if(!check_activity())
			return;
		for(auto n : source) {
			if(game.get(Coins) >= n)
				an.add(n, "%1i", n);
		}
		auto bet = choose_message(GamblingIntro, true);
		if(!bet)
			break;
		addv(Checks, -1);
		pass_effort();
		if(player->roll(Charisma, -4)) {
			game.add(Coins, bet);
			show_message(GamblingWin, bet);
			// Win more cause additional suspeciously
			if(bet >= 500)
				addv(Checks, -1);
			if(bet >= 1000)
				addv(Checks, -1);
		} else {
			game.add(Coins, -bet);
			show_message(GamblingLose, bet);
		}
	}
}

static void donate() {
	static long source[] = {100, 200, 500};
	for(auto n : source) {
		if(game.get(Coins) >= n)
			an.add(n, "%1i", n);
	}
	auto bet = choose_message(DonateIntro, true);
	if(!bet)
		return;
	pass_time(xrand(10, 20));
	addv(Coins, -bet);
	auto chance_reputation = bet / 10;
	if(chance(chance_reputation))
		addv(Reputation, 1, 0, 100); // RULE: Donation can raise reputation.
	bet = bet / 50 - 1;
	addv(Blessing, xrand(bet, bet * 2), 0, 100);
	show_message(DonateAccept);
}

static void pick_pockets() {
	auto coins = 0;
	if(!check_suspecious())
		return;
	if(!check_activity())
		return;
	addv(Checks, -1);
	pass_effort();
	if(player->roll(PickPockets)) {
		coins = roll_dice(2, 6) * 20;
		addv(Coins, coins);
		show_message(PickPocketsCoins, coins);
		if(chance(15)) {
			item it = random(RandomTavernPickPocket);
			it.create(20, 5);
			player->additem(it);
			show_message(PickPocketsJackpot, it.name());
		}
	} else {
		if(last_ability_roll >= 95) {
			addv(Reputation, -1);
			addv(Checks, -3);
			player->damage(Bludgeon, xrand(1, 4));
			answer_picture = PicPickpocketsFail;
			show_message(PickPocketsFail);
		} else
			show_message(PickPocketsFail);
	}
}

static void heal_wounds() {
	player->hp = player->hpm;
}

static void ask_for_miracle() {
	auto chance_miracle = getv(Blessing) / 4;
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm(AskMiracleIntro), player->name());
	if(chance(chance_miracle)) {
		auto raise = random(RandomMiracleRaise);
		if(raise < 25)
			player->basic.abilities[raise]++;
		else {
			raise = Hits;
			player->hpr += xrand(2, 4);
		}
		update_player();
		sb.adds("\"");
		sb.add(getnm(UseHolySymbolSuccess));
		sb.add(".");
		sb.adds(getnm(YourAbilityGrow), ability_names[raise]);
		sb.add(".\"");
	} else {
		sb.adds("\"");
		switch(d4()) {
		case 1: sb.add(getnm(UseHolySymbolNoEffect)); break;
		case 2: sb.add(getnm(UseHolySymbolFail)); break;
		default: sb.add(getnm(YourFaithIsWeak)); break;
		}
		sb.add("\"");
	}
	addv(Blessing, -xrand(10, 20));
	show_message(temp, false, getnm(Continue), 13);
}

static void scrible_scrolls() {
}

static void repair_weapons() {
	for(auto& e : player->wears) {
		if(!e || !e.isweapon())
			continue;
		if(e.hits > 1) // Repair only if state is not trade-in.
			e.hits = 0;
	}
}

static bool allow_repair_weapons() {
	for(auto& e : player->wears) {
		if(!e || !e.isweapon())
			continue;
		if(e.hits > 1) // Repair only if state is not trade-in.
			return true;
	}
	return false;
}

static void buy_weapons() {
	buy_item(WeaponShop, BuyWeaponsEmpty);
}

static void pray_for_spells() {
	choose_spells(getnm(PrayForSpells), getnm(Cancel), 0);
}

static void memorize_spells() {
	choose_spells(getnm(MemorizeSpells), getnm(Cancel), 1);
}

static actioni tavern_actions[] = {
	{EatFoodAndDrink, {Coins, 20}, {}, eat_and_drink},
	{PickPocketsAction, {}, {Theif}, pick_pockets},
	{Gambling, {}, {Theif, Fighter, Ranger, Mage}, gambling},
	{Carousing, {Coins, 200}, {}, carousing},
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni inn_actions[] = {
	{RestParty, {}, {}, rest_party_inn},
	{MemorizeSpells, {}, {Mage}, memorize_spells},
	{PrayForSpells, {}, {Cleric}, pray_for_spells},
	{ScribleScrolls, {}, {Mage}, scrible_scrolls},
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni blacksmith_actions[] = {
	{BuyWeapons, {}, {}, buy_weapons},
	{RepairWeapons, {Coins, 20}, {}, repair_weapons, allow_repair_weapons},
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni temple_actions[] = {
	{AskMiracle, {}, {}, ask_for_miracle, if_blessing_good},
	{HealWounds, {Coins, 50}, {}, heal_wounds, if_wounded},
	{Donate, {}, {}, donate},
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni wizard_tower_actions[] = {
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni palace_actions[] = {
	{LeaveOutside, {}, {}, leave_outside},
	{}};
static actioni city_actions[] = {
	{GoAdventure, {}, {}, go_adventure},
	{Tavern},
	{Blacksmith, {Reputation, 25}},
	{Temple},
	{Inn, {Coins, 30}, {}},
	{WizardTower, {Reputation, 45}},
	{Palace, {Reputation, 70}},
	{}};

static actioni* get_actions(actionn v) {
	switch(v) {
	case NoAction: return city_actions;
	case Tavern: return tavern_actions;
	case Inn: return inn_actions;
	case Blacksmith: return blacksmith_actions;
	case Palace: return palace_actions;
	case Temple: return temple_actions;
	case WizardTower: return wizard_tower_actions;
	default: return 0;
	}
}

void play_city_actions() {
	while(running_scene()) {
		setv(get_picture(location));
		auto music = get_music(location);
		if(music)
			current_music = music;
		play_city();
		if(indoor(location)) {
			auto p = choose_action(get_actions(location));
			if(!p)
				continue;
			apply_action(p);
		} else {
			auto p = choose_location(get_actions(location));
			if(!p)
				continue;
			if(!pass_payment(p->action, p->required))
				continue;
			if(p->proc)
				p->proc();
			else
				location = p->action;
		}
	}
}

void load_game() {
	game_serial("autosave", false, true);
}

static void save_game() {
	game_serial("autosave", true, true);
}

static void empthy_exit_game() {
	// Before game exit
}

static void exit_game() {
	if(!confirm(ConfirmExitGame))
		return;
	next_scene(empthy_exit_game);
}

static void game_music_stop() {
	if(!music_enable)
		music_stop();
}

static void game_perferences() {
	static perferencei source[] = {
		{Music, music_enable, game_music_stop},
		{Sound, sound_enable},
		{}};
	show_perferences(getnm(Settings), source);
}

static void game_options() {
	static actioni actions[] = {
		{LoadGame, {}, {}, load_game},
		{SaveGame, {}, {}, save_game},
		{Settings, {}, {}, game_perferences},
		{ExitGame, {}, {}, exit_game},
		{}};
	open_options(actions);
}

void show_dungeon_options() {
	static actioni actions[] = {
		{MemorizeSpells, {}, {Mage}, memorize_spells},
		{PrayForSpells, {}, {Cleric}, pray_for_spells},
		{ScribleScrolls, {}, {Mage}, scrible_scrolls},
		{GameOptions, {}, {}, game_options},
		{}};
	open_options(actions);
}