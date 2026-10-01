#pragma once

#include "action.h"
#include "slice.h"

struct sitei;

enum questn : unsigned char;

enum questfn : unsigned char {
	QuestPrepared, QuestPassed,
};

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
	questn index() const;
	bool is(questfn v) const { return state.is(v); }
	void set(questfn v) { return state.set(v); }
};
extern questi quests[128]; // All quest predifined data.
extern questi* last_quest;

questi* active_quest();

int quest_count(questfn v);