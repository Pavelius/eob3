#include "action.h"
#include "adat.h"
#include "answers.h"
#include "console.h"
#include "creature.h"
#include "dungeon.h"
#include "game.h"
#include "math.h"
#include "rand.h"

static adat<creature*, 16> combatants;
static int enemy_distance;

void turnto(pointc v, directionn d, bool test_surprise);

static featn get_resist(damagen v) {
	switch(v) {
	case FireDamage: return ResistFire;
	case ColdDamage: return ResistCold;
	case Bludgeon: return ResistBludgeon;
	case Piercing: return ResistPiercing;
	case Slashing: return ResistSlashing;
	default: return (featn)0;
	}
}

static featn get_immunity(damagen v) {
	switch(v) {
	case Bludgeon: case Slashing: case Piercing: return ImmuneNormalWeapon;
	case IllDamage: return ImmuneNormalWeapon;
	default: return (featn)0;
	}
}

static int get_hit_points(celln t) {
	switch(t) {
	case CellWeb: return 4; // Easy to hit. Affected by fire spells.
	case CellCocon: return 5;
	case CellBarel: return 7; // Tought to hit. Crushed by acid spells.
	case CellEyeColumn: return 10;
	default: return 0;
	}
}

static size_t shrink_creatures(creature** dest, creature** units, size_t count) {
	auto ps = dest;
	auto pb = units;
	auto pe = units + count;
	while(pb < pe) {
		if(*pb)
			*ps++ = *pb;
		pb++;
	}
	return ps - dest;
}

static int compare_creatures(const void* v1, const void* v2) {
	return (*((creature**)v1))->initiative - (*((creature**)v2))->initiative;
}

static bool select_combatants(pointc position) {
	loc->getmonsters(combatants.data, position);
	combatants.count = shrink_creatures(combatants.data, combatants.data, 6);
	if(!combatants)
		return false;
	combatants.count += shrink_creatures(combatants.data + combatants.count, adventurers, 6);
	// Lowest initiative win, so positive speed is bad
	for(auto p : combatants) {
		if(p->is(SlowMove))
			p->initiative = 30 + xrand(1, 10);
		else if(p->is(Surprised))
			p->initiative = 60 + xrand(1, 10); // Move last
		else
			p->initiative = xrand(1, 10) + p->get(Speed);
	}
	qsort(combatants.data, combatants.count, sizeof(combatants.data[0]), compare_creatures);
	return true;
}

static bool select_combatants(pointc v, directionn d) {
	enemy_distance = 0;
	for(auto i = 0; i < 3; i++) {
		v = to(v, d);
		if(!v)
			return false;
		if(select_combatants(v)) {
			enemy_distance = i + 1;
			turnto(v, to(d, Down), false);
			return true;
		}
	}
	return false;
}

static void select_combatants(creature** result, bool enemies) {
	memset(result, 0, sizeof(result[0]) * 6);
	for(auto p : combatants) {
		if(p->isdisabled())
			continue;
		auto ismonster = (p->monster != NoMonster);
		if(ismonster != enemies)
			continue;
		if(ismonster) {
			p->reaction = Hostile;
			auto side = get_side(p->side, party.d);
			if(!result[side])
				result[side] = p;
		} else {
			if(!result[p->side])
				result[p->side] = p;
		}
	}
}

static creature* get_opponent(bool left, bool enemies) {
	creature* result[6]; select_combatants(result, enemies);
	for(auto y = 0; y < 3; y++) {
		auto n = left ? 0 : 1;
		if(d100() < 30) // Randomly select nearest
			n = n ? 0 : 1;
		auto p = result[y * 2 + n];
		if(p)
			return p;
		p = result[y * 2 + (n + 1) % 2];
		if(p)
			return p;
	}
	return 0;
}

static void drain_attack(creature* defender, const item& weapon, featn type, abilityn ability, int save_bonus) {
	if(!player->is(weapon, type))
		return;
	if(player->is(Undead) && defender->is(ControlEvil))
		return;
	if(defender->roll(SaveVsMagic, save_bonus))
		return;
	defender->add(ability, 1);
	auto drain_death = (defender->get(DrainedLevels) >= defender->level())
		|| (defender->get(DrainedStrenght) >= defender->basic.abilities[Strenght])
		|| (defender->get(DrainedConstitution) >= defender->basic.abilities[Constitution]);
	if(drain_death) {
		if(defender->monster)
			defender->kill();
		else
			defender->hp = -10;
	}
}

