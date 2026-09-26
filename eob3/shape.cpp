#include "game.h"
#include "slice.h"
#include "shape.h"
#include "stringbuilder.h"
#include "math.h"

static const char* shape_symbols = {"UX.1234567890 "};

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

//#ShapeLargeRoom
//
//XXXX1XXXX
//X.......X
//X.X.2.X.X
//X..202..X
//X.X...X.X
//X...3...X
//XXXXXXXXX
//
//#ShapeCity
//
//XXXX1XXXX
//X2222222X
//X2X222X2X
//X222X222X
//XX..2..XX
//X4.X0X.3X
//XXXXXXXXX
//
//#ShapeRoom
//
//XX1XX
//X.2.X
//X202X
//X...X
//XXXXX
//
//#SmallCircleRoom
//
//XX1XX
//XXX...XXX
//X...0...X
//XXX...XXX
//XXXXX
//
//#ShapePrison
//
//X1X
//X0X
//XXX
//
//#ShapeDeadEnd
//
//X1X
//X.0X
//XXX
//
//#ShapePassage
//
//X1X
//X.X
//X.X
//X.X
//X0X
//XXX
//
//#ShapeExit
//
//1
//X.X
//X0X
//XXX
//
//#ShapeTorture
//
//XX1XX
//X...X
//XX1XX
//X.0.X
//XXXXX