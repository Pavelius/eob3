#include "game.h"

extern unsigned char bin_font6[];
extern unsigned char bin_font8[];
extern unsigned char bin_menu[];
extern unsigned char bin_playfld[];

sprite* res_data[LastRes + 1] = {
	(sprite*)bin_font6, (sprite*)bin_font8,
	0, 0, 0, 0, 0, 0, 0, 0,
	(sprite*)bin_menu, (sprite*)bin_playfld,
};