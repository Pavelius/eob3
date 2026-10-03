#include "assign.h"
#include "dungeon.h"
#include "game.h"
#include "math.h"

posable party;

int dungeons_count;

dungeoni dungeons[250];

celli cells[LastCell + 1] = {
	{FONT6, -1, {Passable}}, // CellUnknown
	{FONT6, -1, {Passable}}, // CellPassable
	{FONT6, 0 * walls_frames, {LookWall}}, // CellWall
	{FONT6, 2 * walls_frames, {LookWall, PassableActivated}}, // CellDoor
	{FONT6, 3 * walls_frames, {LookWall, Passable, MonsterForbidden}}, // CellStairsUp
	{FONT6, 4 * walls_frames, {LookWall, Passable, MonsterForbidden}}, // CellStairsDown
	{FONT6, 5 * walls_frames, {LookWall}}, // CellPortal
	{FONT6, decor_offset + 1 * decor_frames, {LookObject, Passable, FloorLevel}}, // CellButton
	{FONT6, decor_offset + 3 * decor_frames, {LookObject, Passable, MonsterForbidden, FloorLevel}}, // CellPit
	{DECORS, 0 * decor_frames, {LookObject}, CellWebTorned}, // CellWeb
	{DECORS, 1 * decor_frames, {LookObject, Passable}}, // CellWebTorned
	{DECORS, 3 * decor_frames, {LookObject}, CellBarelDestroyed}, // CellBarel
	{DECORS, 4 * decor_frames, {LookObject, Passable}}, // CellBarelDestroyed
	{DECORS, 5 * decor_frames, {LookObject}}, // CellEyeColumn
	{DECORS, 12 * decor_frames, {LookObject, Passable, FloorLevel}}, // CellBloodStain
	{DECORS, 10 * decor_frames, {LookObject, Passable, MonsterForbidden, FloorLevel}}, // CellBloodBlades
	{DECORS, 13 * decor_frames, {LookObject, Passable, FloorLevel}}, // CellDirtyStains
	{DECORS, 11 * decor_frames, {LookObject, Passable}}, // CellJugDestroyed
	{DECORS, 6 * decor_frames, {LookObject}, CellCoconOpened}, // CellCocon
	{DECORS, 7 * decor_frames, {LookObject, Passable}}, // CellCoconOpened
	{DECORS, 8 * decor_frames, {LookObject, Passable}, CellGraveDesecrated}, // CellGrave
	{DECORS, 9 * decor_frames, {LookObject, Passable, FloorLevel}}, // CellGraveDesecrated
	{FONT6, decor_offset + 4 * decor_frames, {LookObject, Passable}}, // CellPitUp
	{FONT6, decor_offset + 9 * decor_frames, {LookOverlay}}, // CellPuller
	{FONT6, decor_offset + 7 * decor_frames, {LookOverlay}, CellPassable}, // CellSecretButton
	{FONT6, decor_offset + 11 * decor_frames, {LookOverlay}}, // CellCellar
	{FONT6, decor_offset + 12 * decor_frames, {LookOverlay}}, // CellMessage
	{FONT6, decor_offset + 13 * decor_frames, {LookOverlay}}, // CellKeyHole
	{FONT6, decor_offset + 15 * decor_frames, {LookOverlay}}, // CellTrapLauncher
	{FONT6, decor_offset + 16 * decor_frames, {LookOverlay}}, // CellDecor1
	{FONT6, decor_offset + 17 * decor_frames, {LookOverlay}}, // CellDecor2
	{FONT6, decor_offset + 18 * decor_frames, {LookOverlay}}, // CellDecor3
	{FONT6, -1, {LookOverlay}}, // CellDoorButton
	{OVERLAYS, 0, {LookWall, Passable, MonsterForbidden}}, // CellOverlay1
	{OVERLAYS, 1, {LookWall, Passable, MonsterForbidden}}, // CellOverlay2
	{OVERLAYS, 2, {LookWall, Passable, MonsterForbidden}}, // CellOverlay3
};

const unsigned short Blocked = 0xFFFF;
static directionn all_directionn[] = {Up, Right, Down, Left};
unsigned short pathmap[mpy][mpx];
static pointc path_stack[256];
static unsigned char path_push;
static unsigned char path_pop;

dungeoni *loc, *locup, *last_dungeon;

static void snode(pointc v, short unsigned cost) {
	if(!v)
		return;
	auto a = pathmap[v.y][v.x];
	if(a != 0xFFFF && (!a || cost < a)) {
		path_stack[path_push++] = v;
		pathmap[v.y][v.x] = cost;
	}
}

