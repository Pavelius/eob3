#include "creature.h"
#include "math.h"
#include "rand.h"
#include "stringbuilder.h"

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
static char race_minimum[Halfling + 1][6] = {
	{3, 3, 3, 3, 3, 3},
	{8, 3, 11, 3, 3, 3},
	{3, 6, 7, 8, 3, 8},
	{3, 6, 6, 4, 3, 3},
	{7, 7, 10, 6, 3, 3},
};
static char race_maximum[Halfling + 1][6] = {
	{18, 18, 18, 18, 18, 18},
	{18, 17, 18, 18, 18, 17},
	{18, 18, 18, 18, 18, 18},
	{18, 18, 18, 18, 18, 18},
	{18, 18, 18, 18, 17, 18},
};
static char class_minimum[Theif + 1][6] = {
	{9, 0, 0, 0, 0, 0}, // Fighter
	{13, 13, 14, 0, 14, 0}, // Ranger
	{12, 0, 9, 0, 13, 17}, // Paladin
	{0, 0, 0, 9, 0, 0}, // Mage
	{0, 0, 0, 0, 9, 0}, // Cleric
	{0, 9, 0, 0, 0, 0}, // Theif
};

creature characters[32];
creature* player;
creature* adventurers[6];

static classn class_data[FighterMageTheif + 1][3] = {
	{Monster},
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

void creature::clear() {
	memset((void*)this, 0, sizeof(*this));
	name_id = 0xFF;
	avatar = 0xFF;
}

void creature::add(abilityn n, int v) {
	v += abilities[n];
	if(v < -120)
		v = -120;
	else if(v > 120)
		v = 120;
	abilities[n] = (char)v;
}

void creature::setframe(short* frames, short index) const {
	if(monster) {
		auto po = monsters[monster].overlays;
		frames[0] = po[0] * 6 + index;
		frames[1] = po[1] ? po[1] * 6 + index : 0;
		frames[2] = po[2] ? po[2] * 6 + index : 0;
		frames[3] = po[3] ? po[3] * 6 + index : 0;
	} else {
		frames[0] = index;
		frames[1] = 0;
	}
}

bool creature::allow(itemn type) const {
	switch(type) {
	case ShortSword: case Longsword:
		return is(Theif) || is(Elf) || is(Fighter) || is(Paladin) || is(Ranger);
	case Dagger:
		return !is(Cleric);
	case WarHammer: case Mace: case Club:
		return !is(Mage);
	case Axe:
		return is(Theif) || is(Fighter) || is(Paladin) || is(Ranger);
	case TwoHandedSword: case BattleAxe: case Halberd:
		return is(Fighter) || is(Paladin) || is(Ranger);
	case LeatherArmor:
		return !is(Mage);
	case ScaleMail: case ChainMail: case BandedMail: case PlateMail:
	case Shield: case DwarvenShield:
	case Helm: case DwarvenHelm:
		return is(Fighter) || is(Paladin) || is(Ranger) || is(Cleric);
	case HolySymbol: case HolySymbolEvil: case PriestScroll:
		return is(Cleric);
	case Wand: case IceSphere: case FlameSphere: case MageScroll: case MageBook:
		return is(Mage);
	case TheifTools:
		return is(Theif);
	default:
		return true;
	}
}

void creature::equip(item& v) {
	if(!allow(v.type))
		return;
	for(auto i = Head; i <= Quiver; i = (wearn)(i + 1)) {
		if(wears[i])
			continue;
		if(!::allow(v.type, i))
			continue;
		wears[i] = v;
		v.clear();
		last_item = &wears[i];
		break;
	}
}


static abilityn get_primary(classn v) {
	switch(v) {
	case Theif: return Dexterity;
	case Cleric: return Wisdow;
	case Mage: return Intellegence;
	default: return Strenght;
	}
}

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
	case FighterCleric: case FighterMage: case FighterTheif: case MageTheif: return 2;
	case FighterMageTheif: return 3;
	default: return 1;
	}
}

int get_class_index(classn type, classn v) {
	for(auto i = 0; i < sizeof(class_data[0]) / sizeof(class_data[0][0]); i++) {
		if(class_data[type][i] == v)
			return i;
	}
	return -1;
}

int get_hit_die(classn type) {
	switch(type) {
	case Fighter: case Paladin: case Ranger: return 10;
	case Theif: return 6;
	case Mage: return 4;
	default: return 8;
	}
}

int get_party_index(const creature* player) {
	return 0;
}

