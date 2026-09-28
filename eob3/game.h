#pragma once

struct creature;
struct item;
struct sprite;

typedef void(*fnevent)(); // Callback function of any command executing
typedef void(*fnoutput)(const char* format); // Callback function of string out
typedef void(*fnapaint)(int index, long value, const char* text, unsigned key);

enum variablen : unsigned char;
enum wearn : unsigned char;

enum directionn : unsigned char {
	Center, Left, Up, Right, Down,
	LeftUp, RightUp, LeftDown, RightDown
};
enum messagen : unsigned char {
	Cancel, Continue, Title, Yes, No, OK, Agree, Decline,
	StartGame, LoadGame, ExitGame,
	Characterinfo, CharacterSkills, PartyStatusFormat,
	SelectRace, SelectGender, SelectClass, SelectAlignment,
	GeneraionInfo, GenerationPlayInfo,
	ConfirmDeleteCharacter, ConfirmBuyHealing, ConfirmEatAndDrink, ConfirmRentRoom, ConfirmCarousing,
	ConfirmRestParty,
	WhatPlayerDo, WhichWayToGo,
	PlayerIsDisabled,
	QuestGoals, VisitBuilding,
	Class, Race,
	LastMessage = Race
};
enum resn : unsigned char {
	FONT6, FONT8,
	BRICK, FOREST,
	KOBOLD, LEECH,
	BORDER, CHARGEN, CHARGENB, COMPASS, INVENT,
	ITEMGS, ITEMGL, ITEMS,
	MENU, PLAYFLD, PORTM, SCENE, THROWN, XSPL,
	LastRes = XSPL
};
enum soundn : unsigned char {
	NoMusic,
	MusAdept, MusAutomap, MusBinge, MusBlut, MusCamp, MusDepot, MusDiskmenu, MusFinster,
	MusFireGhost, MusGashok, MusGenerate, MusTravel, MusHealer, MusInn, MusKvirasim, MusLowangen,
	MusOptions, MusWin, MusTempleOfTheives, MusCombat, MusShop, MusSmith, MusDanger, MusTavern,
	MusTemple, MusEpic, MusTiefhus,
	MusDialog,
};
enum picturen : unsigned char {
	NoPicture,
	PicAdaque, PicAmaldo, PicDexter, PicNord1, PicNord2,
	PicCity, PicTavern, PicTavern2, PicInn, PicPickpockets, PicGambling, PicForge,
	LastPicture = PicForge
};

extern const char* direction_names[Down + 1];
extern const char* message_names[LastMessage + 1];

inline const char* getnm(directionn v) { return direction_names[v]; }
inline const char* getnm(messagen v) { return message_names[v]; }

extern sprite* res_data[LastRes + 1];
extern bool interactive;
extern int generate_player_index;

int getv(variablen v);

void addv(variablen v, int i);
void alternate_focus_input();
void button_frame(int count, bool focused, bool pressed);
void button_label(int index, long data, const char* format, unsigned key);
void city_input();
void common_input();
bool confirm(const char* format);
bool confirm(messagen header);
bool confirm_message(messagen header, int value);
void correct_answers(int maximum);
void change_avatar();
void change_character();
long choose_action(const char* cancel);
long choose_avatar(unsigned char* source, unsigned count);
long choose_dialog(const char* title, int padding);
long choose_generate_box(const char* header, const char* footer, int current);
long choose_generate_dialog(const char* header);
long choose_large_menu(const char* header, const char* cancel);
long choose_large_menu_no_player(const char* header, const char* cancel);
long choose_main_menu();
long choose_small_menu(const char* header, const char* cancel);
void choose_spells(const char* title, const char* cancel, int spell_type);
void fix_animate();
void fix_attack(const creature* attacker, wearn slot, int hits);
void fix_damage(const creature* target, int value);
void focus_input();
void game_generation();
void header_yellow(const char* format);
void initialize_gui();
void message_box(const char* format);
void paint_adventure();
void paint_small_menu();
void paint_main_menu();
void pick_up_item();
long play_city();
void play_city_actions();
void printn(messagen id, ...);
void show_scene(fnevent before_paint, fnevent input, long focus);
void show_scene_font();
void show_sprites_command();
long show_message(const char* format, bool add_anaswers, const char* cancel, unsigned cancel_key);
void text_label(int index, long data, const char* format, unsigned key);
void text_label_left(int index, long data, const char* format, unsigned key);