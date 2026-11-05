#pragma once

enum resn : unsigned char;

struct effecti {
	typedef bool(*fnproc)(resn res, short unsigned frame, short unsigned count, unsigned tick);
	fnproc			proc;
	resn			res;
	short unsigned	frame, count;
	unsigned		start;
	constexpr explicit operator bool() const { return proc != 0; }
	void clear() { proc = 0; }
	void paint(unsigned current_tick);
	void set(fnproc proc, resn res, short unsigned frame, short unsigned count, unsigned start);
};