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
	auto count = select_avatars(source, player->race, player->gender, player->type);
	return (unsigned char)choose_avatar(source, count);
}

void game_generation() {
	pushvalue push(player);
	current_music = MusGenerate;
	generate_player_index = 0;
	while(true) {
		const char* footer = 0;
		generate_player_index = choose_generate_box(message_names[MsgGeneraionInfo], footer, generate_player_index);
		if(generate_player_index == 2000) // Start game
			break;
		auto race = select_race();
		auto gender = select_gender();
		auto class_type = select_class(race);
		auto alignment = select_alignment(class_type);
		player = characters + generate_player_index - 1;
		player->clear();
		player->race = race;
		player->gender = gender;
		player->type = class_type;
		player->alignment = alignment;
		generate_abilities();
		player->update();
		player->avatar = choose_avatar();
	}
	//while(true) {
	//	player_position = choose_player_position();
	//	if(!player_position)
	//		break;
	//	if(*player_position)
	//		choose_generate_box(paint_character_edit);
	//	else {
	//		choose_race(0);
	//		choose_gender(0);
	//		choose_class(0);
	//		choose_alignment(0);
	//		player = bsdata<creaturei>::addz();
	//		player->clear();
	//		clear_spellbook();
	//		create_npc(player, 0, is_party_name);
	//		generate_abilities();
	//		apply_race_ability();
	//		roll_player_hits();
	//		update_player();
	//		update_player_hits();
	//		if(!choose_avatar()) {
	//			player->clear();
	//			continue;
	//		}
	//		create_player_finish();
	//		*player_position = player;
	//		choose_generate_box(paint_character_edit);
	//	}
	//}
	//// Join party
	//for(auto p : characters) {
	//	if(!p)
	//		continue;
	//	player = p;
	//	join_party();
	//}

}