static void update_languages() {
	//player->languages = player->getrace().languages;
	//player->understand(player->race);
	//if(player->getrace().origin)
	//	player->understand(player->getrace().origin);
	//if(player->basic.abilities[Intellegence] >= 10)
	//	player->understand((racen)0); // All creatures with 11+ untellegence known common language
}

static void update_basic() {
	memcpy(player->abilities, player->basic.abilities, ExeptionalStrenght + 1);
	memcpy(&player->feats, &player->basic.feats, sizeof(player->feats));
}

static int get_skill_level(abilityn v) {
//	// For monsters and other special effects
//	if(player->is(v))
//		return player->getlevel();
//	// For multiclass and characters
//	auto& ei = player->getclass();
//	for(auto i = 0; i < ei.count; i++) {
//		if(bsdata<classi>::elements[ei.classes[i]].is(v))
//			return player->levels[i];
//	}
	return 0;
}

static void update_basic_skills() {
	for(auto i = ClimbWalls; i <= ReadLanguages; i = (abilityn)(i + 1)) {
		auto level = imin(imax(0, get_skill_level(i)), 17);
		auto value = theif_skill_basic[level][i - ClimbWalls];
		player->abilities[i] += value;
	}
}

static int get_maximum_hits() {
	auto n = get_class_count(player->type);
	auto m = player->level();
	auto a = player->get(Constitution);
	auto h = maptbl(hit_points_adjustment, a);
	if(h > 2 && !player->is(Fighter))
		h = 2;
	auto r = player->get(Hits) + h * m + player->hpr / imax(1, (int)n);
	if(r < m)
		r = m;
	return r;
}

static bool allow_exeptional_strenght(classn type, racen race) {
	if(race == Halfling)
		return false;
	switch(get_class(type, 0)) {
	case Fighter: case Paladin: case Ranger: return true;
	default: return false;
	}
}

static int get_modified_strenght() {
	auto a = player->get(Strenght);
	auto e = player->get(ExeptionalStrenght);
	if(!allow_exeptional_strenght(player->type, player->race))
		e = 0;
	if(a > 18)
		a += 6;
	else if(a == 18 && e > 0) {
		if(e <= 50)
			a += 1;
		else if(e <= 75)
			a += 2;
		else if(e <= 90)
			a += 3;
		else if(e <= 99)
			a += 4;
		else
			a += 5;
	}
	return a;
}

static void update_ability(abilityn v, int level, int per_level, int minimal) {
	if(level <= 0 || !per_level)
		return;
	auto value = player->abilities[v];
	if(value <= minimal)
		return;
	value -= level / per_level;
	if(value < minimal)
		value = minimal;
	player->abilities[v] = value;
}

static void update_abilities() {
	player->add(Strenght, -player->abilities[DrainedStrenght]);
	auto n = -(player->abilities[DrainedStrenght] + player->abilities[DrainedLevels]);
	player->add(AttackMelee, n);
	player->add(AttackRange, n);
	player->add(Constitution, -player->abilities[DrainedConstitution]);
	auto s = -player->abilities[DrainedLevels] * 5;
	player->add(SaveVsParalization, s);
	player->add(SaveVsPoison, s);
	player->add(SaveVsTraps, s);
	player->add(SaveVsMagic, s);
	auto disease_level = player->abilities[DiseaseLevel];
	update_ability(Charisma, disease_level, 2, 6);
	update_ability(Dexterity, disease_level, 3, 6);
	update_ability(Strenght, disease_level, 5, 3);
}

static void update_theif_skill_by_dexterity() {
	auto a = player->get(Dexterity);
	auto p = maptbl(theif_skill_adjustment, a);
	player->add(PickPockets, p[0]);
	player->add(OpenLocks, p[1]);
	player->add(RemoveTraps, p[2]);
	player->add(MoveSilently, p[3]);
}

static void add_additional_spell(abilityn v) {
	if(player->abilities[v])
		player->abilities[v]++;
}

static void update_additional_spells() {
	if(player->is(Cleric)) {
		// RULE: Priest have their spell count increased, depend on window.
		auto k = player->get(Wisdow);
		if(k >= 13)
			add_additional_spell(Spell1);
		if(k >= 14)
			add_additional_spell(Spell1);
		if(k >= 15)
			add_additional_spell(Spell2);
		if(k >= 16)
			add_additional_spell(Spell2);
		if(k >= 17)
			add_additional_spell(Spell3);
		if(k >= 18)
			add_additional_spell(Spell4);
		if(k >= 19) {
			add_additional_spell(Spell1);
			add_additional_spell(Spell3);
		}
	}
}

