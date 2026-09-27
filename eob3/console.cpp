#include "console.h"
#include "timer.h"
#include "stringbuilder.h"
#include "slice.h"

char console_text[512];
static unsigned long time_stamp;

void console_delete_line() {
	auto p = zchr(console_text, '\n');
	if(p) {
		p++;
		memmove(console_text, p, zlen(p) + 1);
	} else
		console_text[0] = 0;
}

void console_scroll(unsigned long seconds) {
	if(!time_stamp)
		time_stamp = getcputime();
	auto d = getcputime() - time_stamp;
	if(d > seconds) {
		console_delete_line();
		time_stamp = getcputime();
	}
}

static int get_line_count() {
	if(console_text[0] == 0)
		return 0;
	auto count = 1;
	auto p = console_text;
	while(*p) {
		if(*p == '\n')
			count++;
		p++;
	}
	return count;
}

void consolens(char sym) {
	auto n = zlen(console_text);
	if(!n)
		return;
	auto s = console_text[n - 1];
	if(s == sym)
		return;
	char temp[2] = {sym, 0};
	console(temp);
}

void console(const char* format, ...) {
	XVA_FORMAT(format);
	consolev(format, format_param);
}

void consolen(const char* format, ...) {
	if(!format || !format[0])
		return;
	consolens('\n');
	XVA_FORMAT(format);
	consolev(format, format_param);
}

void consolev(const char* format, const char* format_param) {
	if(!format || !format[0])
		return;
	stringbuilder sb(console_text);
	sb.setend();
	sb.addv(format, format_param);
	while(get_line_count() > 3)
		console_delete_line();
	time_stamp = getcputime();
}

void console_clear() {
	console_text[0] = 0;
}
