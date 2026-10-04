#pragma once

typedef bool(*fncondition)();
typedef void(*fnevent)();
typedef void(*fninstant)(int value);

struct spelli {
	const char*	name;
	char		levels[2]; // 0 - priest, 1 - mage, 2 - other
	fninstant	instant;
	fncondition	test;
	fnevent		wear;
};