static void update_depended_abilities() {
	auto k = get_modified_strenght();
	player->abilities[AttackMelee] += maptbl(hit_probability, k);
	player->abilities[AttackRange] += maptbl(reaction_adjustment, player->abilities[Dexterity]);
	player->abilities[DamageMelee] += maptbl(damage_adjustment, k);
	player->abilities[AC] += maptbl(defence_adjustment, player->abilities[Dexterity]);
	player->abilities[ReactionBonus] += maptbl(cha_reaction_adjustment, player->abilities[Charisma]);
	if(player->is(Mage))
		player->abilities[LearnSpell] += maptbl(chance_learn_spell, player->abilities[Intellegence]);
	if(player->wears[RightHand])
		player->abilities[Speed] += player->wears[RightHand].geti().combat.speed;
	else if(player->wears[LeftHand])
		player->abilities[Speed] += player->wears[LeftHand].geti().combat.speed;
	//else if(player->is(Large))
	//	player->abilities[Speed] += 6;
	else
		player->abilities[Speed] += 3;
	//if(player->is(FeelPain))
	//	player->add(AttackMelee, -4);
	//if(player->is(Blinded)) {
	//	player->add(AttackMelee, -4);
	//	player->add(AttackRange, -4);
	//}
}

static void update_bonus_saves() {
	auto k = player->get(Constitution);
	if(player->is(Dwarf) || player->is(Halfling))
		player->abilities[SaveVsPoison] += maptbl(dwarven_bonus, k) * 5;
	if(player->is(Dwarf) || player->is(Halfling))
		player->abilities[SaveVsMagic] += maptbl(dwarven_bonus, k) * 5;
}

static int magic_wear_value(int magic_bonus) {
	return 17 + magic_bonus;
}

//static void magic_wear(variant v) {
//	if(v.iskind<abilityi>()) {
//		auto m = bsdata<abilityi>::elements[v.value].wearing_multiplier;
//		if(m == 100) {
//			auto k = magic_wear_value(v.counter);
//			auto n = player->abilities[v.value];
//			if(n < k)
//				n = k;
//			player->abilities[v.value] = n;
//		} else
//			player->add((abilityn)v.value, v.counter * m);
//	} else if(v.iskind<spelli>()) {
//		// Nothing to do. Camp spell add.
//	} else
//		script_run(v);
//}

static void update_wear() {
	for(auto& e : player->equipment()) {
		if(!e)
			continue;
		auto& ei = e.geti();
		if(ei.wear == LeftHand) {
			if(player->wears[RightHand] && player->wears[RightHand].is(TwoHanded))
				continue; // RULE: Two handed weapon
		}
		//if(ei.wearing)
		//	script_run(ei.wearing);
		//if((ei.wear >= Head && ei.wear <= Legs) // If wearable equipment only!
		//	&& ei.wear != LeftHand && ei.wear != RightHand) {
		//	auto power = e.getpower();
		//	if(power)
		//		magic_wear(power);
		//}
	}
}

static void apply_boost(short type, short param) {
	//if(type <= Hits)
	//	player->add((abilityn)type, param);
	//else if(type == BoostSpell)
	//	ftscript<spelli>(param, 0);
	//else if(type == BoostFeat)
	//	ftscript<feati>(param, 0);
	//else {
	//	// TODO: error for debuging
	//}
}

static void update_duration() {
	//auto push_modifier = modifier; modifier = Wearing;
	//referencei target = player;
	//for(auto& e : bsdata<boosti>()) {
	//	if(e.target == target)
	//		apply_boost(e.type, e.param);
	//}
	//modifier = push_modifier;
}

static bool have_boost_summon(const item& it) {
	//referencei target = player;
	//for(auto& e : bsdata<boosti>()) {
	//	if(e.target == target && e.type == BoostSpell) {
	//		if(it.is(bsdata<spelli>::elements[e.param].summon))
	//			return true;
	//	}
	//}
	return false;
}

static void update_summon() {
	for(auto& e : player->wears) {
		if(e.is(SummonedItem) && !have_boost_summon(e)) {
			auto w = e.geti().wear;
			e.clear();
			//if(w == RightHand)
			//	change_quick_item(player, RightHand);
		}
	}
}

static int get_thac0_value() {
	auto count = get_class_count(player->type);
	auto result = 0;
	for(auto i = 0; i < count; i++) {
		auto type = get_class(player->type, i);
		auto level = imin(imax(0, (int)player->levels[i]), 21);
		auto save_group = get_group(type);
		auto value = thac0_advance[save_group][level];
		if(value > result)
			result = value;
	}
	return result;
}

