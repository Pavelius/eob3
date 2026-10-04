#include "action.h"
#include "creature.h"
#include "dungeon.h"
#include "game.h"
#include "math.h"
#include "spell.h"

const int Round = 1;
const int Hours = Round * 60;

item* last_item;

static int roll(int c, int d, int m = 0) {
	for(auto i = 0; i < c; i++)
		m += roll_dice(d);
	return m;
}

static bool if_wounded() {
	return player->hp < player->hpm && player->hp > -10;
}

static void cure_light_wounds(int level) {
	player->heal(roll(1, 8, 0));
}

static void bless_effect() {
	player->add(AttackMelee, 1);
	player->add(AttackRange, 1);
	player->add(DamageMelee, 1);
	player->add(DamageRange, 1);
	player->add(SaveVsMagic, 1);
	player->add(SaveVsParalization, 1);
	player->add(SaveVsPoison, 1);
	player->add(SaveVsTraps, 1);
}
static void bless(int level) {
	apply(bless_effect, 5 + level);
}

static void detect_evil(int level) {
	apply(0, 2 * level, SeeCursed);
}

static void detect_magic(int level) {
	apply(0, 2 * level, SeeMagical);
}

static void protection_from_evil(int level) {
	apply(0, 5 + level, ProtectionFromEvil);
}

static bool if_edible() {
	return last_item->is(Edible);
}

static void purify_food(int level) {
	last_item->hits = 0;
}

static void armor_effect() {
	if(player->abilities[AC] < 4)
		player->abilities[AC] = 4;
}
static void armor(int level) {
	apply(armor_effect, Hours * 8);
}

static void burning_hands(int level) {
	apply(FireDamage, roll(1, 3, imin(10, level) * 2), SaveVsMagic);
}

static void chill_touch(int level) {
	summon(ChillTouchHand, 3 + level);
}

static void comprehend_languages_effect() {
	player->languages.data = -1;
}
static void comprehend_languages(int level) {
	apply(comprehend_languages_effect, 5 + 5 * level);
}

static void shocking_grasp(int level) {
	apply(ShockDamage, roll(1, 8, level));
}

static void shield_effect() {
	if(player->abilities[AC] < 7)
		player->abilities[AC] += 7;
}
static void shield(int level) {
	apply(shield_effect, 5 * level);
}

//Friends levels(0 1)
//feats Enemy Group SummaryEffect
//filter IfIntelligence ImmuneCharm - 1
//instant Indifferent MonstersReaction Roll2d4 ReactionCheck + 101
//Identify levels(0 1)
//feats Ally You
//filter_item IfItemIdentified - 1
//instant IdentifyItem
//MagicMissile levels(0 1) avatar_thrown(5)
//feats Enemy
//instant Roll1d4p1x1d4p1s3p2c9 Magic + 101
//Mending levels(0 1)
//feats Ally You
//filter_item IfItemEdible - 1 IfItemCharged - 1 IfItemDamaged
//instant DamageItem - 1 EffectCount + 1

spelli spells[] = {
	// Level 1 Cleric spells
	{{1}, Ally, bless},
	{{1}, Ally, cure_light_wounds, if_wounded},
	{{1, 2}, You, detect_evil},
	{{1, 1}, You, detect_magic},
	{{1, 1}, Ally, protection_from_evil, 0, color(150, 0, 24)},
	{{1}, AllAllyItems, purify_food, if_edible},
	// Level 1 Mage spells
	{{0, 1}, You, armor, 0, color(50, 205, 50)},
	{{0, 1}, AllEnemy, burning_hands},
	{{0, 1}, SummonWeapon, chill_touch},
	{{0, 1}, You, comprehend_languages},
	{{0, 1}, Ally, shield},
	{{0, 1}, Enemy, shocking_grasp},
};