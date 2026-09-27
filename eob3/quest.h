#pragma once

#include "game.h"
#include "slice.h"

struct sitei;

enum questn : unsigned char;

enum questfn : unsigned char {
	QuestActive, QuestPassed, QuestPrepared,
};

extern const char* variable_names[LastVariable + 1];

struct questfc {
	unsigned char data = 0;
	questfc() = default;
	template<typename... Ts> constexpr questfc(questfn v, Ts... args) : questfc(args...) { set(v); }
	bool is(questfn v) const { return (data & (1 << v)) != 0; }
	void set(questfn v) { data |= (1 << v); }
};

struct questi {
	char			difficult; // Difficult is 0-5, where 0 is start quest, 1 lower difficult, 5 is toughess boss.
	variablei		rewards; // Reward, if quest is done.
	slice<sitei>	dungeon; // Main quest dungeon
	questfc			state; // Current quest state. Can be serialzed.
};
extern questi quests[128]; // All quest predifined data.

int quest_count(questfn v);