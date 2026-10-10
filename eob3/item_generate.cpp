#include "item.h"
#include "game.h"
#include "rand.h"
#include "spell.h"

static featn sword_powers[] = {
	Magical, Magical, Magical, Magical, Magical, Magical, Magical,
	Magical2, Magical2, Magical2, Magical2,
	Magical3, Magical3,
	Magical4, Magical5,
	DispelEvil, Holy, Vampiric, Vorpal, Flaming, Freezing,
	ControlGoblinoid};
static featn melee_weapon_powers[] = {
	Magical, Magical, Magical, Magical, Magical, Magical,
	Magical2, Magical2, Magical2, Magical2,
	Magical3, Magical3,
	Magical4,
	DispelEvil, Holy, Flaming, Freezing};
static itemn random_ration[] = {
	Ration, Ration, Ration,
	RationIron};
static itemn random_ring[] = {BlueRing, GreenRing, RedRing};
static itemn random_treasure[] = {
	BlueGem, BlueGem, BlueGem, BlueGem, BlueGem,
	GreenGem, GreenGem, GreenGem,
	RedGem, RedGem,
	PurpleGem};
static itemn random_small_item[] = {
	TheifTools, TheifTools,
	MageBook, PriestScroll, PriestScroll, MageScroll, MageScroll, MageScroll, MagicMap,
	RandomTreasure, Wand, RandomRing, Amulet,
	SteelKey, SteelKey, SteelKey};
static itemn random_item[] = {RandomWeapon, RandomSmallItem};
static itemn random_weapon[] = {
	BattleAxe, BattleAxe, Axe, Axe, Dagger, Dagger,
	Halberd, WarHammer, Mace, Spear,
	Longsword, ShortSword, ShortSword, TwoHandedSword,
	Bow, Sling};
static spelln mage_spell_1[] = {
	MagicMissile, Armor, BurningHands, ChillTouch, ComprehendLanguages, Friends, Identify, MagicMissile, Mending, ShieldSpell, ShockingGrasp
};
static spelln priest_spell_1[] = {
	CureLightWound, DetectEvil, DetectMagic, PurifyFood,
};
static spelln wand_spells[] = {
	MagicMissile, MagicMissile, MagicMissile, BurningHands, BurningHands, BurningHands,
	ShockingGrasp, ShockingGrasp, DetectMagic,
};

item shops[LastShop + 1][6];

featn get_powers(itemn type) {
	switch(type) {
	case ShortSword: case Longsword: case TwoHandedSword: case Dagger:
		return SwordPower;
	case Mace: case WarHammer: case Flail:
		return MeleeWeaponPower;
	case Spear: case Staff:
		return MeleeWeaponPower;
	case Halberd: case Axe: case BattleAxe:
		return MeleeWeaponPower;
	default:
		return NoPower;
	}
}

itemn random(itemn v) {
	switch(v) {
	case RandomWeapon: return random(maprnd(random_weapon));
	case RandomRation: return random(maprnd(random_ration));
	case RandomRing: return random(maprnd(random_ring));
	case RandomTreasure: return random(maprnd(random_treasure));
	case RandomItem: return random(maprnd(random_item));
	case RandomSmallItem: return random(maprnd(random_small_item));
	default: return v;
	}
}

featn random(featn v) {
	switch(v) {
	case SwordPower: return maprnd(sword_powers);
	case MeleeWeaponPower: return maprnd(melee_weapon_powers);
	default: return v;
	}
}

static int random_spell_level() {
	switch(d20()) {
	case 7: case 8: case 9: case 10: return 2;
	case 11: case 12: case 13: return 3;
	case 14: case 15: return 4;
	case 16: return 5;
	case 17: return 6;
	case 18: return 7;
	default: return 1;
	}
}

static spelln random_priest_spell(int level) {
	switch(level) {
	case 1: return maprnd(priest_spell_1);
	default: return CureLightWound;
	}
}

static spelln random_mage_spell(int level) {
	switch(level) {
	case 1: return maprnd(mage_spell_1);
	default: return MagicMissile;
	}
}

static spelln random_mage_spell() {
	return random_mage_spell(random_spell_level());
}

static bool can_be_mundane(featn power) {
	switch(power) {
	case SwordPower: case MeleeWeaponPower:
		return true;
	default:
		return false;
	}
}

void item::create(int chance_power, int chance_cursed) {
	switch(type) {
	case MageScroll: power = (featn)random_mage_spell(); break;
	case PriestScroll: power = (featn)random_mage_spell(); break;
	case Wand: power = (featn)maprnd(wand_spells); break;
	default:
		if(chance(chance_cursed))
			power = Cursed;
		else {
			auto result = get_powers(type);
			if(can_be_mundane(result)) {
				if(!chance(chance_power))
					return;
			}
			power = random(result);
		}
		break;
	}
}

static void refresh_shop(shopn id, itemn type) {
	for(auto& e : shops[id]) {
		if(e)
			continue;
		e.clear();
		e.type = random(type);
		e.create(100, 0);
		e.identified = 1;
	}
}

void refresh_shop(shopn id) {
	switch(id) {
	case WeaponShop: refresh_shop(id, RandomWeapon); break;
	default: break;
	}
}

bool allow(shopn v) {
	for(auto& e : shops[v]) {
		if(e)
			return true;
	}
	return false;
}

void refresh_shops() {
	for(auto i = (shopn)0; i <= LastShop; i = (shopn)(i + 1))
		refresh_shop(i);
}

void normalize_shop(shopn id) {
	auto ps = shops[id];
	for(auto& e : shops[id]) {
		if(e)
			*ps++ = e;
	}
	for(auto pe = shops[id] + lengthof(shops[id]); ps < pe; ps++)
		ps->clear();
}