static int get_save_value(int save_index_value) {
	auto count = get_class_count(player->type);
	auto result = 20;
	for(auto i = 0; i < count; i++) {
		auto type = get_class(player->type, i);
		auto level = imin(imax(0, (int)player->levels[i]), 21);
		auto save_group = get_group(type);
		auto value = saves_advance[save_group][save_index_value][level];
		if(value < result)
			result = value;
	}
	return (20 - result + 1) * 5;
}

static void update_saves() {
	for(auto i = SaveVsParalization; i <= SaveVsMagic; i = (abilityn)(i + 1))
		player->add(i, get_save_value(save_index[i - SaveVsParalization]));
}

static void update_thac0() {
	auto value = get_thac0_value();
	player->add(AttackMelee, value);
	player->add(AttackRange, value);
}

static void update_bonus_experience() {
	auto primary = get_primary(get_class(player->type, 0));
	if(player->get(primary) >= 16)
		player->add(BonusExperience, 1);
}

void update_player() {
	update_basic();
	update_languages();
	update_summon();
	update_wear();
	update_duration();
	update_saves();
	update_thac0();
	update_basic_skills();
	update_abilities();
	update_depended_abilities();
	update_additional_spells();
	update_theif_skill_by_dexterity();
	update_bonus_saves();
	update_bonus_experience();
	player->hpm = get_maximum_hits();
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

static int compare_char_desc(const void* v1, const void* v2) {
	return *((char*)v2) - *((char*)v1);
}

static int get_best_4d6() {
	char result[4];
	for(size_t i = 0; i < sizeof(result) / sizeof(result[0]); i++)
		result[i] = (rand() % 6) + 1;
	qsort(result, sizeof(result) / sizeof(result[0]), sizeof(result[0]), compare_char_desc);
	return result[0] + result[1] + result[2];
}

static int get_best_index(char* result, size_t size) {
	auto result_index = 0;
	for(size_t i = 0; i < size; i++) {
		if(result[i] > result[result_index])
			result_index = i;
	}
	return result_index;
}

static void apply_minimal(char* abilities, const char* minimal) {
	for(auto i = 0; i < 6; i++) {
		if(minimal[i] && abilities[Strenght + i] < minimal[i])
			abilities[Strenght + i] = minimal[i];
	}
}

static void apply_maximal(char* abilities, const char* maximal) {
	for(auto i = 0; i < 6; i++) {
		if(maximal[i] && abilities[Strenght + i] > maximal[i])
			abilities[Strenght + i] = maximal[i];
	}
}

static void standart_ability() {
	for(size_t i = 0; i < 6; i++)
		player->basic.abilities[Strenght + i] = 10;
}

void reroll_ability() {
	char result[12] = {};
	if(true) {
		for(size_t i = 0; i < sizeof(result) / sizeof(result[0]); i++)
			result[i] = (rand() % 6) + (rand() % 6) + (rand() % 6) + 3;
		qsort(result, sizeof(result) / sizeof(result[0]), sizeof(result[0]), compare_char_desc);
		zshuffle(result, 6);
	} else {
		for(size_t i = 0; i < 6; i++)
			result[i] = get_best_4d6();
	}
	for(size_t i = 0; i < 6; i++)
		player->basic.abilities[Strenght + i] = result[i];
	auto primary = get_primary(player->type);
	auto base_class = get_class(player->type, 0);
	auto race = player->race;
	iswap(player->basic.abilities[get_best_index(player->basic.abilities + Strenght, 6)], player->basic.abilities[primary]);
	apply_minimal(player->basic.abilities, class_minimum[base_class]);
	apply_minimal(player->basic.abilities, race_minimum[race]);
	apply_maximal(player->basic.abilities, race_maximum[race]);
	player->basic.abilities[ExeptionalStrenght] = d100() + 1;
}

void reroll_hits() {
	auto n = get_class_count(player->type);
	player->hpr = 0;
	for(char i = 0; i < n; i++) {
		auto die = get_hit_die(get_class(player->type, i));
		auto value = 1 + rand() % die;
		if(value < die / 2)
			value = die / 2;
		player->hpr += value;
	}
}

void reroll_character() {
	player->name_id = random_name(player->race, player->gender);
	reroll_ability();
	reroll_hits();
	player->update();
}

void creature::update() {
	auto push = player; player = this;
	update_player();
	player = push;
}

static bool specialized(itemn type, racen race) {
	switch(race) {
	case Dwarf: return type == BattleAxe || type == Mace;
	case Elf: return type == Longsword || type == ShortSword;
	case HalfElf: return type == Longsword || type == ShortSword;
	case Halfling: return type == ShortSword || type == Dagger;
	default: return type == Longsword || type == TwoHandedSword;
	}
}

combati creature::getattack(wearn id, bool large_enemy) const {
	auto weapon = wears[id].type;
	auto result = wears[id].geti().combat;
	if(large_enemy && result.large)
		result.damage = result.large;
	auto isranged = wears[id].isranged();
	result.attack += player->get(isranged ? AttackRange : AttackMelee);
	result.damage.b += player->get(isranged ? DamageRange : DamageMelee);
	// RULE: Single player fighter have bonus speñialization
	if(type == Fighter && specialized(weapon, race)) {
		if(isranged)
			result.attack += 2;
		else {
			result.attack += 1;
			result.damage.b += 2;
		}
	}
	// RULE: Elves gain bonus to attack with elvish weapon
	if(race == Elf && (weapon == Longsword || weapon == ShortSword))
		result.attack += 1;
	auto magic = get_magic(wears[id].power);
	result.attack += magic;
	result.damage.b += magic;
	return result;
}

static const char* str(const dice& v) {
	static char temp[32]; stringbuilder sb(temp);
	sb.add("%1i-%2i", v.minimum(), v.maximum());
	return temp;
}

const char* creature::strvalue(abilityn id) const {
	switch(id) {
	case AttackMelee: return str("%1i", 20 - getattack(RightHand, false).attack);
	case DamageMelee: return str(getattack(RightHand, false).damage);
	case AC: return str("%1i", 10 - get(id));
	case Hits: return str("%1i", hpm);
	case ReactionBonus: return str("%+1i", get(id));
	case Strenght:
		if(get(id) == 18) {
			auto exeptional = player->get(ExeptionalStrenght);
			if(exeptional == 100)
				return "18/00";
			else
				return str("18/%1.2i", exeptional);
		} else
			return str("%1i", get(id));
	default:
		return str("%1i", get(id));
	}
}

static bool no_party_name(unsigned char v) {
	for(auto i = 0; i < 4; i++) {
		if(characters[i].avatar == v)
			return false;
	}
	return true;
}

unsigned char random_name(racen race, gendern gender) {
	unsigned char source[250];
	auto count = select_names(source, race, gender, no_party_name);
	if(!count)
		return 0xFF;
	return source[rand() % count];
}

bool no_party_avatar(unsigned char v) {
	for(auto i = 0; i < 4; i++) {
		if(characters[i].avatar == v)
			return false;
	}
	return true;
}

unsigned char random_avatar(racen race, gendern gender, classn type) {
	unsigned char source[250];
	auto count = select_avatars(source, race, gender, type, no_party_avatar);
	if(!count)
		return 0xFF;
	return source[rand() % count];
}

creature* new_character() {
	for(auto& e : characters) {
		if(e.avatar == 0xFF)
			return &e;
	}
	return 0;
}

creature* get_creature(void* pointer) {
	if(pointer >= characters && pointer < characters + lengthof(characters))
		return characters + ((creature*)pointer - characters);
	return 0;
}

static void add_magical(itemn type) {
}

static void start_equipment() {
	switch(get_class(player->type, 0)) {
	case Fighter:
	case Paladin:
	case Ranger:
		if(player->is(Dwarf))
			player->equip(BattleAxe);
		else
			player->equip(Longsword);
		player->equip(LeatherArmor);
		player->equip(Shield);
		player->equip(DwarvenHelm);
		break;
	case Cleric:
		player->equip(Mace);
		player->equip(LeatherArmor);
		break;
	case Mage:
		player->equip(Dagger);
		add_magical(Wand);
		break;
	default:
		player->equip(Dagger);
		player->equip(LeatherArmor);
		break;
	}
}

void finish_character() {
	if(!player->monster)
		start_equipment();
}

void create_charater(racen race, gendern gender, classn class_type, alignmentn alignment) {
	player->clear();
	player->race = race;
	player->gender = gender;
	player->type = class_type;
	player->alignment = alignment;
	player->avatar = random_avatar(race, gender, class_type);
	reroll_character();
	finish_character();
}

void create_monster(monstern type) {
	const auto& e = monsters[type];
	player->clear();
	player->race = e.race;
	player->gender = Male;
	player->type = Fighter;
	player->alignment = e.alignment;
	standart_ability();
	reroll_hits();
	player->update();
	finish_character();
}