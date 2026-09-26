#pragma once

#include "pointc.h"
#include "adat.h"

extern unsigned short pathmap[mpy][mpx];

struct pointca : adat<pointc, mpy*mpx> {
	void select(int r1, int r2);
};
extern pointca points;