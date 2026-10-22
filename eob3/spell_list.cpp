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

bool creature::apply(spelln spell, int level, bool run) {
	switch(spell) {
	case Abjure:
		if(!is(Demon))
			return false;
		if(run) {
			auto chance_roll = imax(5, imin(50 + 5 * (level - this->level()), 95));
			if(chance(chance_roll))
				kill();
		}
		break;
	case Aid:
		if(find_boost(Bless, this))
			return false;
		if(run) {
			hp_aid += roll_dice(1, 8);
			apply(spell, (1 + level) * 5);
		}
		break;
	case Armor:
		if(abilities[AC] >= 4)
			return false;
		if(run)
			apply(spell, 8 * Hours);
		break;
	case Bless:
		if(run)
			apply(spell, 6);
		break;
	case BurningHands:
		if(run)
			damage(FireDamage, rolld(1, 3, imin(10, level) * 2), SaveVsMagic, false);
		break;
	case ChillTouch:
		if(run)
			summon(spell, ChillTouchHand, (3 + level) * 10);
		break;
	case ComprehendLanguages:
		if(languages.data == -1 || isdead())
			return false;
		if(run)
			apply(spell, 5 + 5 * level);
		break;
	case CreateFood:
		if(run)
			addgear(Ration, CommonItem);
		break;
	case CureCriticalWounds:
		if(hp >= hpm || isdead() || is(Undead))
			return false;
		if(run)
			heal(rolld(3, 8, 3));
		break;
	case CureDisease:
		if(!is(DiseaseLevel) || isdead())
			return false;
		if(run)
			abilities[DiseaseLevel] = 0;
		break;
	case CureLightWound:
		if(hp >= hpm || isdead() || is(Undead))
			return false;
		if(run)
			heal(rolld(1, 8, 0));
		break;
	case CureSeriousWounds:
		if(hp >= hpm || isdead() || is(Undead))
			return false;
		if(run)
			heal(rolld(2, 8, 1));
		break;
	case DetectEvil:
		if(run)
			apply(spell, SeeCursed, 2 * level);
		break;
	case DetectMagic:
		if(run)
			apply(spell, SeeMagical, 2 * level);
		break;
	case FlameBlade:
		if(run)
			summon(FlameBlade, FlameBladeHand, (4 + level / 2) * 10);
		break;
	case Friends:
		if(charmable())
			return false;
		if(run) {
			reaction = Indifferent;
			// TODO: Need to make reaction roll
		}
		break;
	case HeroesFeast:
		if(isdead())
			return false;
		if(run) {
			abilities[DiseaseLevel] = 0;
			addgear(RationIron, CommonItem);
		}
		break;
	case MagicalVestment:
		break;
	case MagicMissile:
		if(run) {
			auto count = 1 + (level - 1) / 2;
			damage(ForceDamage, rolld(count, 4, count));
		}
		break;
	case NeutralizePoison:
		if(!is(PoisonLevel))
			return false;
		if(run)
			abilities[PoisonLevel] = 0;
		break;
	case Prayer:
		if(run)
			apply(spell, level * 2);
		break;
	case ProtectionFromEvil:
		if(run)
			apply(spell, ControlEvil, 5 + level);
		break;
	case RemoveParalize:
		if(!is(Paralizing))
			return false;
		if(run) {
			dispell_magic(HoldPerson, this);
			feats.remove(Paralizing);
		}
		break;
	case ResistColdSpell:
		if(is(ResistCold))
			return false;
		if(run)
			apply(spell, ResistCold, level);
		break;
	case ResistFireSpell:
		if(is(ResistFire))
			return false;
		if(run)
			apply(spell, ResistFire, level);
		break;
	case SlowPoison:
		if(is(StoppedPoison))
			return false;
		if(run)
			apply(spell, StoppedPoison, level * 60);
		break;
	case ShockingGrasp:
		if(run)
			damage(ShockDamage, rolld(1, 8, level));
		break;
	case ShieldSpell:
		if(abilities[AC] >= 7)
			return false;
		if(run)
			apply(spell, 5 * level);
		break;
	case TrueSeeing:
		if(is(ImmuneIllusion))
			return false;
		if(run)
			apply(TrueSeeing, ImmuneIllusion, level * 5);
		break;
	default:
		return false;
	}
	return true;
}

