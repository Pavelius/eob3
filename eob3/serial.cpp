#include "archive.h"
#include "game.h"
#include "quest.h"
#include "stringbuilder.h"

template<> void archive::set<questi>(questi& v) {
	set(v.rumor);
	set(v.state);
}

static void serial(archive& a) {
	if(!a.signature("SAV"))
		return;
	if(!a.version(0, 1))
		return;
	if(!a.writemode)
		game_clear();
	a.set(game);
	a.set(party);
	a.set(shops, sizeof(shops));
	a.set(characters, sizeof(characters));
	a.set(spellbooks, sizeof(spellbooks));
	a.set(quests);
	a.setv(dungeons, dungeons_count);
}

bool game_serial(const char* id, bool write_mode) {
	char temp[512]; stringbuilder sb(temp);
	sb.add(io::file::getdocpath());
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