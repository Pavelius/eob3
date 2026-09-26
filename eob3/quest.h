#pragma once

#include "slice.h"

struct sitei;

enum questn : unsigned char;

enum variablen : unsigned char {
	Reputation, GoldCoins, Blessing,
	LastVariable = Blessing
};
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

struct variablei {
	int	variables[LastVariable + 1] = {};
	constexpr variablei() = default;
	template<typename... Ts> constexpr variablei(variablen v, int n, Ts... args) : variablei(args...) { add(v, n); }
	constexpr void add(variablen v, int n) { variables[v] += n; }
	constexpr int get(variablen v) const { return variables[v]; }
};
extern variablei game;

struct questi {
	char			difficult; // Difficult is 0-5, where 0 is start quest, 1 lower difficult, 5 is toughess boss.
	variablei		rewards; // Reward, if quest is done.
	slice<sitei>	dungeon; // Main quest dungeon
	questfc			state; // Current quest state. Can be serialzed.
};
extern questi quests[128]; // All quest predifined data.

int quest_count(questfn v);