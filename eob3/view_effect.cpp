#include "draw.h"
#include "pushvalue.h"
#include "slice.h"
#include "view_effect.h"

namespace {
struct paintinfo {
	effectn		effect;
	point		caret;
	void*		data;
	unsigned	start, stop;
	constexpr explicit operator bool() const { return stop != 0; }
	void		clear() { memset((void*)this, 0, sizeof(*this)); }
};
}
static paintinfo objects[32];

static paintinfo* new_effect() {
	for(auto& e : objects) {
		if(!e)
			return &e;
	}
	return objects;
}

void add_effect(void* data, point position, effectn effect, unsigned start, unsigned duration) {
	auto p = new_effect();
	p->data = data;
	p->caret = position;
	p->effect = effect;
	p->start = start;
	p->stop = start + duration;
}

bool effect_active() {
	return objects[0].operator bool();
}

void paint_effects(const void* data, unsigned current_tick) {
	pushrect push;
	for(auto& e : objects) {
		if(!e)
			break;
		if(e.data != data)
			continue;
		if(e.start > current_tick || e.stop < current_tick)
			continue;
		auto& ei = effects[e.effect];
		caret = e.caret + push.caret;
		ei.proc(ei.res, ei.frame, ei.count, current_tick - e.start);
	}
}

void update_effects(unsigned current_tick) {
	if(!effect_active())
		return; // Optimize effect updating.
	auto ps = objects;
	// Store valid objects
	for(auto& e : objects) {
		if(e.stop > current_tick)
			*ps++ = e;
	}
	// Clear all tail
	while(ps < objects + lengthof(objects)) {
		ps->clear();
		ps++;
	}
}