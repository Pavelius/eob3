#include "game.h"

#define RS(T) (sprite*)bin_##T

extern unsigned char bin_ant[];
extern unsigned char bin_aservant[];
extern unsigned char bin_basilisk[];
extern unsigned char bin_beholder[];
extern unsigned char bin_bulette[];
extern unsigned char bin_blue[];
extern unsigned char bin_bonenaga[];
extern unsigned char bin_border[];
extern unsigned char bin_brick[];
extern unsigned char bin_chargen[];
extern unsigned char bin_chargenb[];
extern unsigned char bin_chimera[];
extern unsigned char bin_cleric1[];
extern unsigned char bin_cleric2[];
extern unsigned char bin_cleric3[];
extern unsigned char bin_compass[];
extern unsigned char bin_cube[];
extern unsigned char bin_decors[];
extern unsigned char bin_disbeast[];
extern unsigned char bin_dragon[];
extern unsigned char bin_dran[];
extern unsigned char bin_drider1[];
extern unsigned char bin_drowelf[];
extern unsigned char bin_dung[];
extern unsigned char bin_dwarf[];
extern unsigned char bin_evilwiz[];
extern unsigned char bin_feyr[];
extern unsigned char bin_flind[];
extern unsigned char bin_font6[];
extern unsigned char bin_font8[];
extern unsigned char bin_forest[];
extern unsigned char bin_frgiant[];
extern unsigned char bin_fsnake[];
extern unsigned char bin_gargoyle[];
extern unsigned char bin_gasspore[];
extern unsigned char bin_green[];
extern unsigned char bin_groan[];
extern unsigned char bin_hag[];
extern unsigned char bin_invent[];
extern unsigned char bin_items[];
extern unsigned char bin_itemgl[];
extern unsigned char bin_itemgs[];
extern unsigned char bin_kobold[];
extern unsigned char bin_leech[];
extern unsigned char bin_lich[];
extern unsigned char bin_menu[];
extern unsigned char bin_minotaur[];
extern unsigned char bin_mist[];
extern unsigned char bin_mound[];
extern unsigned char bin_oslug[];
extern unsigned char bin_overlays[];
extern unsigned char bin_playfld[];
extern unsigned char bin_portm[];
extern unsigned char bin_pudding[];
extern unsigned char bin_scene[];
extern unsigned char bin_scaladar[];
extern unsigned char bin_shadow[];
extern unsigned char bin_shound[];
extern unsigned char bin_slither[];
extern unsigned char bin_snaga[];
extern unsigned char bin_steelsh[];
extern unsigned char bin_thrown[];
extern unsigned char bin_troll[];
extern unsigned char bin_xspl[];
extern unsigned char bin_ubeast[];
extern unsigned char bin_water[];
extern unsigned char bin_weird[];
extern unsigned char bin_wghost[];
extern unsigned char bin_wight[];
extern unsigned char bin_wolf[];
extern unsigned char bin_wyvern[];
extern unsigned char bin_xanath1[];
extern unsigned char bin_xorn1[];
extern unsigned char bin_zombie[];

sprite* res_data[LastRes + 1] = {
	RS(font6), RS(font8),
	RS(blue), RS(brick), 0, RS(dung), RS(forest), RS(green), 0,
	RS(ant), RS(aservant), RS(basilisk), RS(beholder), RS(bulette), RS(bonenaga), RS(chimera),
	RS(cleric1), RS(cleric2), RS(cleric3), RS(cube), RS(disbeast), RS(drider1), RS(dragon), RS(dran), RS(drowelf),
	RS(dwarf), RS(evilwiz), RS(feyr), RS(flind), RS(frgiant), RS(fsnake), RS(gargoyle), RS(gasspore),
	RS(groan), RS(hag), RS(kobold), RS(leech), RS(lich), RS(minotaur),
	RS(mist), RS(mound), RS(oslug), RS(pudding), RS(scaladar), RS(shadow), RS(shound), RS(slither), RS(snaga), RS(steelsh),
	RS(troll), RS(ubeast), RS(water), RS(weird), RS(wghost), RS(wight), RS(wolf), RS(wyvern),
	RS(xorn1), RS(zombie),
	RS(border), RS(chargen), RS(chargenb), RS(compass), RS(invent),
	RS(itemgs), RS(itemgl), RS(items), RS(overlays), RS(decors),
	RS(menu), RS(playfld), RS(portm), RS(scene), RS(thrown), RS(xspl),
};