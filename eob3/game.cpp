#include "answers.h"
#include "draw.h"
#include "game.h"
#include "sound.h"
#include "rand.h"
#include "timer.h"

void main_util();

static void load_game() {
	if(!confirm("Do you really want exit?"))
		return;
}

static void next_main_menu() {
	auto result = choose_main_menu();
	if(result > 0)
		next_scene((fnevent)result);
}

static void main_menu() {
	an.add((long)game_generation, message_names[StartGame]);
	an.add((long)load_game, message_names[LoadGame]);
	an.addv(buttonparam, 0, 0, message_names[ExitGame], KeyEscape, 0);
	next_main_menu();
}

int main(int argc, char* argv[]) {
	start_random_seed = getcputime();
	// music_mute = true;
	// start_random_seed = 1423089921;
	srand(start_random_seed);
	initialize_gui();
#ifdef _DEBUG
	main_util();
#endif
	current_music = MusKvirasim;
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