#include "draw.h"
#include "math.h"
#include "slice.h"
#include "view_focus.h"

namespace {
struct renderi {
	long	focus;
	point	pt;
	void clear() { pt.x = -1000; pt.y = -1000; focus = empty_focus; }
};
}

static renderi objects[48];
static renderi*	render_current;

long current_focus, pressed_focus;
bool disable_input;

void focusing(long focus) {
	if(!focus || disable_input)
		return;
	if(!render_current
		|| render_current >= objects + sizeof(objects) / sizeof(objects[0]) - 1)
		render_current = objects;
	render_current[0].pt.x = caret.x + width / 2;
	render_current[0].pt.y = caret.y + height / 2;
	render_current[0].focus = focus;
	render_current++;
	render_current->clear();
	if(!focus_valid(current_focus))
		current_focus = focus;
}

static renderi* getby(long av) {
	for(auto& e : objects) {
		if(!focus_valid(e.focus))
			return 0;
		if(e.focus == av)
			return &e;
	}
	return 0;
}

static renderi* getfirst() {
	for(auto& e : objects) {
		if(!focus_valid(e.focus))
			return 0;
		return &e;
	}
	return 0;
}

static renderi* getlast() {
	auto p = objects;
	for(auto& e : objects) {
		if(!focus_valid(e.focus))
			break;
		p = &e;
	}
	return p;
}

static bool is_vert(point p1, point p2) {
	return iabs(p1.x - p2.x) <= iabs(p1.y - p2.y);
}

static renderi* next_focus(long ev, int key) {
	if(!key)
		return 0;
	auto pc = getby(ev);
	if(!pc)
		pc = getfirst();
	if(!pc)
		return 0;
	auto pe = pc;
	auto pl = getlast();
	int inc = 1;
	renderi* r1 = 0;
	auto p1 = pe->pt;
	while(true) {
		pc += inc;
		if(pc > pl)
			pc = objects;
		else if(pc < objects)
			pc = pl;
		if(pe == pc) {
			if(r1)
				return r1;
			return pe;
		}
		auto p2 = pc->pt;
		auto dx = iabs(p1.x - p2.x);
		auto dy = iabs(p1.y - p2.y);
		switch(key) {
		case KeyLeft:
			if(p2.x >= p1.x)
				continue;
			if(is_vert(p1, p2))
				continue;
			if(r1) {
				if(dy > iabs(p1.y - r1->pt.y))
					continue;
				if(distance(pe->pt, pc->pt) > distance(pe->pt, r1->pt))
					continue;
			}
			break;
		case KeyRight:
			if(p2.x <= p1.x)
				continue;
			if(is_vert(p1, p2))
				continue;
			if(r1) {
				if(dy > iabs(p1.y - r1->pt.y))
					continue;
				if(distance(pe->pt, pc->pt) > distance(pe->pt, r1->pt))
					continue;
			}
			break;
		case KeyDown:
			if(p2.y <= p1.y)
				continue;
			if(r1) {
				if(dx > iabs(p1.x - r1->pt.x))
					continue;
				if(distance(pe->pt, pc->pt) > distance(pe->pt, r1->pt))
					continue;
			}
			break;
		case KeyUp:
			if(p2.y >= p1.y)
				continue;
			if(r1) {
				if(dx > iabs(p1.x - r1->pt.x))
					continue;
				if(distance(pe->pt, pc->pt) > distance(pe->pt, r1->pt))
					continue;
			}
			break;
		default:
			return pc;
		}
		r1 = pc;
	}
}

void apply_focus(int key) {
	auto p = next_focus(current_focus, key);
	if(!p)
		return;
	current_focus = p->focus;
}

long focus_next(long focus, int key) {
	auto p = next_focus(focus, key);
	if(!p)
		return 0;
	return p->focus;
}

bool focus_input() {
	switch(hkey) {
	case KeyLeft: apply_focus(KeyLeft); break;
	case KeyRight: apply_focus(KeyRight); break;
	case KeyUp: apply_focus(KeyUp); break;
	case KeyDown: apply_focus(KeyDown); break;
	default: return false;
	}
	return true;
}

void clear_focus_data() {
	render_current = 0;
	objects[0].clear();
}