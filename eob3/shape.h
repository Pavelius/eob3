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

	Symbols:
	Space	Unknown transparent cell.
	X		Blocking pass walls.
	.		Floors.
	0..9	Special point of interests. 0 - point is center.

*/

#pragma once

#include "pointc.h"

enum directionn : unsigned char;

enum shapen : unsigned char {
	ShapeLargeRoom, ShapeCity, ShapeRoom, SmallCircleRoom, ShapePrison,
	ShapeDeadEnd, ShapePassage, ShapeExit, ShapeTorture
};

typedef void (*fnroom)(pointc v, directionn d, shapen shape);

struct shapei {
	const char*		content;
	pointc			size;
	pointc			origin;
	pointc			points[10];
	char operator[](pointc m) const { return content[m.y * size.x + m.x]; }
	pointc			center(pointc c) const { return c + origin; }
	void			clear();
	pointc			find(char sym) const;
	size_t			maximum() const { return size.x * size.y; }
	pointc			translate(pointc s, pointc m, directionn d) const;
};
extern shapei shapes[ShapeTorture + 1];

void initialize_shapes();