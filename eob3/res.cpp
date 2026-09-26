#include "game.h"

extern unsigned char bin_font6[];
extern unsigned char bin_font8[];
extern unsigned char bin_chargen[];
extern unsigned char bin_chargenb[];
extern unsigned char bin_menu[];
extern unsigned char bin_playfld[];
extern unsigned char bin_portm[];
extern unsigned char bin_xspl[];

sprite* res_data[LastRes + 1] = {
	(sprite*)bin_font6, (sprite*)bin_font8,
	0, 0,
	0, 0,
	0, (sprite*)bin_chargen, (sprite*)bin_chargenb, 0, 0, 0, 0, 0,
	(sprite*)bin_menu, (sprite*)bin_playfld, (sprite*)bin_portm, 0, 0, (sprite*)bin_xspl
};