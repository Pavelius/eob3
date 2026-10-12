#include "archive.h"
#include "game.h"
#include "quest.h"
#include "stringbuilder.h"

void archive::setp(void** p, void* source, unsigned size) {
	unsigned char index;
	if(writemode) {
		index = (*p) ? ((unsigned char*)(*p) - (unsigned char*)source) / size : 0xFF;
		set(index);
	} else {
		set(index);
		*p = (index == 0xFF) ? 0 : (unsigned char*)source + index * size;
	}
}

template<> void archive::set<questi>(questi& v) {
	set(v.rumor);
	set(v.state);
}

template<> void archive::set<dungeoni*>(dungeoni*& v) {
	setp((void**)&v, dungeons, sizeof(dungeons[0]));
}

template<> void archive::set<creature*>(creature*& v) {
	setp((void**)&v, characters, sizeof(characters[0]));
}

static void set_adventures(archive& a) {
	for(auto i = 0; i < lengthof(adventurers); i++)
		a.set(adventurers[i]);
}

static void serial(archive& a) {
	if(!a.signature("SAV"))
		return;
	if(!a.version(0, 2))
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

bool game_serial(const char* id, bool write_mode) {
	char temp[512]; stringbuilder sb(temp);
#ifdef _WIN32
	sb.add(io::file::getdocpath());
#endif
	sb.add("\\Eye of Beholder");
	io::file::makedir(temp);
	sb.add("\\%1.sav", id);
	io::file file(temp, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write_mode);
	serial(a);
	return true;
}