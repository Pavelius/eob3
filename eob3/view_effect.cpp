#include "draw.h"
#include "pushvalue.h"
#include "slice.h"
#include "view_effect.h"

void effecti::paint(unsigned current_tick) {
	if(!operator bool())
		return;
	if(start > current_tick)
		return;
	if(!proc(res, frame, count, current_tick - start))
		clear();
}

void effecti::set(fnproc proc, resn res, short unsigned frame, short unsigned count, unsigned start) {
	this->proc = proc;
	this->res = res;
	this->frame = frame;
	this->count = count;
	this->start = start;
}