bool item::apply(spelln spell, int level, bool run) {
	wearn wear;
	switch(spell) {
	case Mending:
		if(!hits)
			return false;
		wear = item_data[type].wear;
		if(wear == Edible || wear == Readable || wear == Drinkable || is_spell_power(type) || is_faithable(type))
			return false;
		if(run)
			hits = 0;
		break;
	case PurifyFood:
		if(!hits)
			return false;
		wear = item_data[type].wear;
		if(wear != Edible)
			return false;
		if(run)
			hits = 0;
		break;
	case RemoveCurse:
		if(!cursed())
			return false;
		if(run)
			power = NoPower;
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

bool dungeoni::apply(spelln spell, int level) {
	switch(spell) {
	case FindTraps:
		loc->add(points, CellButton, true);
		break;
	default:
		return false;
	}
	return points.operator bool();
}

int get_thrown(spelln spell) {
	switch(spell) {
	case MagicMissile: return 5;
	case HoldPerson: return 6;
	default: return -1;
	}
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

static void prayer_effect() {
	player->add(AttackMelee, 2);
	player->add(AttackRange, 2);
	player->add(DamageMelee, 2);
	player->add(DamageRange, 2);
	player->add(SaveVsMagic, 2);
	player->add(SaveVsParalization, 2);
	player->add(SaveVsPoison, 2);
	player->add(SaveVsTraps, 2);
	player->add(AC, 2); // Enemies -2 to attack.
}

static void armor_effect() {
	if(player->abilities[AC] < 4)
		player->abilities[AC] = 4;
}

static void magical_vestment_effect() {
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

static void aid_stop() {
	player->hp_aid = 0;
}

spelli spell_data[LastSpell + 1] = {
	// Level 1 Cleric spells
	{{1}, Ally, Bless, color(200, 200, 30), bless_effect},
	{{1}, Ally, CureLightWound},
	{{1, 2}, AllAlly, DetectEvil},
	{{1, 1}, AllAlly, DetectMagic},
	{{1, 1}, Ally, ProtectionFromEvil, color(150, 0, 24)},
	{{1}, AllAllyItems, PurifyFood},
	// Level 2 Cleric spells
	{{2}, Ally, Aid, color(200, 200, 30), bless_effect, aid_stop},
	{{2}, SummonWeapon, FlameBlade},
	{{2}, ShowArea, FindTraps},
	{{2, 3}, Enemy, HoldPerson},
	{{2}, Ally, ResistFireSpell},
	{{2}, Ally, ResistColdSpell},
	{{2}, Ally, SlowPoison},
	// Level 3 Cleric spells
	{{3}, You, CreateFood},
	{{3}, Ally, CureDisease},
	{{3}, You, MagicalVestment, color(94, 77, 178), magical_vestment_effect},
	{{3}, AllAlly, Prayer, color(255, 239, 0), prayer_effect},
	{{3, 4}, YouItem, RemoveCurse},
	{{3}, Ally, RemoveParalize},
	// Level 4 Cleric spells
	{{4}, Enemy, Abjure},
	{{4}, Ally, CloackOfBravery, color(159, 143, 239)},
	{{4}, Ally, CureSeriousWounds},
	{{4}, Enemy, Fear},
	{{4}, Ally, FreeAction, color(94, 77, 178)},
	{{4}, Ally, NeutralizePoison},
	{{4, 3}, AllAlly, ProtectionFromEvil10ft, color(150, 0, 24)},
	// Level 5 Cleric spells
	{{5}, Ally, CureCriticalWounds},
	{{5}, Enemy, DispellEvil},
	{{5}, AllEnemy, FlameStrike},
	{{5}, Ally, RaiseDead},
	{{5}, You, TrueSeeing},
	// Level 6 Cleric spells
	{{6}, AllEnemy, BladeBarrier},
	{{6}, ShowArea, FindPath},
	{{6}, Ally, HealSpell},
	{{6}, AllAlly, HeroesFeast},
	{{6}, You, WorldOfRecall},
	// Level 7 Cleric spells
	{{7}, Ally, Restoration},
	{{7}, AllEnemy, FireStorm},
	{{7}, Ally, Ressurection},
	{{7}, AllEnemy, SunRay},
	{{7}, AllEnemy, SymbolPain},
	// Level 1 Mage spells
	{{0, 1}, You, Armor, color(50, 205, 50), armor_effect},
	{{0, 1}, AllEnemy, BurningHands},
	{{0, 1}, SummonWeapon, ChillTouch},
	{{0, 1}, You, ComprehendLanguages, {}, comprehend_languages_effect},
	{{0, 1}, You, Friends},
	{{0, 1}, YouItem, Identify},
	{{0, 1}, Enemy, MagicMissile},
	{{0, 1}, AllAllyItems, Mending},
	{{0, 1}, Ally, ShieldSpell, {}, shield_effect},
	{{0, 1}, Enemy, ShockingGrasp},
};