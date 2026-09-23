#pragma once

enum soundn : unsigned char;

struct soundi {
	unsigned*		data;
	unsigned		size;
	short unsigned	division;
};
extern soundi songs[];

void music_play(soundn id);
void music_setvolume(short unsigned v);
void music_stop();