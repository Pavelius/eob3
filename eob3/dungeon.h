#pragma once

#include "creature.h"

enum directionn : unsigned short;
enum monstern : unsigned char;
enum resid : unsigned short;

enum celln : unsigned char {
	CellUnknown,
	// Dungeon cells
	CellPassable, CellWall, CellDoor, CellStairsUp, CellStairsDown, CellPortal, // On space
	CellButton, CellPit,
	CellWeb, CellWebTorned,
	CellBarel, CellBarelDestroyed,
	CellEyeColumn,
	CellBloodStain, CellBloodBlades, CellDirtyStains, CellJugDestroyed,
	CellCocon, CellCoconOpened,
	CellGrave, CellGraveDesecrated,
	CellPitUp, // On floor
	// Decor
	CellPuller, CellSecretButton, CellCellar, CellMessage,
	CellKeyHole, CellTrapLauncher,
	CellDecor1, CellDecor2, CellDecor3,
	CellDoorButton,
	// Overaly
	CellOverlay1, CellOverlay2, CellOverlay3,
};
enum cellfn : unsigned char {
	CellExplored, CellActive, CellExperience,
	Passable, MonsterForbidden,
	LookWall, LookOverlay, LookObject, FloorLevel,
	PassableActivated
};

const int mpx = 38;
const int mpy = 23;

struct pointc {
	char x = -1, y = -1;
	constexpr bool operator==(pointc v) const { return x == v.x && y == v.y; }
	constexpr bool operator!=(pointc v) const { return x != v.x || y != v.y; }
	explicit operator bool() const { return x >= 0 && y >= 0 && x < mpx && y < mpy; }
	pointc operator+(const pointc& v) const { return {(char)(x + v.x), (char)(y + v.y)}; }
	pointc operator+(int i) const { pointc v; v.set(x + i, y + i); return v; }
	pointc operator-(int i) const { pointc v; v.set(x - i, y - i); return v; }
	void clear() { x = y = -1; }
	int	distance(pointc v) const;
	void set(int nx, int ny);
	pointc to(int dx, int dy) const { return {(char)(x + dx), (char)(y + dy)}; }
};

struct posable : pointc {
	char		side = 0;
	directionn	d = (directionn)0;
	constexpr posable() = default;
	constexpr posable(pointc v) : pointc(v), side(0), d() {}
	constexpr posable(pointc v, directionn d) : pointc(v), side(0), d(d) {}
	void clear() { pointc::clear(); side = 0; d = (directionn)0; }
	void set(pointc v, directionn d) { x = v.x; y = v.y; d = d; }
};
extern posable party;

struct sitei {
	resid type; // Resources of dungeon
	racen language; // All messages in this language (by race)
	unsigned char level; // Dungeon level: 0 - is outdoor surface, 1+ for underground.
	monstern habbits[2]; // Who dwelve here
	monstern boss, minions; // Boss with minions can be present on level lair
	itemn key; // Key open all doors
	itemn special; // Special item find somewhere
	unsigned char webs, barrels, eggs, graves, blood, dirt, blades, jug; // Count of special corridor features in dungeon
	unsigned char trap; // Type of all dungeon traps
	char cursed; // Chance to all items found be cursed
	char magical; // Chance to all items found be magical
	unsigned short textures[3]; // Special wall for interactions with special texture
	constexpr explicit operator bool() const { return type != (resid)0; }
};

struct dungeoni : sitei {
	struct ground : item, posable {
		explicit operator bool() const { return item::operator bool(); }
		void		clear();
	};
	struct overlayi : posable {
		celln		type; // type of overlay
		pointc		link; // linked to this location
		unsigned char subtype; // depends on value type
		unsigned char flags;
		void		clear();
		bool		is(cellfn v) const { return (flags & (1<<v)) != 0; }
		void		remove() { clear(); }
		void		set(cellfn v) { flags |= 1 << v; }
	};
	struct overlayitem : item {
		unsigned short storage_index;
	};
	unsigned short	quest_id;
	//dungeon_state	state;
	ground			items[512];
	creature		monsters[256];
	overlayi		overlays[256];
	overlayitem		overlayitems[256];
	celln			data[mpy][mpx];
	unsigned char	flags[mpy][mpx];
	//goalf			rewards;
	overlayi*		add(pointc v, directionn d, celln i);
	void			add(overlayi* po, item& it);
	void			add(monstern type, pointc v, directionn d, int side);
	int				around(pointc v, celln t1, celln t2) const;
	void			block(bool treat_door_as_passable) const;
	void			broke(pointc v);
	void			clear();
	void			change(celln s, celln n);
	void			drop(pointc v, item& it, int side);
	celln			get(pointc v) const;
	overlayi*		get(pointc v, directionn d);
	overlayi*		getlinked(pointc v);
	directionn		getnear(pointc v, celln c) const;
	size_t			getitems(ground** result, size_t result_maximum, pointc v);
	size_t			getitems(item** result, size_t result_maximum, const overlayi* po);
	itemi*			getkey() const;
	creature*		getmonster(short unsigned monster_id);
	void			getmonsters(creature** result, pointc index, directionn dr);
	void			getmonsters(creature** result, pointc index);
	overlayi*		getoverlay(pointc v, celln type);
	//void			getoverlays(pointca& result, celln type, bool hidden) const;
	directionn		getpassable(pointc v) const;
	int				getpassables(bool explored) const;
	bool			have(const overlayi* p) const { return p >= overlays && p <= overlays + sizeof(overlays) / sizeof(overlays[0]); }
	bool			have(const creature* p) const { return p >= monsters && p <= monsters + sizeof(monsters) / sizeof(monsters[0]); }
	bool			is(pointc v, cellfn i) const;
	bool			is(pointc v, celln t1, celln t2) const;
	//bool			is(goaln v) const { return rewards.is(v); }
	//bool			is(fnpointc v) const;
	bool			isitem(pointc v) const;
	bool			isforbidden(pointc v) const;
	bool			ismonster(pointc v) const;
	//bool			ismonster(pointc v, featn f) const;
	bool			isoverlay(pointc v) const;
	bool			isoverlay(pointc v, directionn d) const { return const_cast<dungeoni*>(this)->get(v, d) != 0; }
	bool			ispassable(pointc v) const;
	static void		makewave(pointc start);
	void			markoverlay(celln type, short unsigned value) const;
	void			remove(pointc v, cellfn i);
	void			removeov(pointc v);
	void			set(pointc v, celln i);
	void			set(pointc v, celln i, directionn d);
	void			set(pointc v, celln i, pointc size);
	void			set(pointc v, cellfn i, int radius);
	void			set(pointc v, cellfn i);
};
extern dungeoni* loc;
extern dungeoni* locup;
extern dungeoni* last_dungeon;