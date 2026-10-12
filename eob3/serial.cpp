#include "archive.h"
#include "game.h"
#include "quest.h"
#include "stringbuilder.h"

template<> void archive::set<questi>(questi& v) {
	set(v.rumor);
	set(v.state);
}

template<> void archive::set<dungeoni*>(dungeoni*& v) {
	set((void**)&v, dungeons, sizeof(dungeons[0]));
}

template<> void archive::set<creature*>(creature*& v) {
	set((void**)&v, characters, sizeof(characters[0]));
}

static void set_adventures(archive& a) {
	for(auto i = 0; i < lengthof(adventurers); i++)
		a.set(adventurers[i]);
}

static unsigned get_checksum() {
	unsigned index = 0;
	unsigned result = 0;
	result = (++index) * sizeof(game);
	result = (++index) * sizeof(party);
	result = (++index) * lengthof(adventurers);
	result = (++index) * sizeof(shops[0]);
	result = (++index) * lengthof(shops);
	result = (++index) * sizeof(characters[0]);
	result = (++index) * lengthof(characters);
	result = (++index) * lengthof(spell_data);
	result = (++index) * lengthof(item_data);
	result = (++index) * lengthof(quests);
	result = (++index) * sizeof(dungeons[0]);
	result = (++index) * sizeof(boosts[0]);
	return result;
}

static void serial(archive& a) {
	if(!a.signature("SAV"))
		return;
	if(!a.version(0, 2))
		return;
	if(!a.checksum(get_checksum()))
		return;
	if(!a.writemode)
		game_clear();
	a.set(game);
	a.set(party);
	a.set(loc);
	a.set(location);
	a.set(player);
	set_adventures(a);
	a.set(shops, sizeof(shops));
	a.set(characters, sizeof(characters));
	a.set(spellbooks, sizeof(spellbooks));
	a.set(quests);
	a.setv(dungeons, dungeons_count);
	a.setv(boosts, boost_count);
	if(!a.writemode) {
		if(loc)
			next_scene(play_dungeon);
		else
			next_scene(play_city_actions);
	}
}

void game_file(char* result, const char* folder, const char* name, const char* ext, int index) {
	stringbuilder sb(result, result + 511);
#ifdef _WIN32
	sb.add(io::file::getdocpath());
#endif
	sb.add("\\Eye of Beholder");
	io::file::makedir(result);
	if(folder) {
		sb.addv("\\", 0);
		sb.addv(folder, 0);
		io::file::makedir(result);
	}
	if(name) {
		sb.addv("\\", 0);
		sb.add(name, index);
		if(ext) {
			sb.addv(".", 0);
			sb.addv(ext, 0);
		}
	}
}

bool game_serial(const char* id, bool write_mode) {
	char temp[512]; game_file(temp, "Saved Games", id, "sav");
	io::file file(temp, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write_mode);
	serial(a);
	return true;
}