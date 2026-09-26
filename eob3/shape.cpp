#include "game.h"
#include "slice.h"
#include "shape.h"
#include "stringbuilder.h"
#include "math.h"

// static const char* shape_symbols = {"UX.1234567890 "};

pointc shapei::find(char sym) const {
	pointc v;
	for(v.y = 0; v.y < size.y; v.y++) {
		for(v.x = 0; v.x < size.x; v.x++) {
			if((*this)[v] == sym)
				return v;
		}
	}
	return {-1, -1};
}

pointc shapei::translate(pointc c, pointc v, directionn d) const {
	switch(d) {
	case Up: return c.to(origin.x + v.x, origin.y + v.y);
	case Down: return c.to(origin.x + v.x, -origin.y - v.y);
	case Left: return c.to(v.y + origin.y, v.x);
	case Right: return c.to(size.y - v.y + origin.y, origin.x + v.x);
	default: return c;
	}
}

void shapei::clear() {
	memset(this, 0, sizeof(*this));
	for(auto& e : points)
		e.clear();
	origin.clear();
}

void initialize_shapes() {
	for(auto& e : shapes) {
		for(auto sym : "0123456789") {
			if(!sym)
				break;
			e.points[sym - '0'] = e.find(sym);
		}
		e.origin.x = -e.points[0].x;
		e.origin.y = -e.points[0].y;
	}
}

static const char* shape_large_room =
"XXXX1XXXX"
"X.......X"
"X.X.2.X.X"
"X..202..X"
"X.X...X.X"
"X...3...X"
"XXXXXXXXX";
static const char* shape_city =
"XXXX1XXXX"
"X2222222X"
"X2X222X2X"
"X222X222X"
"XX..2..XX"
"X4.X0X.3X"
"XXXXXXXXX";
static const char* shape_room =
"XX1XX"
"X.2.X"
"X202X"
"X...X"
"XXXXX";
static const char* shape_small_circle_room =
"  XX1XX  "
"XXX...XXX"
"X...0...X"
"XXX...XXX"
"  XXXXX  ";
static const char* shape_prison =
"X1X"
"X0X"
"XXX";
static const char* shape_dead_end =
"X1X "
"X.0X"
"XXX ";
static const char* shape_passage =
"X1X"
"X.X"
"X.X"
"X.X"
"X0X"
"XXX";
static const char* shape_exit =
" 1 "
"X.X"
"X0X"
"XXX";
static const char* shape_torture =
"XX1XX"
"X...X"
"XX1XX"
"X.0.X"
"XXXXX";

shapei shapes[ShapeTorture + 1] = {
	{shape_large_room, {7, 9}},
	{shape_city, {7, 9}},
	{shape_room, {5, 5}},
	{shape_small_circle_room, {9, 5}},
	{shape_prison, {3, 3}},
	{shape_dead_end, {4, 3}},
	{shape_passage, {3, 6}},
	{shape_exit, {3, 4}},
	{shape_torture, {5, 5}}
};