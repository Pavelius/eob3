#pragma once

typedef void(*fnevent)();

enum abilityn : unsigned char;
enum classn : unsigned char;
enum messagen : unsigned char;
enum picturen : unsigned char;
enum soundn : unsigned char;

enum actionn : unsigned char {
	NoAction,
	Tavern, Blacksmith, Temple, Inn, WizardTower, GoAdventure,
	PickPocketsSomeone, EatFoodAndDrink, Gambling, Carousing,
	RestParty, ScribleScrolls,
	LeaveOutside,
	LastAction = LeaveOutside
};
enum variablen : unsigned char {
	Reputation, Coins, Blessing, Time,
	LastVariable = Time
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
	constexpr bool operator==(const variablei& v) const { for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1)) if(variables[i] != v.variables[i]) return false; return true; }
	constexpr bool operator!=(const variablei& v) const { for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1)) if(variables[i] == v.variables[i]) return false; return true; }
	constexpr int get(variablen v) const { return variables[v]; }
	constexpr void add(variablen v, int n) { variables[v] += n; }
};
extern variablei game;

struct actioni {
	actionn		action; // What action do.
	variablei	required; // Pass this requitment to show action. Gold coins must be payed.
	classnc		restriction; // If filled, only this classes can use action.
	fnevent		success, fail; // This action outcome procedures
	abilityn	ability; // If fail defined, roll this ability
	char		bonus; // Bonus to roll for ability
	constexpr explicit operator bool() const { return action != NoAction; }
};

picturen get_picture(actionn v);
soundn get_music(actionn v);

long choose_player_action(const char* cancel);
bool confirm_message(messagen header, int value);
bool enough(const variablei& v1, const variablei& v2);
bool indoor(actionn v);
bool pass_payment(actionn action, const variablei& required);
void pass_time(unsigned minutes);