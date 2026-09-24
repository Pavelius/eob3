#include "creature.h"

static char hit_points_adjustment[] = {
	-4, -3, -2, -2, -1, -1, -1, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 3, 4, 5,
	5, 6, 6, 6, 7, 7
};
static char reaction_adjustment[] = {
	-7, -6, -4, -3, -2, -1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 2, 3, 3,
	4, 4, 4, 5, 5
};
static char defence_adjustment[] = {
	-5, -5, -4, -3, -2, -1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 3, 4, 4,
	4, 5, 5, 5, 6, 6
};
static char hit_probability[] = {
	-5, -5, -3, -3, -2, -2, -1, -1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 2, 2, 2, 3,
	3, 4, 4, 5, 6, 7
};
static char damage_adjustment[] = {
	-5, -5, -3, -3, -2, -2, -1, -1, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 2,
	3, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12, 14
};
static char cha_reaction_adjustment[] = {
	-10, -7, -6, -5, -4, -3, -2, -1, 0, 0,
	0, 0, 0, 1, 2, 3, 5, 6, 7,
	8, 9, 10, 11, 12, 13, 14
};
static char dwarven_bonus[] = {
	0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
	2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5,
	6, 6, 6, 6, 7
};
static const int hd_experience[] = {
	7, 15, 35, 65, 120, 175, 270, 420, 650, 975,
	1400, 1700, 2000, 3000
};
static char chance_learn_spell[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 35,
	40, 45, 50, 55, 60, 65, 70, 75, 85, 95,
	96, 97, 98, 99, 100
};

