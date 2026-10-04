#pragma once

typedef void(*fnevent)();

enum messagen : unsigned char;

struct valuei {
	enum typen : unsigned char {
		Unsigned, Signed, Bool,
	};
	typen		type;
	void*		source;
	unsigned	size;
	constexpr valuei() : type(Unsigned), source(0), size(0) {}
	constexpr valuei(unsigned& v) : type(Unsigned), source(&v), size(sizeof(v)) {}
	constexpr valuei(int& v) : type(Signed), source(&v), size(sizeof(v)) {}
	constexpr valuei(unsigned short& v) : type(Unsigned), source(&v), size(sizeof(v)) {}
	constexpr valuei(short& v) : type(Signed), source(&v), size(sizeof(v)) {}
	constexpr valuei(unsigned char& v) : type(Unsigned), source(&v), size(sizeof(v)) {}
	constexpr valuei(char& v) : type(Signed), source(&v), size(sizeof(v)) {}
	constexpr valuei(bool& v) : type(Bool), source(&v), size(sizeof(v)) {}
	constexpr explicit operator bool() const { return size != 0; }
	long get() const;
	void set(long value) const;
};

struct perferencei {
	messagen	id; // Name of setting
	valuei		value; // Value of setting.
	fnevent		proc; // After value set.
	constexpr explicit operator bool() const { return id != (messagen)0; }
};
