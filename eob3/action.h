#pragma once

//enum resultn : unsigned char {
//	
//};
enum actionn : unsigned char {
	Tavern, Blacksmith, Temple, Inn, WizardTower, GoAdventure,
	PickPocketsSomeone, EatFoodAndDrink, Gambling, Carousing,
	LastAction = Carousing
};
enum resultn : unsigned char {
	NoResult, BasicAction//, Failed, Successed
};
enum variablen : unsigned char {
	Reputation, GoldCoins, Blessing,
	LastVariable = Blessing
};

extern const char* action_names[LastAction + 1];
extern const char* action_text[LastAction + 1];
extern const char* variable_names[LastVariable + 1];

inline const char* getnm(actionn v) { return action_names[v]; }
inline const char* getnm(variablen v) { return variable_names[v]; }

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
	actionn		action;
	constexpr explicit operator bool() const { return type != NoResult; }
};