static void hit_equipment(creature* player) {
	static wearn equipment[] = {Body, LeftHand, Head, Elbow, Legs, Neck};
	if(player->isdisabled())
		return;
	for(auto w : equipment) {
		if(!player->wears[w] || player->wears[w].natural())
			continue;
		if(player->roll(SaveVsParalization))
			continue;
		player->wears[w].damage(0, 1);
		break;
	}
}

static void single_attack(creature* defender, wearn slot, int bonus, int multiplier) {
	if(!defender || defender->isdisabled())
		return;
	auto& weapon = player->wears[slot];
	if(!weapon.isweapon())
		return;
	auto power = weapon.getpower();
	auto chance_critical = 0;
	auto attack = player->getattack(slot, defender->islarge());
	auto damage_type = weapon.geti().combat.type;
	auto isrange = weapon.isranged();
	auto ammo = weapon.geti().combat.ammo;
	if(ammo) {
		if(!player->wears[Quiver].is(ammo))
			return; // No Ammo!
		auto arrow_magic = get_magic(player->wears[Quiver].power);
		attack.damage.b += item_data[ammo].combat.damage.b + arrow_magic;
		attack.attack += item_data[ammo].combat.attack + arrow_magic;
		if(player->wears[Quiver].is(Precise))
			chance_critical++;
		// Use ammo
		player->wears[Quiver].consume();
	}
	if(weapon.is(Precise))
		chance_critical++;
	// Magical weapon stats
	auto magic_bonus = get_magic(power);
	bonus += attack.attack;
	bonus += magic_bonus;
	// Other stats
	auto ac = defender->get(AC);
	if(!isrange) {
		if((player->is(ChaoticEvil) || player->is(Undead)) && defender->is(Protection))
			ac += 1;
		// RULE: Small dwarf use special tactics vs large opponents
		if(player->islarge() && (defender->is(Dwarf) || defender->is(Halfling)))
			ac += 4;
	}
	// RULE: Magically blurred wizard
	if((defender->is(Blurred) || defender->is(Invisible)) && !player->is(ImmuneIllusion))
		bonus -= 4;
	if((player->is(Invisible) && !defender->is(ImmuneIllusion)) || defender->is(Surprised))
		bonus += 4;
	// RULE: Dwarf hate to goblinoids
	if(player->is(Dwarf) && defender->is(Goblinoid))
		bonus += 1;
	// RULE: Ranger special hunter skill
	if(player->is(Ranger) && defender->is(Goblinoid))
		bonus += 4;
	// Special magical power, like Bane
	if((power == ControlGoblinoid && defender->race == Goblinoid)
		|| (player->is(weapon, Holy) && defender->is(Undead))) {
		bonus += 3;
		multiplier += 1;
	}
	if(player->is(Panic))
		bonus -= 2;
	auto tohit = 20 - bonus - (10 - ac);
	auto rolls = xrand(1, 20);
	auto hits = -1;
	tohit = imax(2, imin(20, tohit));
	auto is_critical_hit = false;
	auto critical_threshold = 20 - chance_critical;
	if(rolls >= tohit) {
		// If weapon hits
		if(rolls >= tohit && rolls >= critical_threshold) {
			// RULE: crtitical hit can apply only if attack hit and can be deflected
			if(!defender->roll(CriticalDeflect))
				is_critical_hit = true;
		}
		if(is_critical_hit) {
			multiplier += 1;
			if(weapon.is(Deadly))
				multiplier += 1;
		}
		attack.damage.m += multiplier;
		hits = attack.damage.roll();
		// Weapon of specific damage type
		switch(power) {
		case Flaming: hits += xrand(1, 6); break;
		case Freezing: hits += xrand(2, 5); break;
		default: break;
		}
		if(is_critical_hit && player->is(weapon, Vorpal))
			hits = 1000;
		if(player->is(weapon, DispelEvil) && defender->is(Demon)) {
			if(!defender->roll(SaveVsMagic))
				hits = 1000;
		}
		if(defender->is(Displaced) && d100() < 50)
			hits = -1; // Miss if displaced
		if(defender->is(Blinked) && defender->initiative < player->initiative)
			hits = -1; // Miss if blinked away
	}
	// Show result
	if(!player->is(ImmuneIllusion) && defender->get(DuplicateIllusion)) {
		hits = -2; // RULE: Mirror image effect
		defender->add(DuplicateIllusion, -1);
	} else
		defender->damage(damage_type, hits, magic_bonus);
	fix_attack(player, slot, hits);
	if(hits > 0) {
		// After all effects, if hit, do additional effects
		if(is_critical_hit) {
			// RULE: Weapon with spell cast it when critical hit occurs
			if(slot == RightHand) {
				//if(ps->is(Enemy))
				//	cast_spell(ps, player->level(), 0, true, false, 0, defender);
				//else
				//	cast_spell(ps, player->level(), 0, true, false, 0, player);
			}
		}
		// RULE: vampiric ability allow user to drain blood and regain own HP
		if(player->is(weapon, Vampiric)) {
			auto hits_healed = xrand(1, 3);
			if(hits_healed > hits)
				hits_healed = hits;
			player->heal(hits_healed);
		}
		// RULE: diseased weapon can cause disease if hit
		if(player->is(weapon, Disease)) {
			if(!defender->roll(SaveVsPoison))
				defender->add(DiseaseLevel, 1);
		}
		// RULE: poison attack
		if(player->is(weapon, Poison)) {
			if(!defender->roll(SaveVsPoison))
				defender->add(PoisonLevel, xrand(2, 8));
		}
		// RULE: paralized attack of ghouls and others
		if(player->is(weapon, Paralizing) && !defender->roll(SaveVsParalization))
			defender->add(Paralizing, xrand(3, 8));
		drain_attack(defender, weapon, DrainStrenght, DrainedStrenght, 0);
		drain_attack(defender, weapon, DrainEnergy, DrainedLevels, -100);
		// Poison attack
		//if(wi.is(OfPoison))
		// defender->add(Poison, Instant, SaveNegate);
		// 15% of all attack can damage equipment (if e hit and can harm)
		if(d100() < 15)
			hit_equipment(defender);
	}
	// Weapon can be broken
	if(rolls == 1)
		weapon.damage("WeaponBroken", 1);
}

