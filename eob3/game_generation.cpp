#include "answers.h"
#include "creature.h"
#include "draw.h"
#include "game.h"
#include "pushvalue.h"
#include "sound.h"

int generate_player_index;

static racen select_race() {
	for(auto i = Human; i <= Halfling; i = (racen)(i + 1))
		an.add(i, race_names[i]);
	return (racen)choose_generate_dialog(message_names[SelectRace]);
}

static gendern select_gender() {
	an.add(Male, gender_names[Male]);
	an.add(Female, gender_names[Female]);
	return (gendern)choose_generate_dialog(message_names[SelectGender]);
}

static classn select_class(racen race) {
	for(auto i = Fighter; i <= FighterMageTheif; i = (classn)(i + 1)) {
		if(!allow(i, race))
			continue;
		an.add(i, class_names[i]);
	}
	return (classn)choose_generate_dialog(message_names[SelectGender]);
}

static alignmentn select_alignment(classn type) {
	for(auto i = LawfulGood; i <= ChaoticEvil; i = (alignmentn)(i + 1)) {
		if(!allow(i, type))
			continue;
		an.add(i, alignment_names[i]);
	}
	return (alignmentn)choose_generate_dialog(message_names[SelectAlignment]);
}

static unsigned char choose_avatar() {
	unsigned char source[256];
	auto count = select_avatars(source, player->race, player->gender, player->type, no_party_avatar);
	return (unsigned char)choose_avatar(source, count);
}

void change_avatar() {
	player->avatar = choose_avatar();
}

static void game_clear() {
	memset(party, 0, lenghof(party));
	for(auto& e : characters)
		e.clear();
}

static bool is_party_formed() {
	for(auto i = 0; i < 4; i++) {
		if(characters[i].avatar == 0xFF)
			return false;
	}
	return true;
}

static void party_generation() {
	pushvalue push(player);
	current_music = MusGenerate;
	generate_player_index = 0;
	game_clear();
	while(true) {
		const char* footer = is_party_formed() ? message_names[GeneraionInfo] : 0;
		generate_player_index = choose_generate_box(message_names[GeneraionInfo], footer, generate_player_index);
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
	for(auto i = 0; i < 4; i++)
		characters[i].joinparty();
}

void game_generation() {
	game_clear();
	party_generation();
}