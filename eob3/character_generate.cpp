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

void game_generation() {
	pushvalue push(player);
	music_play(MusGenerate);
	while(true) {
		const char* footer = 0;
		generate_player_index = choose_generate_box("Test string", footer);
		if(generate_player_index == 2000) // Start game
			break;
		player = characters + generate_player_index - 1;
		select_race();
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