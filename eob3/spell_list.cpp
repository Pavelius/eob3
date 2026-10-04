#include "action.h"
#include "creature.h"
#include "dungeon.h"
#include "game.h"
#include "math.h"
#include "spell.h"

const int Round = 1;
const int Hours = Round * 60;

creature* caster;

static int rolld(int c, int d, int m = 0) {
	for(auto i = 0; i < c; i++)
		m += roll_dice(d);
	return m;
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

static void armor_effect() {
	if(player->abilities[AC] < 4)
		player->abilities[AC] = 4;
}

static void comprehend_languages_effect() {
	player->languages.data = -1;
}

static void shield_effect() {
	if(player->abilities[AC] < 7)
		player->abilities[AC] += 7;
}

bool creature::apply(spelln spell, int level, bool run) {
	switch(spell) {
	case Armor:
		if(abilities[AC] >= 4)
			return false;
		if(run)
			apply(spell, armor_effect, 8 * Hours);
		break;
	case Bless:
		if(run)
			apply(spell, bless_effect, 5 + level);
		break;
	case BurningHands:
		if(run)
			damage(FireDamage, rolld(1, 3, imin(10, level) * 2), SaveVsMagic, false);
		break;
	case ChillTouch:
		if(run)
			apply(spell, ChillTouchHand, 3 + level);
		break;
	case CureLightWound:
		if(hp >= hpm || isdead())
			return false;
		if(run)
			heal(rolld(1, 8, 0));
		break;
	case ComprehendLanguages:
		if(languages.data == -1)
			return false;
		if(run)
			apply(spell, comprehend_languages_effect, 5 + 5 * level);
		break;
	case DetectEvil:
		if(run)
			apply(spell, SeeCursed, 2 * level);
		break;
	case DetectMagic:
		if(run)
			apply(spell, SeeMagical, 2 * level);
		break;
	case Friends:
		if(charmable())
			return false;
		if(run) {
			reaction = Indifferent;
			// TODO: Need to make reaction roll
		}
		break;
	case MagicMissile:
		if(run) {
			auto count = 1 + (level - 1) / 2;
			damage(ForceDamage, rolld(count, 4, count));
		}
		break;
	case ProtectionFromEvil:
		if(run)
			apply(spell, ControlEvil, 5 + level);
		break;
	case ShockingGrasp:
		if(run)
			apply(spell, ShockDamage, rolld(1, 8, level));
		break;
	case ShieldSpell:
		if(abilities[AC] >= 7)
			return false;
		if(run)
			apply(spell, shield_effect, 5 * level);
		break;
	default:
		return false;
	}
	return true;
}

bool item::apply(spelln spell, int level, bool run) {
	switch(spell) {
	case Mending:
		if(!hits)
			return false;
		if(type == Edible || type == Rod || type == Readable || type == Drinkable)
			return false;
		if(run)
			hits = 0;
		break;
	case PurifyFood:
		if(!hits)
			return false;
		if(type != Edible)
			return false;
		if(run)
			hits = 0;
		break;
	case Identify:
		if(identified)
			return false;
		if(run)
			identified = 1;
		break;
	default:
		return false;
	}
	return true;
}

int get_thrown(spelln spell) {
	switch(spell) {
	case MagicMissile: return 5;
	default: return -1;
	}
}

spelli spell_data[LastSpell + 1] = {
	// Level 1 Cleric spells
	{{1}, Ally, Bless},
	{{1}, Ally, CureLightWound},
	{{1, 2}, You, DetectEvil},
	{{1, 1}, You, DetectMagic},
	{{1, 1}, Ally, ProtectionFromEvil, color(150, 0, 24)},
	{{1}, AllAllyItems, PurifyFood},
	// Level 1 Mage spells
	{{0, 1}, You, Armor, color(50, 205, 50)},
	{{0, 1}, AllEnemy, BurningHands},
	{{0, 1}, SummonWeapon, ChillTouch},
	{{0, 1}, You, ComprehendLanguages},
	{{0, 1}, You, Friends},
	{{0, 1}, AllAllyItems, Identify},
	{{0, 1}, ShootEnemy, MagicMissile},
	{{0, 1}, AllAllyItems, Mending},
	{{0, 1}, Ally, ShieldSpell},
	{{0, 1}, Enemy, ShockingGrasp},
};