static celln get_wall(celln v) {
	switch(v) {
	case CellSecretButton:
	case CellPortal:
	case CellStairsUp:
	case CellStairsDown:
		return CellWall;
	default:
		return v;
	}
}

static bool is_no_monsters(celln v) {
	switch(v) {
	case CellWall: return true;
	default: return false;
	}
}

static bool is_passable(celln v) {
	switch(v) {
	case CellWall: return true;
	default: return false;
	}
}

static bool need_activate(celln v) {
	switch(v) {
	case CellButton: return true;
	default: return false;
	}
}

static celln get_broken(celln v) {
	switch(v) {
	case CellBarel: return CellBarelDestroyed;
	default: return CellUnknown;
	}
}

bool filter_corridor(pointc v) {
	if(get_wall(loc->get(v)) == CellWall)
		return false;
	return (get_wall(loc->get(to(v, Up))) == CellWall && get_wall(loc->get(to(v, Down))) == CellWall)
		|| (get_wall(loc->get(to(v, Left))) == CellWall && get_wall(loc->get(to(v, Right))) == CellWall);
}

directionn get_part_placement(pointc v) {
	static directionn directions[] = {
		LeftUp, Up, RightUp,
		Left, Center, Right,
		LeftDown, Down, RightDown
	};
	auto dx = imax(0, imin(2, v.x / (mpx / 3)));
	auto dy = imax(0, imin(2, (v.y / (mpy / 3)) * 3));
	return directions[dy * 3 + dx];
}

int get_side(int side, directionn d) {
	static const char place_sides[4][6] = {
		{1, 3, 0, 2},
		{0, 1, 2, 3},
		{2, 0, 3, 1},
		{3, 2, 1, 0},
	};
	if(d == Center)
		return side;
	return place_sides[d - Left][side];
}

int get_side_ex(int side, directionn d) {
	static const char place_sides[4][6] = {
		{1, 3, 5, 0, 2, 4},
		{0, 1, 2, 3, 4, 5},
		{2, 0, 5, 3, 1, 4},
		{5, 4, 3, 2, 1, 0},
	};
	if(d == Center)
		return side;
	return place_sides[d - Left][side];
}

pointc to(pointc v, directionn d) {
	switch(d) {
	case Up: return {v.x, (unsigned char)(v.y - 1)};
	case Down: return {v.x, (unsigned char)(v.y + 1)};
	case Left: return {(unsigned char)(v.x - 1), v.y};
	case Right: return {(unsigned char)(v.x + 1), v.y};
	default: return v;
	}
}

directionn to(directionn v, directionn d) {
	static const directionn rotate_direction[4][4] = {
		{Down, Left, Up, Right},
		{Left, Up, Right, Down},
		{Up, Right, Down, Left},
		{Right, Down, Left, Up},
	};
	return rotate_direction[v - Left][d - Left];
}

dungeoni* find_dungeon(questn quest, int level) {
	for(auto& e : dungeons) {
		if(e && e.quest==quest && e.level==level)
			return &e;
	}
	return 0;
}

void dungstatei::clear() {
	memset((void*)this, 0, sizeof(*this));
	up.clear();
	down.clear();
	lair.clear();
	portal.clear();
	special.clear();
}

void dungeoni::makewave(pointc start) {
	if(!start)
		return;
	path_push = path_pop = 0;
	path_stack[path_push++] = start;
	pathmap[start.y][start.x] = 1;
	while(path_push != path_pop) {
		auto v = path_stack[path_pop++];
		auto cost = pathmap[v.y][v.x] + 1;
		if(cost >= 0xFF00)
			break;
		snode(to(v, Left), cost);
		snode(to(v, Right), cost);
		snode(to(v, Up), cost);
		snode(to(v, Down), cost);
	}
}

void dungeoni::overlayi::clear() {
	posable::clear();
	link.clear();
	type = CellUnknown;
	subtype = 0;
}

void dungeoni::ground::clear() {
	posable::clear();
	item::clear();
}

void dungeoni::clear() {
	memset((void*)this, 0, sizeof(*this));
	state.clear();
	for(auto& e : overlays)
		e.clear();
	for(auto& e : items)
		e.clear();
	for(auto& e : monsters)
		e.clear();
}

void dungeoni::change(celln s, celln d) {
	pointc v;
	for(v.y = 0; v.y < mpy; v.y++) {
		for(v.x = 0; v.x < mpx; v.x++) {
			if(get(v) == s)
				set(v, d);
		}
	}
}