static void single_main_attack(wearn wear, creature* enemy, int bonus, int multiplier) {
	auto number_attacks = player->wears[wear].geti().combat.number_attacks * 2;
	if(!number_attacks)
		number_attacks = 2;
	number_attacks += player->get(AdditionalAttacks);
	if(player->is(Fighter) || player->is(Paladin) || player->is(Ranger))
		number_attacks += 1;
	if(getv(Time) % 2)
		number_attacks += 1;
	number_attacks /= 2;
	while(number_attacks-- > 0)
		single_attack(enemy, wear, bonus, multiplier);
}

static void make_full_attack(creature* enemy, int bonus) {
	if(!enemy)
		return;
	fix_monster_attack(player);
	auto wp1 = player->wears[RightHand];
	auto wp2 = player->wears[LeftHand];
	auto wp3 = player->wears[Head];
	if(wp1.is(TwoHanded) || !wp2.isweapon())
		wp2.clear();
	if(!wp3.isweapon())
		wp3.clear();
	auto multiplier = 1;
	// RULE: backstabbing attack depend on surprise check and invisibility. Chance is move silently.
	if((enemy->is(Surprised) || player->is(Invisible) || player->initiative < enemy->initiative) && !enemy->is(Undead)) {
		auto theif_bakstab = player->get(Backstab);
		if(theif_bakstab > 0 && player->roll(MoveSilently)) {
			consolen(getnm(SneakAttack));
			multiplier += theif_bakstab;
			if(!bonus)
				bonus += 4;
		}
	}
	if(wp2) {
		single_main_attack(RightHand, enemy, bonus + player->gethitpenalty(-4), multiplier);
		single_attack(enemy, LeftHand, bonus + player->gethitpenalty(-6), multiplier);
	} else
		single_main_attack(RightHand, enemy, bonus, multiplier);
	if(wp3)
		single_attack(enemy, Head, bonus, multiplier);
	fix_monster_attack_end(player);
}

bool make_object_attack(pointc v) {
	if(!player || !player->isready())
		return false;
	auto slot = RightHand;
	auto object = loc->get(v);
	auto toughness = get_hit_points(object);
	if(!toughness)
		return false;
	auto bonus = 0;
	auto attack = player->getattack(slot, true);
	auto tohit = 20 - bonus - 10;
	auto rolls = xrand(1, 20);
	auto hits = -1;
	tohit = imax(2, imin(20, tohit));
	if(rolls >= tohit) {
		hits = attack.damage.roll();
		if(hits < toughness)
			hits = -1;
	}
	fix_attack(player, slot, hits);
	if(hits > 0)
		loc->broke(v);
	return true;
}

