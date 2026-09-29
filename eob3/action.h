#pragma once

typedef bool(*fncondition)();
typedef void(*fnevent)();

struct item;

enum abilityn : unsigned char;
enum classn : unsigned char;
enum messagen : unsigned char;
enum picturen : unsigned char;
enum shopn : unsigned char;
enum soundn : unsigned char;

enum actionn : unsigned char {
	NoAction,
	Tavern, Blacksmith, Temple, Inn, WizardTower, GoAdventure,
	PickPocketsAction, EatFoodAndDrink, EatAndDrinkSuccess,
	Gambling, GamblingIntro, GamblingWin, GamblingLose,
	Carousing,
	BuyWeapons, BuyWeaponsEmpty,
	RepairWeapons,
	RestParty, ScribleScrolls,
	PlayerExhaused,
	LeaveOutside,
	LastAction = LeaveOutside
};
enum variablen : unsigned char {
	Reputation, Coins, Blessing, Time,
	LastVariable = Time
};

extern const char* action_names[LastAction + 1];
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
	fnevent		proc; // This action resolve procedures
	fncondition allow; // Allow action function. Can be null.
	constexpr explicit operator bool() const { return action != NoAction; }
};

picturen get_picture(actionn v);
soundn get_music(actionn v);

int get_hour(); // Return 0..23

bool apply_action(const actioni* p);
void buy_item(shopn shop, actionn shop_empty);
bool check_activity();
long choose_player_action(const char* cancel);
long choose_message(actionn id, bool can_cancel = true);
bool confirm_message(messagen header, int value);
bool enough(const variablei& v1, const variablei& v2);
void for_each_party(fnevent proc);
bool indoor(actionn v);
bool need_activity(actionn v);
bool pass_payment(actionn action, const variablei& required);
void pass_time(unsigned minutes);
void setv(picturen v);
void show_message(actionn id, ...);

item* choose_buy_item(shopn shop, actionn shop_empty);

const actioni* choose_location(const actioni* source);
const actioni* choose_building_action(const actioni* source);