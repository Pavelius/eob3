#pragma once

#include "action.h"
#include "dungeon.h"
#include "slice.h"

struct sitei;

enum questn : unsigned char {
	FloodedCollectors, FintPathToForest,
	LastQuest = FintPathToForest,
};

enum soundn : unsigned char;

enum questfn : unsigned char {
	QuestPrepared, QuestPassed,
};

extern const char* quest_rumor[(LastQuest + 1) * 5];

class questfc {
	unsigned char data = 0;
public:
	questfc() = default;
	template<typename... Ts> constexpr questfc(questfn v, Ts... args) : questfc(args...) { set(v); }
	void clear() { data = 0; }
	bool is(questfn v) const { return (data & (1 << v)) != 0; }
	void set(questfn v) { data |= (1 << v); }
};

struct questi {
	char			difficult; // Difficult is 0-5, where 0 is start quest, 1 lower difficult, 5 is toughess boss.
	soundn			music; // Played music
	variablei		rewards; // Reward, if quest is done.
	slice<sitei>	sites; // Main quest dungeon
	goalc			goals;
	questfc			state; // Current quest state. Can be serialzed.
	unsigned char	rumor; // Maximum rumor is 5.
	questn index() const;
	void clear() { goals.clear(); rumor = 0; state.clear(); }
	bool identify() const { return rumor >= 5; }
	bool is(questfn v) const { return state.is(v); }
	void set(questfn v) { return state.set(v); }
};
extern questi quests[LastQuest + 1]; // All quest predifined data.

questi* active_quest();

int quest_count(questfn v);