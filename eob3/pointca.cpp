#include "dungeon.h"
#include "pointca.h"
#include "rand.h"

pointca points;

void pointca::select(int r1, int r2) {
	clear();
	pointc v;
	for(v.x = 0; v.x < mpx; v.x++) {
		for(v.y = 0; v.y < mpy; v.y++) {
			if(pathmap[v.y][v.x] >= r1 && pathmap[v.y][v.x] <= r2)
				add(v);
		}
	}
}

void pointca::match(fnispointc proc, bool keep) {
	auto ps = data;
	for(auto& v : *this) {
		if(proc(v) == keep)
			*ps++ = v;
	}
	count = ps - data;
}

void pointca::match(cellfn value, bool keep) {
	auto ps = data;
	for(auto& v : *this) {
		if(loc->is(v, value) == keep)
			*ps++ = v;
	}
	count = ps - data;
}

pointc pointca::random() const {
	if(!count)
		return pointc();
	return data[rand() % count];
}

void pointca::shuffle() {
	zshuffle(data, count);
}