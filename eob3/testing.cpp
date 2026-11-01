#include "draw.h"
#include "creature.h"
#include "game.h"
#include "stringbuilder.h"
#include "stringset.h"

static const char* test_items() {
	if(item_data[Robe].wear != Body)
		return "Robe is not Body wear.";
	if(item_data[Helm].avatar.pack != 20)
		return "Helm item wrong avatar. Maybe incorrect item index.";
	if(item_data[Hag2d4].combat.damage.c != 2)
		return "Hag 2d4 incorrect damage dice.";
	return 0;
}

static const char* test_locale() {
	for(auto p = stringset_data; *p; p++) {
		for(auto i = 0; i < p->count; i++) {
			if(!p->names[i]) {
				if(!i)
					return str("Locale names %1 is empty", p->id);
				else
					return str("Locale names %1, after element `%2` is empty", p->id, p->names[i - 1]);
			}
		}
	}
	return 0;
}

bool auto_test() {
	typedef const char* (*fntest)();
	static fntest source[] = {test_locale, test_items};
	auto result = true;
	for(auto proc : source) {
		auto error = proc();
		if(error) {
			message_box(error);
			result = false;
		}
	}
	return result;
}