#pragma once

enum classn : unsigned char;

enum resultn : unsigned char {
	NoResult, Action, Failed, Successed, ReturnToParent
};
enum actionn : unsigned char {
	MainCity,
	Tavern, Blacksmith, Temple, Inn, WizardTower, GoAdventure,
	PickPocketsSomeone, EatFoodAndDrink, Gambling, Carousing,
	LeaveOutside,
	LastAction = LeaveOutside
};
enum variablen : unsigned char {
	Reputation, Coins, Blessing,
	LastVariable = Blessing
};

extern const char* action_names[LastAction + 1];
extern const char* action_text[LastAction + 1];
extern const char* variable_names[LastVariable + 1];

extern int last_number;

inline const char* getnm(actionn v) { return action_names[v]; }
inline const char* getnm(variablen v) { return variable_names[v]; }

struct classnc {
	unsigned char data = 0;
	classnc() = default;
	template<typename... Ts> constexpr classnc(classn v, Ts... args) : classnc(args...) { set(v); }
	constexpr explicit operator bool() const { return data != 0; }
	bool is(classn v) const { return (data & (1 << v)) != 0; }
	void set(classn v) { data |= (1 << v); }
};

struct variablei {
	int variables[LastVariable + 1] = {};
	constexpr variablei() = default;
	template<typename... Ts> constexpr variablei(variablen v, int n, Ts... args) : variablei(args...) { add(v, n); }
	constexpr explicit operator bool() const { for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1)) if(variables[i]) return true; return false; }
	constexpr bool operator>(const variablei& v) const { for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1)) if(variables[i] <= v.variables[i]) return false; return true; }
	constexpr bool operator>=(const variablei& v) const { for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1)) if(variables[i] < v.variables[i]) return false; return true; }
	constexpr bool operator==(const variablei& v) const { for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1)) if(variables[i] != v.variables[i]) return false; return true; }
	constexpr bool operator!=(const variablei& v) const { for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1)) if(variables[i] == v.variables[i]) return false; return true; }
	constexpr void add(variablen v, int n) { variables[v] += n; }
	constexpr int get(variablen v) const { return variables[v]; }
};
extern variablei game;

struct actioni {
	resultn		type;
	actionn		action; // What action do.
	variablei	required; // Pass this requitment to show action. Gold coins must be payed.
	classnc		restriction; // If filled, only this classes can use action.
	constexpr explicit operator bool() const { return type != NoResult; }
};