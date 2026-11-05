#pragma once

#include "point.h"

enum resn : unsigned char;
enum effectn : unsigned char;

struct effecti {
	typedef void(*fnproc)(resn res, short unsigned frame, short unsigned count, unsigned tick);
	fnproc		proc;
	resn		res;
	short unsigned frame, count;
};
extern effecti effects[];

void add_effect(void* data, point pos, effectn res, unsigned start, unsigned duration);
bool effect_active();
void paint_effects(const void* data, unsigned current_tick);
void update_effects(unsigned current_tick);