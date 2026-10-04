#include "action.h"
#include "creature.h"
#include "dungeon.h"
#include "game.h"
#include "spell.h"

spellboost spellboosts[256];
unsigned char spellboost_count;

static spelln current_spell;

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

void apply(fnevent proc, unsigned duration, featn feat) {
	auto p = spellboosts + (spellboost_count++);
	memset(p, 0, sizeof(*p));
	p->spell = current_spell;
	p->target = player;
	p->proc = proc;
	p->feat = feat;
	p->stop = getv(Time) + duration;
}

void apply(damagen type, int value) {
	player->damage(type, value, 5);
}

void apply(damagen type, int value, abilityn save, bool save_ignore) {
	if(player->roll(save)) {
		if(save_ignore)
			return;
		value = value / 2;
	}
	apply(type, value);
}

void summon(itemn type, unsigned duration) {

}