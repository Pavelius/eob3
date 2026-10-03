#pragma once

const int walls_frames = 9;
const int walls_count = 6;
const int door_offset = 1 + walls_frames * walls_count;
const int decor_offset = door_offset + 9;
const int decor_count = 21;
const int decor_frames = 10;
const int scrx = 22 * 8;
const int scry = 15 * 8;

struct creature;
struct item;
struct sprite;
struct pointca;

typedef void(*fnevent)(); // Callback function of any command executing
typedef void(*fnoutput)(const char* format); // Callback function of string out
typedef void(*fnapaint)(int index, long value, const char* text, unsigned key);

enum celln : unsigned char;
enum variablen : unsigned char;
enum questn : unsigned char;
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
	ConfirmDeleteCharacter, ConfirmBuyHealing, ConfirmBuyItem, ConfirmEatAndDrink, ConfirmRentRoom, ConfirmCarousing,
	ConfirmRestParty, ConfirmReturnCity, ConfirmMakeCamp,
	WhatPlayerDo, WhichWayToGo, WhatYouWantToBuy,
	HowYouGetHere,
	PlayerIsDisabled, PlayerPickUpItem,
	PartyGoingUp, PartyGoingDown, PartyFallPit,
	DoorOpened, SneakAttack, FeelPoison,
	MagicDevice, Portal, TrapLauncher,
	LookUnknown, LookDrainageGate, LookStrangeDevice, LookUnknownTheifSign, Button, Cellar,
	QuestGoals, VisitBuilding,
	Ambush, Attack, CalmDown, Hunt, Talk, Lie, Bribe,
	Class, Race,
	LastMessage = Race
};
enum resn : unsigned char {
	FONT6, FONT8,
	BLUE, BRICK, DROW, DUNG, FOREST, GREEN, XANATHA,
	KOBOLD, LEECH,
	BORDER, CHARGEN, CHARGENB, COMPASS, INVENT,
	ITEMGS, ITEMGL, ITEMS, OVERLAYS, DECORS,
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
	PicAdaque, PicAmaldo, PicDexter, PicNord1, PicNord2, PicPriestessSilune, PicWitch,
	PicCity, PicCityNight, PicDwarvenCity,
	PicTavern, PicTavern2, PicInn, PicPickpockets, PicGambling, PicForge, PicTemple, PicMagicShop,
	TavHobbit,
	LastPicture = TavHobbit
};

extern const char* direction_names[Down + 1];
extern const char* message_names[LastMessage + 1];

inline const char* getnm(directionn v) { return direction_names[v]; }
inline const char* getnm(messagen v) { return message_names[v]; }

extern sprite* res_data[LastRes + 1];
extern bool interactive;
extern int generate_player_index;
extern unsigned long current_cpu_time;
extern bool need_update_animation;

int getv(variablen v);
int party_count();
int roll_dice(int v);

void addv(variablen v, int i);
void alternate_focus_input();
void animation_update();
void button_frame(int count, bool focused, bool pressed);
void button_label(int index, long data, const char* format, unsigned key);
void carousing();
void city_input();
bool confirm(const char* format);
bool confirm(messagen header);
bool confirm_message(messagen header, int value);
void correct_answers(int maximum);
bool chance(int v);
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
void enter_dungeon(int level, celln location);
void fix_animate();
void fix_attack(const creature* attacker, wearn slot, int hits);
void fix_damage(const creature* target, int value);
void fix_monster_attack(const creature* target);
void fix_monster_attack_end(const creature* target);
void fix_monster_damage(const creature* target);
void fix_monster_damage_end();
void focus_input();
void game_generation();
void header_yellow(const char* format);
void initialize_gui();
void make_attacks(bool melee_combat);
void message_box(const char* format);
void next_scene(fnevent v);
void paint_dungeon();
void paint_main_menu();
void party_addexp(int value);
void party_turn_left();
void party_turn_right();
void pick_up_item();
long play_city();
void play_city_actions();
void play_dungeon();
void player_manipulate();
void printn(messagen id, ...);
bool running_scene();
void set_dungeon_tiles(resn type);
void show_automap(const pointca& markers, int explore_radius);
void show_automap(bool mshow_fog_of_war, bool mshow_secrets, bool mshow_party, const pointca* vred_markers);
void show_dungeon_images();
void show_scene(fnevent before_paint, fnevent input, long focus = -1);
void show_scene_font();
void show_sprites_command();
long show_message(const char* format, bool add_anaswers, const char* cancel, unsigned cancel_key);
void text_label(int index, long data, const char* format, unsigned key);
void text_label_left(int index, long data, const char* format, unsigned key);