//static spelli* ai_choose_spell() {
//	auto max_spells = sizeof(player->spells) / sizeof(player->spells[0]);
//	for(size_t i = 0; i < max_spells; i++) {
//		if(!player->spells[i])
//			continue;
//		auto ps = bsdata<spelli>::elements + i;
//		if(!cast_spell(ps, player->getlevel(), 32, false, false, 0, 0))
//			continue;
//		an.add(ps, ps->getname());
//	}
//	if(!an)
//		return 0;
//	return (spelli*)an.random();
//}

static bool ai_use_spells() {
	//auto ps = ai_choose_spell();
	//if(!ps)
	return false;
	//cast_spell(ps, player->getlevel(), 35, true, true, 0, 0);
	//return true;
}

//static void move_closer(creature** creatures, directionn d) {
//}
//
//void move_closer(pointc v) {
//	creature* creatures[6] = {};
//	loc->getmonsters(creatures, v);
//	if(!creatures[0])
//		return;
//	move_closer(creatures, party.d);
//}

static void drop_loot(creature* player) {
	for(auto& it : player->wears) {
		if(!it || it.is(NaturalItem) || it.is(SummonedItem))
			continue;
		if(it.is(QuestItem) || (d100() < 15)) {
			it.identify(0);
			loc->drop(player->pos, it, get_side(player->side, party.d));
		}
	}
}

static void party_addexp_per_killed(int hd) {
	for(auto p : adventurers) {
		if(!p || p->isdisabled())
			continue;
		if(p->is(Fighter) || p->is(Paladin) || p->is(Ranger))
			p->addexp(hd * 10);
	}
}

void creature::kill() {
	if(!monster)
		return;
	party_addexp(expaward());
	party_addexp_per_killed(level());
	if(loc) {
		drop_loot(this);
		loc->state.monsters_killed++;
	}
	clear();
}

void creature::damage(damagen type, int value, bool magic_wepon) {
	if(value <= 0)
		return;
	auto resist = get_resist(type);
	auto immunity = get_immunity(type);
	if(immunity && is(immunity) && !magic_wepon)
		value = 0;
	else if(resist && is(resist))
		value = value / 2;
	if(value <= 0)
		return;
	switch(type) {
	case HealthDamage: case IllDamage: break; // Absolutely silent damage
	case PoisonDamage: consolen(getnm(FeelPoison), name()); break;
	default: fix_damage(this, value); break;
	}
	if(hp_aid > 0) {
		if(hp_aid >= value) {
			hp_aid -= value;
			value = 0;
		} else {
			value -= hp_aid;
			hp_aid = 0;
		}
	}
	hp -= value;
	if(hp <= 0)
		kill();
}

void make_attacks(bool melee_combat) {
	if(melee_combat) {
		auto v = to(party.pos, party.d);
		auto d = to(party.d, Down);
		turnto(v, d, true);
		// move_closer(v);
		enemy_distance = 1;
		if(!select_combatants(v))
			return;
	} else {
		if(!select_combatants(party.pos, party.d))
			return;
	}
	animation_update();
	auto push_player = player;
	auto d = to(party.d, Down);
	for(auto p : combatants) {
		player = p;
		player->set(Moved);
		if(!player->isready())
			continue;
		// RULE: Surprised creatures do not move first round in combat
		if(player->is(Surprised)) {
			player->remove(Surprised);
			continue;
		}
		// RULE: Paniced
		if(player->is(Panic)) {
			if(d100() < 30) {
				player->say(IAmScarry);
				continue;
			}
		}
		// If we can only shoot
		if(enemy_distance > 1 && !player->wears[RightHand].isranged())
			continue;
		if(player->monster) {
			if(!ai_use_spells()) {
				auto left_side = (get_side(player->side, d) % 2) == 0;
				if(player->islarge())
					left_side = (rand() % 2);
				make_full_attack(get_opponent(left_side, false), 0);
			}
		} else {
			auto left_side = (player->side % 2) == 0;
			make_full_attack(get_opponent(left_side, true), 0);
		}
		fix_animate();
	}
	player = push_player;
}