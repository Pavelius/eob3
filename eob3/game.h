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
struct perferencei;

typedef void(*fnevent)(); // Callback function of any command executing
typedef void(*fnoutput)(const char* format); // Callback function of string out
typedef void(*fnapaint)(int index, long value, const char* text, unsigned key);

enum alignmentn : unsigned char;
enum celln : unsigned char;
enum itemn : unsigned char;
enum variablen : unsigned char;
enum questn : unsigned char;
enum wearn : unsigned char;

enum directionn : unsigned char {
	Center, Left, Up, Right, Down,
	LeftUp, RightUp, LeftDown, RightDown
};
enum messagen : unsigned char {
	Cancel, Continue, Title, Yes, No, OK, Agree, Decline, On, Off,
	Music, Sound,
	Characterinfo, CharacterSkills, PartyStatusFormat, ReplaceCharacter, CastSpell,
	SelectRace, SelectGender, SelectClass, SelectAlignment,
	GeneraionInfo, GenerationPlayInfo,
	ConfirmDeleteCharacter, ConfirmBuyHealing, ConfirmSellItem, ConfirmBuyItem, ConfirmEatAndDrink, ConfirmRentRoom, ConfirmCarousing,
	ConfirmRestParty, ConfirmReturnCity, ConfirmMakeCamp, ConfirmExitGame,
	WhatPlayerDo, WhichWayToGo, WhatYouWantToBuy, CastOnWho,
	NoSpellsAvailable, SpellsAvailable,
	WelcomeFriends, WhoIsYou, WhatHappens, HowYouGetHere, YouLiers, HelpIdentify, HelpCursed, HelpGift, HelpKey, TalkBribe,
	LookUnknown, LookDrainageGate, LookStrangeDevice, LookUnknownTheifSign, LookTheifSign,
	AnimalStayStill,
	ItemPowerName, Rotten, Damaged, Few, Several, Many,
	PlayerIsDisabled, PlayerPickUpItem, PlayerCastSpell, PlayerDrinkPotion, PlayerFailedSkill, PlayerSuccessSkill,
	PlayerLearnSpell, PlayerLearnSpellFail, PlayerFailToUnderstandText, PlayerRead,
	AllPartyDead,
	PartyGoingUp, PartyGoingDown, PartyFallPit, PartyFailedSurprise, EnemyFailedSurprise,
	RaiseLevelSuccess,
	ItemBroken, DoorOpened, SneakAttack, FeelPoison, FeelDisease,
	MagicDevice, Portal, TrapLauncher,
	Button, Cellar,
	QuestGoals, VisitBuilding,
	MonstersAreNearbe,
	GameSaved, StateDead, StateDisabled, StateParalized, StatePoisoned, StateDiseased,
	YourFaithIsWeak, UseHolySymbolNoEffect, UseHolySymbolSuccess, UseHolySymbolFail, YourAbilityGrow,
	LootIdentyfing, LootSelling, UseLastAmmo, UseLastChange,
	CloseByNight,
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
	ChaAdaque, ChaAmaldo, ChaDexter, ChaNord1, ChaNord2, ChaPriestessSilune, ChaWitch, ChaRanger,
	PicCity, PicCityNight, PicDwarvenCity, PicClosed, PicForest, PicAdventure,
	PicTavern, PicTavern2, PicInn, PicPickpockets, PicPickpocketsFail, PicGambling, PicForge, PicTemple, PicMagicShop,
	PicShop, PicSellItems, PicTriumph, PicSewerEntrance, PicCarousingEnd,
	TavGreatPickPockets, TavDrinkContest, TavNudeNightRide, TavStoryPriest, TavHobbit,
	LastPicture = TavHobbit
};

extern const char* direction_names[Down + 1];
extern const char* message_names[LastMessage + 1];
extern const char* world_side_names[RightDown + 1];

inline const char* getnm(directionn v) { return direction_names[v]; }
inline const char* getnm(messagen v) { return message_names[v]; }

extern sprite* res_data[LastRes + 1];
extern bool interactive;
extern int generate_player_index;
extern int last_number;
extern unsigned long current_cpu_time;
extern bool need_update_animation;

int get_hour();
int getv(variablen v);
int roll_dice(int d);
int roll_dice(int c, int d, int b = 0);
int thrown_side(int avatar_thrown, int side);

void addv(variablen v, int i);
void alternate_focus_input();
void animation_update();
void button_frame(int count, bool focused, bool pressed);
void button_label(int index, long data, const char* format, unsigned key);
void carousing();
void cast_spell();
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
long choose_small_menu(const char* header, const char* cancel, int* columns);
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
void game_clear();
void game_file(char* result, const char* folder, const char* name, const char* ext, int index = 0); // Result is minimum 512 chars.
void game_generation();
bool game_serial(const char* id, bool write_mode, bool run_game);
void go_adventure();
void header_yellow(const char* format);
void initialize_gui();
bool if_blessing_good();
void load_game();
void main_menu();
void make_attacks(bool melee_combat);
void message_box(const char* format);
void monsters_talk_carefully();
void monsters_talk_friendly();
void next_scene(fnevent v);
void paint_dungeon();
void paint_main_menu();
void party_addexp(int value);
void party_addexp(int value, alignmentn alignment);
void party_addexp_each(int value);
void party_addexp_good(int value);
void party_addexp_evil(int value);
void party_turn_left();
void party_turn_right();
void pick_up_item();
long play_city();
void play_city_actions();
void play_dungeon();
void player_manipulate();
void printn(messagen id, ...);
void reaction_check(int bonus);
bool running_scene();
void set_dungeon_tiles(resn type);
void show_automap(const pointca& markers, int explore_radius);
void show_automap(bool mshow_fog_of_war, bool mshow_secrets, bool mshow_party, const pointca* vred_markers);
void show_dungeon_automap();
void show_dungeon_images();
void show_dungeon_options();
void show_scene_font();
void show_sprites_command();
void show_perferences(const char* header, const perferencei* actions);
long show_message(const char* format, bool add_anaswers, const char* cancel, unsigned cancel_key);
void text_label(int index, long data, const char* format, unsigned key);
void text_label_left(int index, long data, const char* format, unsigned key);