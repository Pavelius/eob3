#include "game.h"

#define RS(T) (sprite*)bin_##T

extern unsigned char bin_blue[];
extern unsigned char bin_border[];
extern unsigned char bin_brick[];
extern unsigned char bin_chargen[];
extern unsigned char bin_chargenb[];
extern unsigned char bin_decors[];
extern unsigned char bin_dung[];
extern unsigned char bin_font6[];
extern unsigned char bin_font8[];
extern unsigned char bin_green[];
extern unsigned char bin_invent[];
extern unsigned char bin_items[];
extern unsigned char bin_itemgl[];
extern unsigned char bin_itemgs[];
extern unsigned char bin_menu[];
extern unsigned char bin_overlays[];
extern unsigned char bin_playfld[];
extern unsigned char bin_portm[];
extern unsigned char bin_scene[];
extern unsigned char bin_thrown[];
extern unsigned char bin_xspl[];

extern unsigned char bin_kobold[];
extern unsigned char bin_leech[];

sprite* res_data[LastRes + 1] = {
	RS(font6), RS(font8),
	RS(blue), RS(brick), 0, RS(dung), 0, RS(green), 0,
	RS(kobold), RS(leech),
	RS(border), RS(chargen), RS(chargenb), 0, RS(invent),
	RS(itemgs), RS(itemgl), RS(items), RS(overlays), RS(decors),
	RS(menu), RS(playfld), RS(portm), RS(scene), RS(thrown), RS(xspl)
};