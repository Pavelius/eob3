#include "draw.h"
#include "stringbuilder.h"

static point origin;
unsigned text_flags;

static int textfbc(const char* string, int width) {
	int symbols = 0;
	int w = 0;
	const char* s1 = string;
	while(true) {
		unsigned char s = *s1++;
		if(s == 0x20 || s == 9) {
			symbols = s1 - string - 1;
		} else if(s == 0 || s == '[' || s == ']') {
			symbols = s1 - string - 1;
			break;
		} else if(s == 10 || s == 13) {
			symbols = s1 - string - 1;
			break;
		}
		w += textw(s);
		if(w > width)
			break;
	}
	return symbols;
}

static void textln() {
	caret.x = origin.x;
	caret.y += texth();
}

static const char* textln(const char* p, unsigned flags, color new_fore) {
	pushfore push_fore(new_fore);
	if(caret.y >= clipping.y2)
		return p;
	while(*p) {
		if(*p == ']') {
			p++;
			break;
		} else if(*p == 10 || *p == 13) {
			p = skipcr(p);
			textln();
			if(caret.y >= clipping.y2)
				break;
		} else if(*p == '[') {
			p++;
			switch(*p) {
			case '-': p = textln(p + 1, flags, colors::red); break;
			case '+': p = textln(p + 1, flags, colors::green); break;
			case '~': p = textln(p + 1, flags, colors::gray); break;
			case '*': p = textln(p + 1, flags | TextBold, colors::green); break;
			case ' ': p = textln(p, flags, colors::special); break; // Special case when use '+' or '-'.
			default: p = textln(p, flags, colors::special); break;
			}
		} else {
			auto n = textfbc(p, width - (caret.x - origin.x));
			if(!n) {
				textln();
				p = skipsp(p);
				n = textfbc(p, width - (caret.x - origin.x));
				if(!n)
					return "";
			}
			text(p, n, flags);
			caret.x = text_next.x;
			p += n;
		}
	}
	return p;
}

void textf(const char* format) {
	if(!font)
		return;
	auto push = origin; origin = caret;
	textln(format, text_flags, fore);
	if(origin.x != caret.x)
		textln();
	origin = push;
}