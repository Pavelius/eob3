#include "answers.h"
#include "draw.h"
#include "game.h"
#include "sound.h"
#include "rand.h"
#include "timer.h"

void main_util();

static void main_menu() {
	music_play(MusAdept);
	while(true) {
		an.add(1, "Set volume maximum");
		an.add(2, "Set volume minimal");
		an.add(3, "First song");
		an.add(4, "Second song");
		an.addv(buttonparam, -1, 0, "Exit Game", KeyEscape, 0);
		//
		auto result = choose_main_menu();
		if(result <= 0)
			break;
		switch(result) {
		case 1: music_setvolume(0xFFFF); break;
		case 2: music_setvolume(20000); break;
		case 3: music_play(MusFinster); break;
		case 4: music_play(MusGenerate); break;
		default: break;
		}
	}
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