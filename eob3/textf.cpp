#include "draw.h"
#include "stringbuilder.h"

static point origin, last;
unsigned text_flags = 0;

static int textfbc(const char* string, int width) {
	int symbols = -1;
	int w = 0;
	const char* s1 = string;
	while(true) {
		unsigned char s = *s1++;
		if(s == 0x20 || s == 9 || s == '[' || s==']') {
			symbols = s1 - string - 1;
		} else if(s == 0) {
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
	if(symbols == -1)
		symbols = s1 - string;
	return symbols;
}

static void textln() {
	caret.x = origin.x;
	caret.y += texth();
}

static const char* textln(const char* p, unsigned flags, color new_fore) {
	pushfore push_fore(new_fore);
	while(*p) {
		if(*p==']') {
			p++;
			break;
		} else if(*p==10 || *p==13) {
			p = skipcr(p);
			textln();
		} else if(*p==0x20 || *p==9) {
			auto p1 = skipsp(p);
			caret.x += textw(' ') * (p - p1);
			p = p1;
		} else if(*p=='[') {
			p++;
			switch(*p) {
			case '-': p = textln(p+1, flags, colors::red); break;
			case '+': p = textln(p+1, flags, colors::green); break;
			case '*': p = textln(p+1, flags|TextBold, colors::green); break;
			default: p = textln(p+1, flags, colors::special); break;
			}
		} else {
			auto n = textfbc(p, width);
			text(p, n, flags);
			caret.x = text_next.x;
		}
	}
	return p;
}

void textf(const char* format, unsigned flags) {
	if(!font)
		return;
	auto push = origin;
	origin = caret;
	textln(format, flags, fore);
	if(origin.x != caret.x)
		textln();
	origin = push;
}