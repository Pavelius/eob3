#include "action.h"
#include "answers.h"
#include "collectiona.h"
#include "console.h"
#include "creature.h"
#include "dungeon.h"
#include "game.h"
#include "math.h"
#include "pushvalue.h"
#include "spell.h"

spella			spellbooks[32];
boost			boosts[256];
unsigned char	boost_count;

static collection<creature> creatures;
static collection<item> items;
static spelln default_spells_list[] = {CureLightWound, DetectEvil, DetectMagic, PurifyFood};
static spelln camp_autocast_spells[] = {CureLightWound, PurifyFood};

static unsigned char get_dungeon(const void* target) {
	if(target >= dungeons && target <= dungeons + lengthof(dungeons)) {
		return (unsigned char)(((char*)target - (char*)dungeons) / sizeof(dungeons[0]));
	} else
		return 0xFF;
}

targetref::targetref(const creature* p) {
	if(is_character(p)) {
		index = 0xFE;
		type = p - characters;
	} else {
		index = get_dungeon(p);
		type = p - dungeons[index].monsters;
	}
}

targetref::operator creature*() const {
	if(type == 0xFF)
		return 0;
	else if(type == 0xFE)
		return characters + index;
	return dungeons[type].monsters + index;
}

static boost* add_boost(spelln spell, creature* player, unsigned duration) {
	auto p = boosts + (boost_count++);
	memset(p, 0, sizeof(*p));
	p->spell = spell;
	p->target = player;
	p->stop = getv(Time) + duration;
	return p;
}

void check_boost(unsigned stamp) {
	auto ps = boosts;
	auto pe = boosts + boost_count;
	for(auto pb = boosts; pb < pe; pb++) {
		if(pb->stop > stamp)
			*ps++ = *pb;
		else {
			// TODO: remove boost possible callback implementation
		}
	}
	boost_count = ps - boosts;
}

void creature::damage(damagen type, int value, abilityn save, bool save_ignore) {
	if(roll(save)) {
		if(save_ignore)
			return;
		value = value / 2;
	}
	damage(type, value, true);
}

static bool add_items(creature* target, spelln spell, int level) {
	for(auto& e : target->wears) {
		if(!e)
			continue;
		if(!e.allow(spell, level))
			continue;
		items.add(&e);
	}
	return items.operator bool();
}

static bool add_party_items(spelln spell, int level) {
	for(auto p : adventurers) {
		if(!p)
			continue;
		add_items(p, spell, level);
	}
	return items.operator bool();
}

static bool add_creature(creature* player, spelln spell, int level) {
	if(!player)
		return false;
	if(!player->allow(spell, level))
		return false;
	creatures.add(player);
	return true;
}

static bool add_creatures(creature** source, spelln spell, int level) {
	for(auto i = 0; i < lengthof(adventurers); i++)
		add_creature(source[i], spell, level);
	return creatures.operator bool();
}

static bool add_party(spelln spell, int level) {
	return add_creatures(adventurers, spell, level);
}

static bool add_monsters(spelln spell, int level, int distance) {
	if(!loc)
		return false;
	auto position = to(party.pos, party.d);
	while(distance > 0)
		position = to(position, party.d);
	creature* monsters[6]; loc->getmonsters(monsters, position, party.d);
	return add_creatures(monsters, spell, level);
}

static bool spell_targets(creature* caster, spelln spell, int level) {
	creatures.clear();
	items.clear();
	switch(spell_data[spell].type) {
	case You: return add_creature(caster, spell, level);
	case Ally: case AllAlly: return add_party(spell, level);
	case AllAllyItems: return add_party_items(spell, level);
	case AllEnemy: case Enemy: return add_party_items(spell, level);
	default: return false;
	}
}

static creature* choose_player(bool interactive) {
	if(!interactive)
		return creatures.random();
	for(auto p : creatures)
		an.add((long)p, p->name());
	return (creature*)choose_small_menu(getnm(CastOnWho), 0);
}

static void apply_targets(spelln spell, int level) {
	for(auto p : creatures)
		p->apply(spell, level, true);
	for(auto p : items)
		p->apply(spell, level, true);
}

int creature::level(spelln spell) const {
	if(player->monster)
		return player->levels[0];
	auto priest_level = player->get(Cleric);
	auto mage_level = player->get(Mage);
	auto result_level = 0;
	if(spell_data[spell].levels[0]) {
		if(priest_level)
			result_level = priest_level;
	}
	if(spell_data[spell].levels[1]) {
		if(mage_level && mage_level < result_level)
			result_level = mage_level;
	}
	return result_level;
}

static void add_spell_experience(spelln spell) {
	auto caster_type = get_caster_class(caster->type);
	if(!caster_type || caster_type==Paladin || caster_type==Ranger)
		return; // Palading or Ranger not gain experience for spell casting.
	auto caster_index = get_caster(caster_type);
	switch(caster_type) {
	case Mage: caster->addexp(35 * spell_data[spell].levels[caster_index]); break;
	case Cleric: caster->addexp(20 * spell_data[spell].levels[caster_index]);  break;
	default: break;
	}
}

