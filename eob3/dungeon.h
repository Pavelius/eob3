/*
	Copyright 2026 by Pavel Chistyakov

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.77
*/

#pragma once

#include "creature.h"
#include "pointca.h"

enum directionn : unsigned char;
enum monstern : unsigned char;
enum resn : unsigned char;
enum trapn : unsigned char;

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
	LookWall, LookOverlay, LookObject, FloorLevel,
};
enum goaln : unsigned char {
	ExploreMostArea,
	FindAllSecrets, TakeSpecialItem, OpenAllLockedDoors, DisableAllTraps,
	KillBoss, KillBossMinions, KillAlmostAllMonsters,
};
enum wellmsgn : unsigned char {
	MessageMagicWeapons, MessageMagicRings, MessageSecrets, MessageTraps, MessageLocked,
	MessageAtifacts, MessageCursedItems, MessageSpecialItem, MessageBoss,
	MessageHabbits
};

typedef char goala[KillAlmostAllMonsters + 1];

extern const char* goal_names[MessageHabbits + 1];
extern const char* wellmsg_names[MessageHabbits + 1];

struct sitei {
	resn type; // Resources of dungeon
	racen language; // All wellmsgn in this language (by race)
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
	constexpr explicit operator bool() const { return type != (resn)0; }
};

struct dungstatei {
	posable			up, down; // where is stairs located
	posable			portal; // where is portal
	posable			special; // where is special item dropped
	posable			features[8]; // where is dungeon features locatied (if any)
	short unsigned	wellmsgn; // count of wellmsgn
	short unsigned	secrets_found; // count of secret rooms found (used secret button)
	short unsigned	elements; // count of corridors
	short unsigned	bones; // count of bones
	short unsigned	gems; // count of gems
	short unsigned	relicts; // count of books and holy symbols originally placed
	short unsigned	items; // total count of items originally placed
	short unsigned	items_lying; // total count of items laying on ground
	short unsigned	overlays; // total count of overlays
	short unsigned	monsters; // total count of monsters
	short unsigned	monsters_alive; // total alive monsters
	short unsigned	monsters_killed; // total killed monsters
	short unsigned	traps_disabled; // total disabled traps
	short unsigned	locks_open; // total opened locks by theif tools or by key
	short unsigned	total_passable; // total cell passable (include buttons and pits)
	short unsigned	explored_passable; // total cell passable (include buttons and pits) explored
	short unsigned	wallmessages[MessageHabbits]; // count of variable wellmsgn
	short unsigned	goals; // Reaching goals by party
	void clear();
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
		bool		is(cellfn v) const { return (flags & (1 << v)) != 0; }
		void		remove() { clear(); }
		void		set(cellfn v) { flags |= 1 << v; }
	};
	struct overlayitem : item {
		unsigned short storage_index;
	};
	unsigned char	quest_id;
	dungstatei		state;
	ground			items[512];
	creature		monsters[256];
	overlayi		overlays[256];
	overlayitem		overlayitems[256];
	celln			data[mpy][mpx];
	unsigned char	flags[mpy][mpx];
	short unsigned	rewards;
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
	itemn			getkey() const { return key; }
	creature*		getmonster(monstern type);
	void			getmonsters(creature** result, pointc index, directionn dr);
	void			getmonsters(creature** result, pointc index);
	overlayi*		getoverlay(pointc v, celln type);
	void			getoverlays(pointca& result, celln type, bool hidden) const;
	directionn		getpassable(pointc v) const;
	int				getpassables(bool explored) const;
	bool			have(const overlayi* p) const { return p >= overlays && p <= overlays + sizeof(overlays) / sizeof(overlays[0]); }
	bool			have(const creature* p) const { return p >= monsters && p <= monsters + sizeof(monsters) / sizeof(monsters[0]); }
	bool			is(pointc v, cellfn i) const;
	bool			is(pointc v, celln t1, celln t2) const;
	bool			is(goaln v) const { return (rewards & (1 << v)) != 0; }
	bool			is(fnpointc v) const;
	bool			isitem(pointc v) const;
	bool			isforbidden(pointc v) const;
	bool			ismonster(pointc v) const;
	bool			ismonster(pointc v, featn f) const;
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
extern dungeoni* loc; // Currently used dungeon. If loc = 0 then we are in city.
extern dungeoni* locup; // Top level of currently used dungeon. Use for roof pit painting.
extern dungeoni dungeons[250];

extern int dungeons_count;

pointc to(pointc v, directionn d);

int get_side(int side, directionn d);
int get_side_ex(int side, directionn d);

directionn to(directionn v, directionn d);
directionn get_part_placement(pointc v);

bool filter_corridor(pointc v);