static char thac0_advance[4][22] = {
	{0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{0, 0, 0, 0, 2, 2, 2, 4, 4, 4, 6, 6, 6, 8, 8, 8, 10, 10, 10, 12, 12, 12},
	{0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10},
	{0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6},
};
static char saves_advance[4][5][22] = {
	// Warriors - 0
	{{16, 14, 14, 13, 13, 11, 11, 10, 10, 8, 8, 7, 7, 5, 5, 4, 4, 3, 3, 2, 2, 2},
	{18, 16, 16, 15, 15, 13, 13, 12, 12, 10, 10, 9, 9, 7, 7, 6, 6, 5, 5, 3, 3, 2},
	{17, 15, 15, 14, 14, 12, 12, 11, 11, 9, 9, 8, 8, 6, 6, 5, 5, 4, 4, 3, 3, 2},
	{20, 17, 17, 16, 16, 13, 13, 12, 12, 9, 9, 8, 8, 5, 5, 4, 4, 3, 3, 3, 3, 3},
	{19, 17, 17, 16, 16, 14, 14, 13, 13, 11, 11, 10, 10, 8, 8, 7, 7, 6, 6, 5, 5, 4}},
	// Priest - 1
	{{10, 10, 10, 10, 9, 9, 9, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 2, 2, 2},
	{14, 14, 14, 14, 13, 13, 13, 11, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 6, 6, 6},
	{13, 13, 13, 13, 12, 12, 12, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 5, 5, 5},
	{16, 16, 16, 16, 15, 15, 15, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 8, 8, 8},
	{15, 15, 15, 15, 14, 14, 14, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 7, 7, 7}},
	// Rogues - 2
	{{13, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8},
	{14, 14, 14, 14, 14, 12, 12, 12, 12, 10, 10, 10, 10, 8, 8, 8, 8, 6, 6, 6, 6, 4},
	{12, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7},
	{16, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11},
	{15, 15, 15, 15, 15, 13, 13, 13, 13, 11, 11, 11, 11, 9, 9, 9, 9, 7, 7, 7, 7, 5}},
	// Wizards - 3
	{{14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 8},
	{11, 11, 11, 11, 11, 11, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 3},
	{13, 13, 13, 13, 13, 13, 11, 11, 11, 11, 11, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 5},
	{15, 15, 15, 15, 15, 15, 13, 13, 13, 13, 13, 11, 11, 11, 11, 11, 9, 9, 9, 9, 9, 7},
	{12, 12, 12, 12, 12, 12, 10, 10, 10, 10, 10, 8, 8, 8, 8, 8, 6, 6, 6, 6, 6, 4}},
};
static char save_index[] = {
	0, 0,
	1,
	4,
};
static_assert(sizeof(save_index) / sizeof(save_index[0]) == (SaveVsMagic - SaveVsParalization) + 1, "Invalid count of save index elements");

static char theif_skill_adjustment[][5] = {
	{-25, -20, -15, -30, -20}, // 0
	{-25, -20, -15, -30, -20}, // 1
	{-25, -20, -15, -30, -20}, // 2
	{-25, -20, -15, -30, -20}, // 3
	{-25, -20, -15, -30, -20}, // 4
	{-25, -20, -15, -30, -20}, // 5
	{-25, -20, -15, -30, -20}, // 6
	{-25, -20, -15, -30, -20}, // 7
	{-20, -15, -10, -25, -15}, // 8
	{-15, -10, -10, -20, -10}, // 9
	{-10, -5, -10, -15, -5}, // 10
	{-5, 0, -5, -10, 0}, // 11
	{0, 0, 0, -5, 0}, // 12
	{0, 0, 0, 0, 0}, // 13
	{0, 0, 0, 0, 0}, // 14
	{0, 0, 0, 0, 0}, // 15
	{0, 5, 0, 0, 0}, // 16
	{5, 10, 0, 5, 5}, // 17
	{10, 15, 5, 10, 10}, // 18
	{15, 20, 10, 15, 15}, // 19
};

static char theif_skill_basic[][7] = {
	{40, 10, 0, 0, 0, 0, 0}, // 0
	{85, 10, 15, 25, 30, 20, 0}, // 1
	{86, 10, 21, 29, 35, 25, 0}, // 2
	{87, 15, 27, 33, 40, 30, 0}, // 3
	{88, 15, 33, 37, 45, 35, 20}, // 4
	{90, 20, 40, 42, 50, 40, 25}, // 5
	{92, 20, 47, 47, 55, 45, 30}, // 6
	{94, 25, 55, 52, 60, 50, 35}, // 7
	{96, 25, 62, 57, 65, 55, 40}, // 8
	{98, 30, 70, 62, 70, 60, 45}, // 9
	{99, 30, 78, 67, 80, 65, 50}, // 10
	{99, 35, 86, 72, 90, 70, 55}, // 11
	{99, 35, 94, 77, 95, 75, 60}, // 12
	{99, 40, 99, 82, 99, 80, 65}, // 13
	{99, 40, 99, 87, 99, 85, 70}, // 14
	{99, 50, 99, 92, 99, 90, 75}, // 15
	{99, 50, 99, 97, 99, 95, 80}, // 16
	{99, 55, 99, 99, 99, 99, 80}, // 17
};

static int experience_paladin[21] = {
	0, 0, 2250, 4500, 9000, 18000, 36000, 75000, 150000, 300000,
	600000, 900000, 1200000, 1500000, 1800000, 2100000, 2400000, 2700000, 3000000, 3300000,
	3600000
};
static int experience_warrior[21] = {
	0, 0, 2000, 4000, 8000, 16000, 32000, 64000, 125000, 250000,
	500000, 750000, 1000000, 1250000, 1500000, 1750000, 2000000, 2250000, 2500000, 2750000,
	3000000
};
static int experience_wizard[21] = {
	0, 0, 2500, 5000, 10000, 20000, 40000, 60000, 90000, 135000,
	250000, 375000, 750000, 1125000, 1500000, 1875000, 2250000, 2625000, 3000000, 3375000, 3750000
};
static int experience_priest[21] = {
	0, 0, 1500, 3000, 6000, 13000, 27500, 55000, 110000, 225000,
	450000, 675000, 900000, 1125000, 1350000, 1575000, 1800000, 2025000, 2250000, 2475000, 2700000
};
static int experience_rogue[21] = {
	0, 0, 1250, 2500, 5000, 10000, 20000, 40000, 70000, 110000,
	160000, 220000, 440000, 660000, 880000, 1100000, 1320000, 1540000, 1760000, 1980000, 2200000
};

creature characters[32];
creature* player;
creature* party[6];

static classn class_data[FighterMageTheif + 1][3] = {
	{Fighter},
	{Ranger},
	{Paladin},
	{Mage},
	{Cleric},
	{Theif},
	{Fighter, Cleric},
	{Fighter, Mage},
	{Fighter, Theif},
	{Mage, Theif},
	{Fighter, Mage, Theif},
};

static groupn get_group(classn v) {
	switch(v) {
	case Cleric: return Priests;
	case Mage: return Wizards;
	case Theif: return Rogues;
	default: return Warriors;
	}
}

classn get_class(classn v, int index) {
	return class_data[v][index];
}

static int* get_experience_table(classn v) {
	switch(v) {
	case Cleric: return experience_priest; // Cleric
	case Fighter: return experience_warrior; // Fighter
	case Paladin: case Ranger: return experience_paladin; // Paladin, Ranger
	case Mage: return experience_wizard; // Mage
	default: return experience_rogue; // Rogue
	}
}

int get_class_count(classn v) {
	switch(v) {
	case FighterCleric: case FighterMage: case FighterTheif:
		return 2;
	case FighterMageTheif:
		return 3;
	default:
		return 1;
	}
}

int get_class_index(classn type, classn v) {
	if(v == Fighter) {
		if(class_data[type][0] == Fighter)
			return 0;
	} else {
		for(auto i = 0; i < sizeof(class_data[0]) / sizeof(class_data[0][0]); i++) {
			if(class_data[type][i] == v)
				return i;
		}
	}
	return -1;
}

int get_party_index(const creature* player) {
	return 0;
}

void update_player() {

}

bool allow(classn type, racen race) {
	switch(race) {
	case Dwarf:
		switch(type) {
		case Ranger: case Paladin: case Mage:
		case FighterMage:
		case MageTheif:
		case FighterMageTheif:
			return false;
		default:
			return true;
		}
	case Elf:
		switch(type) {
		case Ranger: case Paladin:
		case FighterCleric:
			return false;
		default:
			return true;
		}
	case HalfElf:
		switch(type) {
		case Ranger: case Paladin:
		case FighterMageTheif:
			return false;
		default:
			return true;
		}
	case Halfling:
		switch(type) {
		case Ranger: case Paladin:
		case FighterCleric: case FighterMage:
		case MageTheif: case FighterMageTheif:
			return false;
		default:
			return true;
		}
	default:
		return type >= Fighter && type <= Theif;
	}
}

bool allow(alignmentn alignment, classn type) {
	switch(type) {
	case Paladin:
		return alignment == LawfulGood;
	case Ranger:
		switch(alignment) {
		case LawfulGood: case NeutralGood: case ChaoticGood:
			return true;
		default:
			return false;
		}
	default:
		return true;
	}
}