void dungeoni::block(bool treat_door_as_passable) const {
	pointc v;
	for(v.y = 0; v.y < mpy; v.y++) {
		for(v.x = 0; v.x < mpx; v.x++) {
			switch(get(v)) {
			case CellWall:
			case CellPortal:
			case CellUnknown:
				pathmap[v.y][v.x] = Blocked;
				break;
			case CellDoor:
				if(!treat_door_as_passable && !is(v, CellActive))
					pathmap[v.y][v.x] = Blocked;
				else
					pathmap[v.y][v.x] = 0;
				break;
			default:
				pathmap[v.y][v.x] = 0;
				break;
			}
		}
	}
}

celln dungeoni::get(pointc v) const {
	if(!v)
		return CellWall;
	return data[v.y][v.x];
}

dungeoni::overlayi* dungeoni::add(pointc v, directionn d, celln i) {
	if(!v)
		return 0;
	for(auto& e : overlays) {
		if(!e) {
			e.clear();
			e.pos = v;
			e.d = d;
			e.type = i;
			return &e;
		}
	}
	return 0;
}

void dungeoni::add(overlayi* po, item& it) {
	if(!it || !po)
		return;
	if(!have(po))
		return;
	for(auto& e : overlayitems) {
		if(!e) {
			e.clear();
			e.storage_index = po - overlays;
			assign<item>(e, it);
			it.clear();
			break;
		}
	}
}

void dungeoni::add(monstern type, pointc v, directionn d, int side) {
	player = 0;
	for(auto& e : monsters) {
		if(e)
			continue;
		player = &e;
		create_monster(type);
		e.pos = v;
		e.d = d;
		e.side = side;
		state.monsters++;
		break;
	}
}

void dungeoni::markoverlay(celln type, short unsigned value) const {
	for(auto& e : overlays) {
		if(!e || e.type != type)
			continue;
		auto v = to(e.pos, e.d);
		pathmap[v.y][v.x] = value;
	}
}

void dungeoni::set(pointc v, cellfn i, int radius) {
	pointc s;
	if(!radius)
		return;
	for(s.x = v.x - radius; s.x <= v.x + radius; s.x++)
		for(s.y = v.y - radius; s.y <= v.y + radius; s.y++)
			set(s, CellExplored);
}

dungeoni::overlayi* dungeoni::get(pointc v, directionn d) {
	if(!v)
		return 0;
	for(auto& e : overlays) {
		if(e.pos == v && e.d == d)
			return &e;
	}
	return 0;
}

dungeoni::overlayi* dungeoni::getlinked(pointc v) {
	if(!v)
		return 0;
	for(auto& e : overlays) {
		if(e && e.link == v)
			return &e;
	}
	return 0;
}

directionn dungeoni::getnear(pointc v, celln t) const {
	for(auto d : all_directionn) {
		auto t1 = get(to(v, d));
		if(t1 == t)
			return d;
	}
	return Center;
}

dungeoni::overlayi* dungeoni::getoverlay(pointc v, celln type) {
	for(auto& e : overlays) {
		if(e.pos == v && e.type == type)
			return &e;
	}
	return 0;
}

void dungeoni::set(pointc v, celln i) {
	if(!v)
		return;
	switch(i) {
	case CellPortal: state.portal = v; break;
	case CellStairsUp: state.up = v; break;
	case CellStairsDown: state.down = v; break;
	default: break;
	}
	data[v.y][v.x] = i;
}

void dungeoni::set(pointc v, celln i, pointc size) {
	auto ve = v + size; pointc pt;
	for(pt.y = v.y; pt.y < ve.y; pt.y++)
		for(pt.x = v.x; pt.x < ve.x; pt.x++)
			set(pt, i);
}

bool dungeoni::is(pointc v, cellfn i) const {
	if(!v)
		return false;
	return (flags[v.y][v.x] & (1 << i)) != 0;
}

void dungeoni::set(pointc v, cellfn i) {
	if(!v)
		return;
	flags[v.y][v.x] |= (1 << i);
}

void dungeoni::remove(pointc v, cellfn i) {
	if(!v)
		return;
	flags[v.y][v.x] &= ~(1 << i);
}

bool dungeoni::is(fnpointc proc) const {
	pointc v;
	for(v.y = 0; v.y < mpy; v.y++) {
		for(v.x = 0; v.x < mpx; v.x++) {
			if(proc(v))
				return true;
		}
	}
	return false;
}

bool dungeoni::is(pointc v, celln t1, celln t2) const {
	if(!v)
		return true;
	auto t = get(v);
	return t == t1 || t == t2;
}

void dungeoni::removeov(pointc v) {
	if(!v)
		return;
	for(auto& e : overlays) {
		if(e.d == Center)
			continue;
		if(to(e.pos, e.d) == v)
			e.clear();
	}
}

