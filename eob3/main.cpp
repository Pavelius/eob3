#include "action.h"
#include "answers.h"
#include "creature.h"
#include "draw.h"
#include "game.h"
#include "shape.h"
#include "sound.h"
#include "rand.h"
#include "timer.h"
#include "stringvar.h"

void main_util();

static void load_game() {
}

static void next_main_menu() {
	auto result = choose_main_menu();
	if(result > 0)
		next_scene((fnevent)result);
}

void main_menu() {
	game_clear();
	current_music = MusKvirasim;
	an.add((long)game_generation, getnm(StartGame));
	an.add((long)load_game, getnm(LoadGame));
	an.add((long)buttoncancel, getnm(ExitGame));
	next_main_menu();
}

static bool test_items() {
	if(item_data[Robe].wear != Body)
		return false;
	if(item_data[Helm].avatar.pack != 20)
		return false;
	return true;
}

static bool auto_test() {
	if(!test_items())
		return false;
	return true;
}

int main(int argc, char* argv[]) {
	// start_random_seed = getcputime();
	start_random_seed = 14479921;
	srand(start_random_seed);
	initialize_gui();
	initialize_shapes();
	stringbuilder::custom = stringbuilder_custom;
#ifdef _DEBUG
	if(!auto_test())
		return -1;
	main_util();
#endif
	// music_enable = false;
	sys_create_window(-1, -1, 320, 200, 0, 32);
	sys_caption("Eye of beholder (remake)");
	sys_timer(100);
	next_scene(main_menu);
	start_scene();
	return 0;
}

#ifdef _MSC_VER
int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}
#endif