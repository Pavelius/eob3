#pragma once

#include "pointc.h"

enum directionn : unsigned char;

enum shapen : unsigned char {
	NoShape,
	ShapeLargeRoom, ShapeCity, ShapeRoom, SmallCircleRoom, ShapePrison,
	ShapeDeadEnd, ShapePassage, ShapeExit, ShapeTorture
};

typedef void (*fnroom)(pointc v, directionn d, shapen shape);

struct shapei {
	const char*		content;
	pointc			origin;
	pointc			size;
	pointc			points[10];
	char operator[](pointc m) const { return content[m.y * size.x + m.x]; }
	pointc			center(pointc c) const { return c + origin; }
	void			clear();
	pointc			find(char sym) const;
	size_t			maximum() const { return size.x * size.y; }
	pointc			translate(pointc s, pointc m, directionn d) const;
};
extern shapei shapes[ShapeTorture + 1];
