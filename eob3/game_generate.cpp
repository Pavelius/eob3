#include "action.h"
#include "answers.h"
#include "console.h"
#include "creature.h"
#include "dungeon.h"
#include "draw.h"
#include "game.h"
#include "pushvalue.h"
#include "quest.h"
#include "rand.h"
#include "sound.h"

int generate_player_index;

static racen select_race() {
	for(auto i = Human; i <= Halfling; i = (racen)(i + 1))
		an.add(i, race_names[i]);
	return (racen)choose_generate_dialog(getnm(SelectRace));
}

static gendern select_gender() {
	an.add(Male, gender_names[Male]);
	an.add(Female, gender_names[Female]);
	return (gendern)choose_generate_dialog(getnm(SelectGender));
}

static classn select_class(racen race) {
	for(auto i = Fighter; i <= FighterMageTheif; i = (classn)(i + 1)) {
		if(!allow(i, race))
			continue;
		an.add(i, class_names[i]);
	}
	return (classn)choose_generate_dialog(getnm(SelectClass));
}

static alignmentn select_alignment(classn type) {
	for(auto i = LawfulGood; i <= ChaoticEvil; i = (alignmentn)(i + 1)) {
		if(!allow(i, type))
			continue;
		an.add(i, alignment_names[i]);
	}
	return (alignmentn)choose_generate_dialog(getnm(SelectAlignment));
}

static unsigned char choose_avatar() {
	unsigned char source[256];
	auto count = select_avatars(source, player->race, player->gender, player->type, no_party_avatar);
	return (unsigned char)choose_avatar(source, count);
}

void change_avatar() {
	player->avatar = choose_avatar();
}

void game_clear() {
	memset((void*)&game, 0, sizeof(game));
	memset(adventurers, 0, sizeof(adventurers));
	memset(dungeons, 0, sizeof(dungeons)); dungeons_count = 0;
	memset(shops, 0, sizeof(shops));
	memset(boosts, 0, sizeof(boosts)); boost_count = 0;
	for(auto& e : characters)
		e.clear();
	for(auto& e : quests)
		e.clear();
}

static bool is_party_formed() {
	for(auto i = 0; i < 4; i++) {
		if(characters[i].avatar == 0xFF)
			return false;
	}
	return true;
}

static void start_variables(classn type) {
	switch(type) {
	case Mage:
		addv(Coins, xrand(4, 20) * gp);
		addv(Reputation, 10);
		break;
	case Theif:
		addv(Coins, xrand(3, 18) * gp);
		addv(Reputation, 5);
		break;
	case Ranger:
		addv(Coins, xrand(1, 6) * gp);
		addv(Reputation, 5);
		break;
	case Cleric:
		addv(Coins, xrand(1, 6) * gp);
		addv(Reputation, 10);
		addv(Blessing, 20);
		break;
	case Paladin:
		addv(Coins, xrand(10, 20) * gp);
		addv(Reputation, 10);
		addv(Blessing, 5);
		break;
	default:
		addv(Coins, xrand(2, 7) * gp);
		break;
	}
}

static void start_variables() {
	game.add(Coins, 10 * gp);
	game.add(Reputation, 5);
	for(auto p : adventurers) {
		if(p)
			start_variables(p->type);
	}
	pass_time(10 * 60);
}

static void party_generation() {
	current_music = MusGenerate;
	generate_player_index = 0;
	game_clear();
	while(true) {
		const char* footer = is_party_formed() ? getnm(GeneraionInfo) : 0;
		generate_player_index = choose_generate_box(getnm(GeneraionInfo), footer, generate_player_index);
		if(generate_player_index == 2000) // Start game
			break;
		player = characters + generate_player_index;
		if(player->avatar == 0xFF) {
			auto race = select_race();
			auto gender = select_gender();
			auto class_type = select_class(race);
			auto alignment = select_alignment(class_type);
			player->clear();
			player->race = race;
			player->gender = gender;
			player->type = class_type;
			player->alignment = alignment;
			// clear_spellbook();
			reroll_character();
			player->avatar = choose_avatar();
		}
		change_character();
	}
	// Join party
	for(auto i = 0; i < 4; i++) {
		player = characters + i;
		adventurers[i] = player;
		finish_character();
		prepare_default_spells();
	}
	player = characters;
}

#ifdef _DEBUG
static void create(int index, racen race, gendern gender, classn type, alignmentn alignment) {
	player = characters + index;
	create_character(race, gender, type, alignment);
	adventurers[index] = player;
}

static void party_random_generation() {
	create(1, Human, Female, Fighter, NeutralGood);
	create(2, Elf, Female, MageTheif, ChaoticGood);
	create(3, Dwarf, Male, Cleric, ChaoticGood);
	player->addgear(PriestScroll, CommonItem, (featn)Aid, 1);
	create(0, Human, Male, Fighter, LawfulGood);
}
#endif // _DEBUG

static unsigned select_quests(questi** result, int difficult) {
	auto ps = result;
	for(auto& e : quests) {
		if(e.difficult == difficult)
			*ps++ = &e;
	}
	return ps - result;
}

static unsigned add_quests(questi** result, int difficult, unsigned max_count) {
	questi* source[LastQuest + 1];
	auto count = select_quests(source, difficult);
	zshuffle(source, count);
	if(count > max_count)
		count = max_count;
	memcpy(result, source, count * sizeof(result[0]));
	return count;
}

static void prepare_quests() {
	questi* result[LastQuest + 1] = {};
	unsigned total_quest_count = 0;
	for(auto difficult = 0; difficult <= 4; difficult++)
		total_quest_count += add_quests(result + total_quest_count, difficult, 2);
	for(unsigned i = 0; i < total_quest_count; i++) {
		result[i]->set(QuestPrepared);
		dungeon_create(result[i]->index(), result[i]->sites);
	}
}

static void start_campaign() {
	prepare_quests();
}

void game_generation() {
	game_clear();
	refresh_shops();
#ifdef _DEBUG
	party_random_generation();
#else
	party_generation();
#endif // _DEBUG
	start_variables();
	start_campaign();
	next_scene(play_city_actions);
}