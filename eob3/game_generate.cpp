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
	memset(adventurers, 0, sizeof(adventurers));
	memset(dungeons, 0, sizeof(dungeons)); dungeons_count = 0;
	memset(shops, 0, sizeof(shops));
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
		game.add(Coins, xrand(4, 20) * gp);
		game.add(Reputation, 10);
		break;
	case Theif:
		game.add(Coins, xrand(3, 18) * gp);
		game.add(Reputation, 5);
		break;
	case Ranger:
		game.add(Coins, xrand(1, 6) * gp);
		game.add(Reputation, 5);
		break;
	case Cleric:
		game.add(Coins, xrand(1, 6) * gp);
		game.add(Reputation, 10);
		game.add(Blessing, 10);
		break;
	case Paladin:
		game.add(Coins, xrand(10, 20) * gp);
		game.add(Reputation, 10);
		game.add(Blessing, 5);
		break;
	default:
		game.add(Coins, xrand(2, 7) * gp);
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
	game.add(Time, 10 * 60);
}

static void party_generation() {
	pushvalue push(player);
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
	}
}

static void create(int index, racen race, gendern gender, classn type, alignmentn alignment) {
	player = characters + index;
	create_character(race, gender, type, alignment);
	adventurers[index] = player;
}

static void party_random_generation() {
	create(1, Human, Female, Fighter, NeutralGood);
	create(2, Elf, Female, MageTheif, ChaoticGood);
	create(3, Dwarf, Male, Cleric, ChaoticGood);
	create(0, Human, Male, Fighter, LawfulGood);
}

static void test_city_menu() {
	auto quest = (questn)0;
	quests[quest].set(QuestPrepared);
	dungeon_create(quest, quests[quest].sites);
	// next_scene(play_city_actions);
	// consolen("This is a [long] text display [+plus] or [-minuses], maybe [~grayed] of simple format output strings.", language_names[Elf]);
	start_quest();
}

void game_generation() {
	game_clear();
	refresh_shops();
	party_random_generation();
	// party_generation();
	start_variables();
	test_city_menu();
}