#include "answers.h"
#include "draw.h"
#include "game.h"
#include "sound.h"
#include "rand.h"
#include "timer.h"

void main_util();

static void character_generation() {
}

static void load_game() {
	if(!confirm("Do you really want exit?"))
		return;
}

static void add_exit_scene(messagen v) {
	an.addv(buttonparam, -1, 0, message_names[v], KeyEscape, 0);
}

static void next_main_menu() {
	auto result = choose_main_menu();
	if(result > 0)
		next_scene((fnevent)result);
}

static void main_menu() {
	music_play(MusAdept);
	an.add((long)character_generation, message_names[StartGame]);
	an.add((long)load_game, message_names[LoadGame]);
	add_exit_scene(ExitGame);
	next_main_menu();
}

int main(int argc, char* argv[]) {
	start_random_seed = getcputime();
	// start_random_seed = 1423089921;
	srand(start_random_seed);
	initialize_gui();
#ifdef _DEBUG
	main_util();
#endif
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