void dungeoni::set(pointc v, celln type, directionn d) {
	set(to(v, to(d, Left)), CellWall);
	set(to(v, to(d, Right)), CellWall);
	set(to(v, to(d, Down)), CellWall);
	set(v, type);
	switch(type) {
	case CellStairsUp: state.up.set(v, d); break;
	case CellStairsDown: state.down.set(v, d); break;
	case CellPortal: state.portal.set(v, d); break;
	default: break;
	}
}

bool dungeoni::isitem(pointc v) const {
	for(auto& e : items) {
		if(e && e.pos == v)
			return true;
	}
	return false;
}

bool dungeoni::ismonster(pointc v) const {
	for(auto& e : monsters) {
		if(e && e.pos == v)
			return true;
	}
	return false;
}

bool dungeoni::isoverlay(pointc v) const {
	for(auto& e : overlays) {
		if(e && e.pos == v)
			return true;
	}
	return false;
}

bool dungeoni::ismonster(pointc v, featn f) const {
	for(auto& e : monsters) {
		if(e && e.pos == v && e.is(f))
			return true;
	}
	return false;
}

bool dungeoni::ispassable(pointc v) const {
	if(!v)
		return false;
	auto n = get(v);
	if(need_activate(n))
		return is(v, CellActive);
	return is_passable(n);
}

bool dungeoni::isforbidden(pointc v) const {
	if(!v)
		return false;
	auto n = get(v);
	return is_no_monsters(n);
}

int dungeoni::around(pointc v, celln t1, celln t2) const {
	auto result = 0;
	for(auto d : all_directionn) {
		auto t = get(to(v, d));
		if(t == t1 || t == t2)
			result++;
	}
	return result;
}

directionn dungeoni::getpassable(pointc v) const {
	for(auto d : all_directionn) {
		auto v1 = to(v, d);
		if(!v1)
			continue;
		if(ispassable(v1))
			return d;
	}
	return Center;
}

void dungeoni::drop(pointc v, item& it, int side) {
	if(!it)
		return;
	for(auto& e : items) {
		if(e)
			continue;
		assign<item>(e, it);
		e.pos = v;
		e.side = side;
		e.d = Center;
		it.clear();
		auto index = &e - items + 1;
		if(state.items < index)
			state.items = index;
		break;
	}
}

size_t dungeoni::getitems(ground** result, size_t result_maximum, pointc v) {
	auto ps = result;
	auto pe = ps + result_maximum;
	for(auto& e : items) {
		if(!e || e.pos != v)
			continue;
		if(ps < pe)
			*ps++ = &e;
		else
			break;
	}
	return ps - result;
}

size_t dungeoni::getitems(item** result, size_t result_maximum, const overlayi* po) {
	if(!have(po))
		return 0;
	auto index = po - overlays;
	auto ps = result;
	auto pe = ps + result_maximum;
	for(auto& e : overlayitems) {
		if(!e || e.storage_index != index)
			continue;
		if(ps < pe)
			*ps++ = &e;
		else
			break;
	}
	return ps - result;
}

void dungeoni::getmonsters(creature** result, pointc index, directionn dr) {
	memset(result, 0, sizeof(creature*) * 6);
	if(!index)
		return;
	for(auto& e : monsters) {
		if(!e)
			continue;
		if(e.pos != index)
			continue;
		if(e.islarge())
			result[2] = &e;
		else
			result[get_side(e.side, dr)] = &e;
	}
}

void dungeoni::getmonsters(creature** result, pointc index) {
	getmonsters(result, index, Center);
}

creature* dungeoni::getmonster(monstern monster) {
	for(auto& e : monsters) {
		if(e && e.monster == monster)
			return &e;
	}
	return 0;
}

int	dungeoni::getpassables(bool explored) const {
	auto result = 0;
	pointc v;
	for(v.y = 0; v.y < mpy; v.y++) {
		for(v.x = 0; v.x < mpx; v.x++) {
			switch(get(v)) {
			case CellPassable:
			case CellButton:
			case CellDoor:
			case CellPit:
				if(explored && !is(v, CellExplored))
					break;
				result++;
				break;
			default:
				break;
			}
		}
	}
	return result;
}

void dungeoni::getoverlays(pointca& result, celln type, bool hidden) const {
	for(auto& e : overlays) {
		if(!e || e.type != type)
			continue;
		auto v = to(e.pos, e.d);
		if(hidden && is(v, CellExplored))
			continue;
		result.addu(v);
	}
}

void dungeoni::broke(pointc v) {
	auto t = get(v);
	auto broken_cell = get_broken(t);
	if(!broken_cell)
		broken_cell = CellPassable;
	set(v, broken_cell);
	animation_update();
	// TODO: When broke cell something happening
}