static bool spell_cast(spelln spell, bool run, int level, bool random_choose, bool expand_slots) {
	if(!spell_targets(caster, spell, level))
		return false;
	switch(spell_data[spell].type) {
	case Ally:
		creatures[0] = choose_player(!random_choose);
		creatures.count = 1;
		break;
	default:
		break;
	}
	apply_targets(spell, level);
	add_spell_experience(spell);
	if(expand_slots) {
		if(caster->spells[spell] > 0)
			caster->spells[spell]--;
	}
	return true;
}

bool creature::cast(spelln spell, bool run) {
	pushvalue push(caster, this);
	if(!spell_cast(spell, run, level(spell), false, true)) {
		say(CantFindTarget);
		return false;
	}
	consolen(getnm(PlayerCastSpell), name(), spell_names[spell]);
	return true;
}

bool creature::allow(spelln spell, int level) {
	switch(spell_data[spell].type) {
	case SummonWeapon:
		return true;
	case AllAlly: case Ally: case You:
	case Enemy: case AllEnemy:
		return apply(spell, level, false);
	default:
		return false;
	}
}

void creature::apply(spelln spell, fnevent value, unsigned duration) {
	auto p = add_boost(spell, this, duration);
	p->proc = value;
}

void creature::apply(spelln spell, featn value, unsigned duration) {
	auto p = add_boost(spell, this, duration);
	p->feat = value;
}

void creature::apply(spelln spell, itemn value, unsigned duration) {
	auto p = add_boost(spell, this, duration);
	p->summon = value;
}

bool item::allow(spelln spell, int level) {
	switch(spell_data[spell].type) {
	case AllAllyItems:
		return true;
	default:
		return false;
	}
}

int spella::total(int type, int level) const {
	auto result = 0;
	for(auto i = (spelln)0; i <= LastSpell; i = (spelln)(i+1)) {
		if(spell_data[i].levels[type] != level)
			continue;
		if(is(i))
			result++;
	}
	return result;
}

bool can_cast_spell(int type, int level) {
	static char maximum_spell_level[] = {
		0, 1, 1, 1, 1, 2, 2, 2, 3, 4,
		5, 5, 6, 6, 7, 7, 8, 8, 9
	};
	int ability;
	switch(type) {
	case 1:
		ability = player->get(Intellegence);
		return maptbl(maximum_spell_level, ability) >= level;
	default:
		return true;
	}
}

bool can_learn_spell(int type, int level) {
	static char maximum_number_of_spells[] = {
		1, 1, 1, 1, 2, 3, 4, 5, 6, 6,
		7, 7, 8, 8, 9, 9, 10, 11, 12
	};
	auto ps = get_spellbook(player);
	if(!ps)
		return false;
	if(!can_cast_spell(type, level))
		return false;
	if(type == 1) {
		auto intellegence = player->basic.abilities[Intellegence];
		auto exist_count = ps->total(type, level);
		auto maximum_count = maptbl(maximum_number_of_spells, intellegence);
		if(exist_count >= maximum_count)
			return false;
	}
	return true;
}

spella* get_spellbook(const creature* target) {
	if(!target)
		return 0;
	if(target>=characters && target<=characters + lengthof(characters))
		return spellbooks + (target - characters);
	return 0;
}

void learn_spells(creature* player, int level, int spell_type) {
	auto pb = get_spellbook(player);
	if(!pb)
		return;
	for(auto i = (spelln)0; i <= LastSpell; i = (spelln)(i+1)) {
		auto& e = spell_data[i];
		if(e.levels[spell_type] != level)
			continue;
		pb->set(i);
	}
}

void prepare_default_spells() {
	auto spell_known = get_spellbook(player);
	if(!spell_known)
		return;
	auto class_count = get_class_count(player->type);
	for(auto i = 0; i < class_count; i++) {
		auto tp = get_class(player->type, i);
		auto pc = get_caster(tp);
		if(pc == -1)
			continue;
		for(auto level = 1; level < 9; level++) {
			auto slot_left = player->get((abilityn)(level + Spell1 - 1));
			for(auto v : default_spells_list) {
				if(!spell_known->is(v))
					continue;
				if(spell_data[v].levels[pc] != level)
					continue;
				spell_known->spells[v] += slot_left;
				break;
			}
		}
	}
	memcpy(player->spells, spell_known->spells, sizeof(player->spells));
}

void camp_autocast() {
	for(auto v : camp_autocast_spells) {
		auto count = player->spells[v];
		if(!count)
			continue;
		auto caster_class = get_caster_class(player->type);
		if(!caster_class)
			continue;
		while(count-- && spell_cast(v, true, player->level(v), true, true))
			consolen(getnm(PlayerCastSpell), player->name(), spell_names[v]);
	}
}