#pragma once

enum resid : unsigned short;
enum racen : unsigned char;
enum itemn : unsigned char;
enum monstern : unsigned char;

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
extern pointc last_point;

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