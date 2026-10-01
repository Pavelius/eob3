#include "item.h"
#include "game.h"
#include "rand.h"

static featn sword_powers[] = {Magical, Magical2, Magical3, Magical4, Magical5};
static itemn random_ration[] = {Ration, Ration, Ration, RationIron};
static itemn random_ring[] = {BlueRing, GreenRing, RedRing};
static itemn random_treasure[] = {BlueGem, BlueGem, BlueGem, BlueGem, BlueGem, GreenGem, GreenGem, GreenGem, RedGem, RedGem, PurpleGem};
static itemn random_small_item[] = {TheifTools, MageBook, PriestScroll, MageScroll, MagicMap, RandomTreasure, Wand, RandomRing, Amulet, IceSphere, FlameSphere, SteelKey};
static itemn random_item[] = {RandomWeapon, RandomSmallItem};
static itemn random_weapon[] = {
	BattleAxe, Axe, Dagger, Halberd, WarHammer, Mace, Spear, Longsword, ShortSword, TwoHandedSword, Bow,
};

item shops[LastShop + 1][6];

featn get_powers(itemn type) {
	switch(type) {
	case ShortSword: case Longsword: case TwoHandedSword: return SwordPower;
	default: return NoPower;
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
	default: return v;
	}
}

static bool can_be_mundane(featn power) {
	switch(power) {
	case SwordPower: return true;
	default: return false;
	}
}

void item::createpower(int chance_power, int chance_cursed) {
	auto result = get_powers(type);
	if(can_be_mundane(result)) {
		if(!chance(chance_power))
			return;
	}
	power = random(result);
}

static void refresh_shop(shopn id, itemn type) {
	for(auto& e : shops[id]) {
		if(e)
			continue;
		e.clear();
		e.type = random(type);
		e.createpower(100, 0);
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