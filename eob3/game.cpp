#include "action.h"
#include "answers.h"
#include "console.h"
#include "creature.h"
#include "collectiona.h"
#include "dungeon.h"
#include "game.h"
#include "math.h"
#include "perference.h"
#include "pushvalue.h"
#include "quest.h"
#include "rand.h"
#include "sound.h"
#include "stringbuilder.h"
#include "view_focus.h"

variablei game;
static long save_focus;
static item* valuable_item;

void thrown_item(pointc v, directionn d, int avatar_thrown, int side, int distance);

static messagen get_confirm(actionn v) {
	switch(v) {
	case Carousing: return ConfirmCarousing;
	case Inn: return ConfirmRentRoom;
	case EatFoodAndDrink: return ConfirmEatAndDrink;
	default: return (messagen)0;
	}
}

picturen get_picture(actionn v) {
	switch(v) {
	case NoAction: return PicCity;
	case Blacksmith: return PicForge;
	case Carousing: return PicTavern2;
	case EatFoodAndDrink: return PicTavern2;
	case Gambling: return PicGambling;
	case Inn: return PicInn;
	case Palace: return PicAmaldo;
	case PickPocketsAction: return PicPickpockets;
	case Tavern: return PicTavern;
	case WizardTower: return PicMagicShop;
	case Temple: return PicTemple;
	default: return NoPicture;
	}
}

soundn get_music(actionn v) {
	switch(v) {
	case NoAction: return MusKvirasim;
	case Blacksmith: return MusSmith;
	case Inn: return MusInn;
	case Palace: return MusDialog;
	case Tavern: return MusTavern;
	case Temple: return MusTemple;
	case WizardTower: return MusHealer;
	default: return NoMusic;
	}
}

static bool is_valuable(item& e) {
	switch(e.type) {
	case PurpleGem: case RedGem: case BlueGem: case GreenGem:
		return true;
	case BlueRing: case GreenRing: case RedRing:
		return e.identified != 0 && !e.power;
	default:
		return false;
	}
}

bool indoor(actionn v) {
	return v >= Tavern && v <= Palace;
}

void addv(variablen v, int i) {
	game.add(v, i);
}

int getv(variablen v) {
	return game.variables[v];
}

static void all_monsters(fnevent proc, bool allow_disabled) {
	pushvalue push(player);
	if(!loc)
		return;
	for(auto& e : loc->monsters) {
		if(!e)
			continue;
		if(!allow_disabled && e.isdisabled())
			continue;
		player = &e; proc();
	}
}

void all_party(fnevent proc, bool allow_disabled) {
	pushvalue push(player);
	for(auto p : adventurers) {
		if(!p)
			continue;
		if(!allow_disabled && p->isdisabled())
			continue;
		player = p; proc();
	}
}

static void all_creatures(fnevent proc) {
	all_party(proc, true);
	all_monsters(proc, true);
}

int roll_dice(int v) {
	if(!v)
		return 0;
	return 1 + rand() % v;
}

int quest_count(questfn v) {
	auto result = 0;
	for(auto& e : quests) {
		if(e.state.is(v))
			result++;
	}
	return result;
}

static void party_items(collection<item>& result, fnvisible filter, bool keep) {
	for(auto p : adventurers) {
		if(!p)
			continue;
		for(auto& e : p->backpack()) {
			if(!e)
				continue;
			if(filter && filter(&e) != keep)
				continue;
			result.add(&e);
		}
	}
}

questi* active_quest() {
	for(auto& e : quests) {
		if(e.is(QuestPrepared) && !e.is(QuestPassed))
			return &e;
	}
	return 0; // You win game. All quest is done.
}

questn questi::index() const {
	return (questn)(this - quests);
}

bool check_activity() {
	if(player->food <= 6) {
		show_message(PlayerExhaused);
		return false;
	}
	return true;
}

void pass_activity() {
	pass_time(xrand(20, 40));
	auto roll = d20();
	auto efforts = 10; // Maximum effort consumed
	// AD&D standart success roll must be lesser that ability
	if(roll < player->abilities[Constitution]) {
		efforts -= player->abilities[Constitution] - roll;
		if(efforts < 1)
			efforts = 1;
	}
	player->food -= efforts;
	if(player->food < 0)
		player->food = 0;
}

bool enough(const variablei& v1, const variablei& v2) {
	for(auto i = (variablen)0; i <= LastVariable; i = (variablen)(i + 1))
		if(v1.variables[i] < v2.variables[i])
			return false;
	return true;
}

static bool can_see_party(pointc v, directionn d) {
	for(auto i = 0; i < 3; i++) {
		v = to(v, d);
		if(!v || !loc->ispassable(v))
			return false;
		if(v == party.pos)
			return true;
	}
	return false;
}

static directionn random_free_look(pointc v, directionn d, bool monster_forbidden) {
	directionn source[] = {Up, Left, Right, Down};
	if(d100() < 50) // Monster move until reach in wall. Then usually turn. Rare turn back.
		iswap(source[1], source[2]);
	for(auto nd : source) {
		auto d1 = to(d, nd);
		auto v1 = to(v, d1);
		if(!v1)
			continue;
		if(!loc->ispassable(v1))
			continue;
		if(monster_forbidden && loc->isforbidden(v1))
			continue;
		if(loc->ismonster(v1))
			continue;
		return d1;
	}
	return Center;
}

static void update_party_position() {
	for(size_t i = 0; i < lengthof(adventurers); i++) {
		if(!adventurers[i])
			continue;
		adventurers[i]->side = (char)i;
		adventurers[i]->pos = party.pos;
		adventurers[i]->d = party.d;
	}
}

static void party_set(creature** source, directionn d) {
	for(auto i = 0; i < lengthof(adventurers); i++) {
		if(!source[i])
			continue;
		adventurers[i]->d = d;
	}
}

static void party_set(pointc v) {
	party.pos = v;
	update_party_position();
}

static void party_set(posable v) {
	party = v;
	party.pos = to(party.pos, party.d);
	update_party_position();
}

static void party_set(pointc v, directionn d) {
	party.pos = v;
	party.d = d;
	update_party_position();
}

void turnto(pointc v, directionn d, bool test_surprise) {
	if(!d)
		return;
	if(v == party.pos) {
		if(test_surprise) {
			if(party.d != d) {
				creature* monsters[6]; loc->getmonsters(monsters, to(v, d));
				// surprise_roll(characters, party_sneaky(monsters));
			}
		}
		party_set(v, d);
	} else {
		creature* monsters[6] = {}; loc->getmonsters(monsters, v, party.d);
		for(int i = 0; i < 6; i++) {
			auto p = monsters[i];
			if(!p || p->isdisabled())
				continue;
			if(test_surprise) {
				if(p->d != d) {
					// surprise_roll(monsters, party_sneaky(characters));
					test_surprise = false;
				}
			}
			p->d = d;
		}
	}
}

static void monsters_stop(pointc v) {
	if(!v || !loc)
		return;
	for(auto& e : loc->monsters) {
		if(e.pos == v)
			e.set(Moved);
	}
}

static void monsters_move(pointc v, directionn d) {
	auto n = to(v, d);
	if(n == party.pos) {
		monsters_stop(v);
		turnto(party.pos, to(d, Down), true);
		reaction_check(0);
		return;
	}
	if(!n || !loc->ispassable(n) || loc->isforbidden(n))
		return;
	if(loc->ismonster(n))
		monsters_stop(v);
	else {
		for(auto& e : loc->monsters) {
			if(e.pos == v) {
				e.d = d;
				if(!n)
					e.set(Moved);
				e.pos = n;
				e.set(Moved);
			}
		}
	}
}

static void monsters_movement() {
	if(!loc)
		return;
	for(auto& e : loc->monsters) {
		if(!e || e.isdisabled() || e.is(Moved))
			continue;
		if(can_see_party(e.pos, e.d))
			monsters_move(e.pos, e.d);
		else if(e.roll(Dexterity)) {
			auto d = random_free_look(e.pos, e.d, true);
			if(d != Center)
				monsters_move(e.pos, d);
		} else
			monsters_stop(e.pos);
	}
}

static void check_regeneration() {
	if(player->is(Regenerated))
		player->heal(1);
}

static void check_poison() {
	if(player->is(StoppedPoison))
		return;
	if(!player->is(PoisonLevel))
		return;
	auto penalty = player->get(PoisonLevel) / 5;
	if(!player->roll(SaveVsPoison, -penalty))
		player->damage(PoisonDamage, 1);
	player->add(PoisonLevel, -1);
}

static void check_acid() {
	auto damage = 0;
	if(player->is(AcidD1Level)) {
		damage += xrand(1, 4);
		player->add(AcidD1Level, -1);
	}
	if(player->is(AcidD2Level)) {
		damage += xrand(1, 4);
		player->add(AcidD2Level, -1);
	}
	if(damage)
		player->damage(AcidDamage, damage);
}

static void check_disease() {
	if(!player->is(DiseaseLevel))
		return;
	// Two test in row to overcome disease or two failed tests to get worse
	if(player->roll(SaveVsPoison, 0)) {
		if(player->roll(SaveVsPoison, 0))
			player->add(DiseaseLevel, -1);
	} else {
		if(!player->roll(SaveVsPoison, 0)) {
			player->add(DiseaseLevel, 1);
			consolen(getnm(FeelDisease));
		}
	}
	// Reduce hp (can die if disease level high)
	if(player->is(DiseaseLevel)) {
		auto m = player->hpm / 3;
		if(player->get(DiseaseLevel) > 10)
			m = 0;
		if(player->hp > m)
			player->hp--;
	}
}

static void update_every_round() {
	player->remove(Moved);
	update_player();
	check_regeneration();
	check_acid();
}

static void check_food() {
	if(player->roll(Constitution))
		return;
	if(player->food > 0)
		player->food--;
	else {
		if(chance(40))
			player->say(IAmTired);
		player->damage(HealthDamage, 1);
	}
}

static void check_goals() {
}

static void check_return_to_base() {
	all_party(check_food, true);
}

static void update_every_turn() {
}

static void update_every_hour() {
	check_disease();
}

static bool player_damage(int attack_type, damagen type, dice damage, const featn effect) {
	auto hits = damage.roll();
	switch(attack_type) {
	case 0:
		if(player->roll(SaveVsTraps))
			return false;
		break;
	case 1:
		if(player->roll(SaveVsTraps))
			hits = hits / 2;
		break;
	case 2:
		if(d100() < 30 + player->get(AC) * 5)
			return true; // Player count hitted
		break;
	default:
		break;
	}
	if(!hits)
		return false;
	player->damage(type, hits);
	return true;
}

static void group_damage(creature** creatures, pointc v, directionn d, const combati& ei) {
	pushvalue push(player);
	auto test_projectile = false;
	auto targets = ei.number_attacks;
	for(auto i = 0; i < 6 && targets > 0; i++) {
		auto n = get_side_ex(i, d);
		auto p = creatures[n];
		if(!p || p->isdead())
			continue;
		player = p;
		if(player_damage(ei.attack, ei.type, ei.damage, ei.effect)) {
			test_projectile = true;
			targets--;
		}
	}
	if(ei.ammo && test_projectile && d100() < 30) {
		item it(ei.ammo);
		loc->drop(v, it, xrand(0, 3));
	}
}

static void trap_launch(pointc v, directionn d, int avatar, const combati& ei) {
	auto start = v;
	while(v) {
		if(party.pos == v) {
			if(to(party.d, Down) == d && party.pos.x == v.x || party.pos.y == v.y)
				thrown_item(start, Down, avatar, thrown_side(avatar, 1), start.distance(party.pos) + 1);
			group_damage(adventurers, v, to(party.d, d), ei);
			break;
		} else if(loc->ismonster(v)) {
			creature* creatures[6]; loc->getmonsters(creatures, v);
			group_damage(creatures, v, d, ei);
			break;
		} else {
			auto t = loc->get(v);
			if(t == CellDoor && loc->is(v, CellActive))
				break;
			else if(t == CellWall || t == CellStairsUp || t == CellStairsDown || t == CellWeb)
				break;
		}
		v = to(v, d);
	}
}

static void trap_launch(pointc v, directionn d) {
	trap_launch(v, d, 0, traps[loc->trap]);
}

static void update_floor_state() {
	if(!loc)
		return;
	unsigned char map[mpy][mpx] = {0};
	loc->state.monsters_alive = 0;
	loc->state.items_lying = 0;
	loc->state.explored_passable = loc->getpassables(true);
	if(party)
		map[party.pos.y][party.pos.x]++;
	for(auto& e : loc->monsters) {
		if(!e)
			continue;
		loc->state.monsters_alive++;
		if(map[e.pos.y][e.pos.x] > 0)
			continue;
		map[e.pos.y][e.pos.x]++;
	}
	for(auto& e : loc->items) {
		if(!e)
			continue;
		loc->state.items_lying++;
		if(map[e.pos.y][e.pos.x] > 0)
			continue;
		map[e.pos.y][e.pos.x]++;
	}
	pointc pt;
	for(pt.y = 0; pt.y < mpy; pt.y++) {
		for(pt.x = 0; pt.x < mpx; pt.x++) {
			auto t = loc->get(pt);
			if(t == CellButton) {
				auto new_active = map[pt.y][pt.x] > 0;
				auto active = loc->is(pt, CellActive);
				if(active != new_active && new_active) {
					auto po = loc->getlinked(pt);
					if(po) {
						switch(po->type) {
						case CellTrapLauncher:
							if(!po->is(CellActive))
								trap_launch(po->pos, to(po->d, Down));
							break;
						default:
							break;
						}
					}
				}
				if(new_active)
					loc->set(pt, CellActive);
				else
					loc->remove(pt, CellActive);
			}
		}
	}
}

static size_t shrink(creature** result, creature** source) {
	auto ps = result;
	for(size_t i = 0; i < party_size; i++) {
		if(source[i])
			*ps++ = source[i];
	}
	return ps - result;
}

static slice<creature*> random_party() {
	static creature* monsters[6];
	auto count = shrink(monsters, adventurers);
	zshuffle(monsters, count);
	return slice<creature*>(monsters, count);
}

static bool check_secrets(directionn d) {
	if(!loc)
		return false;
	auto po = loc->get(party.pos, to(party.d, d));
	if(!po || po->type != CellSecretButton)
		return false;
	for(auto p : random_party()) {
		if(!p->roll(DetectSecrets, -10))
			continue;
		p->say(ISeeSomething, direction_names[d]);
		return true;
	}
	return false;
}

static void check_secrets() {
	if(check_secrets(Right))
		return;
	if(check_secrets(Left))
		return;
}

static bool check_noises_behind_door(directionn d) {
	if(!loc)
		return false;
	auto v = to(party.pos, to(party.d, d));
	auto t = loc->get(v);
	if(t != CellDoor || loc->is(v, CellActive) || loc->is(v, CellExperience))
		return false;
	creature* monsters[6]; loc->getmonsters(monsters, to(v, to(party.d, d)));
	auto count = shrink(monsters, monsters);
	loc->set(v, CellExperience);
	for(auto p : random_party()) {
		if(!p->roll(HearNoise))
			continue;
		p->addexp(20);
		if(count) {
			if(count == 1 && monsters[0]->islarge())
				p->say(IHearSomethingLarge);
			else
				p->say(IHearSomething, count);
		} else
			p->say(BehideThisDoorIsNoOne);
		return true;
	}
	return true;
}

static void check_noises_behind_door() {
	if(check_noises_behind_door(Right))
		return;
	if(check_noises_behind_door(Left))
		return;
	if(check_noises_behind_door(Up))
		return;
}

static bool party_disabled() {
	for(auto p : adventurers) {
		if(p && p->isready())
			return false;
	}
	return true;
}

static void pass_time_activity() {
	auto minute = getv(Time);
	check_boost(minute);
	monsters_movement();
	update_floor_state();
	check_secrets();
	check_noises_behind_door();
	all_creatures(update_every_round);
	if((minute % 6) == 0) {
		all_party(check_food, true);
		all_creatures(update_every_turn);
	}
	if((minute % 20) == 0)
		check_return_to_base();
	if((minute % 60) == 0)
		all_creatures(update_every_hour);
	check_goals();
	fix_animate();
	if(party_disabled()) {
		message_box(getnm(AllPartyDead));
		next_scene(main_menu);
	}
}

void pass_time(unsigned minutes) {
	game.variables[Time] += minutes;
	pass_time_activity();
}

int get_hour() {
	return (game.get(Time) / 60) % 24;
}

bool pass_payment(actionn action, const variablei& required) {
	if(required.variables[Coins]) {
		auto message = get_confirm(action);
		if(message) {
			if(!confirm_message(message, required.variables[Coins]))
				return false;
		}
		game.variables[Coins] -= required.variables[Coins];
	}
	return true;
}

long choose_player_action(const char* cancel) {
	char temp[32]; stringbuilder sb(temp);
	sb.add(message_names[WhatPlayerDo], player->name());
	return choose_large_menu(temp, cancel);
}

bool confirm_message(messagen header, int value) {
	char temp[512]; stringbuilder sb(temp);
	sb.add(getnm(header), value);
	an.add(1, getnm(Agree));
	return show_message(temp, true, getnm(Decline), 0) != 0;
}

void show_message(actionn id, ...) {
	XVA_FORMAT(id);
	char temp[512]; stringbuilder sb(temp);
	sb.addv(getnm(id), format_param);
	show_message(temp, false, getnm(Continue), '\n');
}

long choose_message(actionn id, bool can_cancel) {
	return show_message(getnm(id), true, can_cancel ? getnm(Cancel) : 0, 27);
}

static bool pass_restriction(const classnc& v) {
	if(!v)
		return true;
	auto n = get_class_count(player->type);
	for(auto i = 0; i < n; i++) {
		if(v.is(get_class(player->type, i)))
			return true;
	}
	return false;
}

void setv(picturen v) {
	if(!v)
		return;
	answer_picture = v;
	// Day/Night
	if(answer_picture == PicCity) {
		auto n = get_hour();
		if(n < 7 || n > 22)
			answer_picture = PicCityNight;
	}
}

bool apply_action(const actioni* p) {
	pushvalue push(answer_picture);
	setv(get_picture(p->action));
	if(!pass_payment(p->action, p->required))
		return false;
	if(p->proc)
		p->proc();
	return true;
}

bool chance(int v) {
	return d100() < v;
}

item* choose_buy_item(shopn shop, actionn shop_empty) {
	if(!allow(shop)) {
		show_message(shop_empty);
		return 0;
	}
	for(auto& e : shops[shop]) {
		if(!e)
			continue; // Can't pay or other reputation
		an.add((long)&e, e.name());
	}
	an.sort();
	return (item*)choose_large_menu(getnm(WhatYouWantToBuy), getnm(Cancel));
}

static bool confirm_action(messagen id, ...) {
	XVA_FORMAT(id);
	char temp[512]; stringbuilder sb(temp);
	sb.addv(getnm(id), format_param);
	return confirm(temp);
}

void buy_item(shopn shop, actionn shop_empty) {
	while(running_scene()) {
		auto pi = choose_buy_item(shop, shop_empty);
		if(!pi)
			break;
		auto cost = pi->getcost();
		if(!confirm_action(ConfirmBuyItem, cost))
			continue;
		game.add(Coins, -cost);
		player->additem(*pi);
		normalize_shop(shop);
	}
}

static void ask_actions(const actioni* source) {
	if(!source)
		return;
	for(auto p = source; *p; p++) {
		if(p->required && !enough(game, p->required))
			continue; // Can't pay or other reputation
		if(!pass_restriction(p->restriction))
			continue; // Not pass restriction
		if(p->allow && !p->allow())
			continue;
		if(p->action >= Tavern && p->action <= Palace)
			an.add((long)p, getnm(VisitBuilding), getnm(p->action));
		else
			an.add((long)p, getnm(p->action));
	}
}

const actioni* choose_location(const actioni* source) {
	if(!source)
		return 0;
	ask_actions(source);
	return (actioni*)choose_large_menu_no_player(getnm(WhichWayToGo), getnm(Cancel));
}

const actioni* choose_action(const actioni* source) {
	if(!source)
		return 0;
	ask_actions(source);
	return (actioni*)choose_player_action(getnm(Cancel));
}

void open_options(const actioni* actions) {
	pushfocus push;
	current_focus = empty_focus;
	while(running_scene()) {
		auto p = choose_action(actions);
		if(!p)
			break;
		apply_action(p);
	}
}

void show_perferences(const char* header, const perferencei* actions) {
	pushfocus push;
	current_focus = empty_focus;
	while(running_scene()) {
		an.clear();
		for(auto p = actions; *p; p++) {
			auto value = p->value.get();
			auto format = "%1: %2i";
			if(p->value.type == valuei::Bool)
				format = "%1 %3";
			an.add((long)p, format, getnm(p->id), value, getnm(value ? On : Off));
		}
		auto p = (perferencei*)choose_large_menu(header, getnm(Cancel));
		if(!p)
			break;
		if(p->value.type == valuei::Bool)
			p->value.set(p->value.get() ? 0 : 1);
		if(p->proc)
			p->proc();
	}
}

int party_count() {
	auto n = 0;
	for(auto p : adventurers) {
		if(p && !p->isdisabled())
			n++;
	}
	return n;
}

void party_addexp(int value) {
	auto n = party_count();
	if(!n)
		return;
	value = (value + n - 1) / n;
	for(auto p : adventurers) {
		if(p && !p->isdisabled())
			p->addexp(value);
	}
}

void party_addexp(int value, alignmentn alignment) {
	for(auto p : adventurers) {
		if(p && !p->isdisabled() && p->is(alignment))
			p->addexp(value);
	}
}

void party_addexp_good(int value) {
	party_addexp(value, LawfulGood);
	party_addexp(value, NeutralGood);
	party_addexp(value, ChaoticGood);
}

void party_addexp_evil(int value) {
	party_addexp(value, LawfulEvil);
	party_addexp(value, NeutralEvil);
	party_addexp(value, ChaoticEvil);
}

static dungeoni* find_dungeon(int level) {
	return find_dungeon(loc->quest, level);
}

static item* find_item_to_get(pointc v, directionn d, int side) {
	dungeoni::ground* items[64];
	if(!loc)
		return 0;
	auto count = loc->getitems(items, lengthof(items), v);
	if(!count)
		return 0;
	int sides[5];
	sides[0] = side;
	sides[1] = get_side(side, Left);
	sides[2] = get_side(side, Up);
	sides[3] = get_side(side, Down);
	sides[4] = get_side(side, Right);
	for(size_t r = 0; r < lengthof(sides); r++) {
		auto s = sides[r];
		for(size_t i = 0; i < count; i++) {
			if(items[i]->side == s)
				return items[i];
		}
	}
	return 0;
}

static int get_side(const creature* p) {
	for(auto i = 0; i < 6; i++) {
		if(adventurers[i] == p) {
			if(i == 4)
				return 2;
			else if(i == 5)
				return 3;
			return i;
		}
	}
	return -1;
}

void pick_up_dungeon_item() {
	auto pi = (item*)current_focus;
	auto pn = get_creature(pi);
	if(!pn || *pi)
		return;
	auto gpi = find_item_to_get(party.pos, party.d, get_side(pn));
	if(!gpi)
		return;
	auto slot = get_wear(pi);
	if(!gpi->allow(slot))
		return;
	if(slot >= Head && slot <= LastBelt) {
		if(!pn->allow(gpi->type))
			return;
	}
	*pi = *gpi;
	gpi->clear();
	consolen(getnm(PlayerPickUpItem), pi->name());
}

static void explore_area() {
	if(!loc)
		return;
	loc->set(party.pos, CellExplored);
	loc->set(to(party.pos, Up), CellExplored);
	loc->set(to(party.pos, Down), CellExplored);
	loc->set(to(party.pos, Left), CellExplored);
	loc->set(to(party.pos, Right), CellExplored);
}

static void make_action() {
	explore_area();
}

static void dungeon_drop(itemn type) {
	item it(type);
	loc->drop(opponent->pos, it, get_side(opponent->side, party.d));
}

void party_turn_right() {
	party.d = to(party.d, Right);
	update_party_position();
}

void party_turn_left() {
	party.d = to(party.d, Left);
	update_party_position();
}

bool party_have(itemn type) {
	for(auto p : adventurers) {
		if(p && p->haveitem(type))
			return true;
	}
	return false;
}

static bool is_passable(pointc v) {
	if(!v)
		return false;
	auto& ei = cells[loc->get(v)];
	return ei.flags.is(Passable) || (ei.flags.is(PassableActivated) && loc->is(v, CellActive));
}

static void pit_fall_down() {
	if(!player->roll(ClimbWalls))
		player->damage(Bludgeon, xrand(3, 18));
}

static void leave_dungeon() {
	// TODO: Check passed goals. If all - close quest and apply reward.
}

static void drop_unique_loot(creature* player) {
}

static void monsters_talk(const char* format, ...) {
	if(!opponent || !opponent->monster)
		return;
	XVA_FORMAT(format);
	char temp[512]; stringbuilder sb(temp);
	sb.add("\"");
	sb.addv(format, format_param);
	show_message(temp, false, getnm(Continue), 0);
}

static long monsters_talk(messagen id, messagen id2 = (messagen)0, messagen id3 = (messagen)0) {
	if(!opponent || !opponent->monster)
		return 0;
	char temp[320]; stringbuilder sb(temp);
	if(opponent->race == Animal) {
		sb.add(getnm(AnimalStayStill));
	} else {
		sb.add("\"");
		sb.add(getnm(id));
		if(id2)
			sb.adds(getnm(id2));
		if(id3)
			sb.adds(getnm(id3));
		sb.add("\"");
	}
	animation_update();
	if(!an)
		return show_message(temp, false, getnm(Continue), 0);
	else
		return show_message(temp, true, 0, 0);
}

static bool monsters_talk(const actioni* actions, messagen id, messagen id2 = (messagen)0, messagen id3 = (messagen)0) {
	if(!opponent || !opponent->monster)
		return false;
	answer_picture = NoPicture;
	fix_animate();
	ask_actions(actions);
	auto result = (actioni*)monsters_talk(id, id2, id3);
	if(!result)
		return false;
	apply_action(result);
	return true;
}

static void monsters_flee() {
	if(!loc)
		return;
	pointca points;
	loc->block(false);
	loc->makewave(party.pos);
	points.select(8, 16);
	if(!points)
		return;
	auto v = points.random();
	creature* creatures[6]; loc->getmonsters(creatures, opponent->pos);
	for(auto p : creatures) {
		if(p) {
			p->pos = v;
			// p->set(Moved);
		}
	}
}

static void monsters_kill() {
	if(!loc || !opponent)
		return;
	creature* creatures[6]; loc->getmonsters(creatures, opponent->pos);
	for(auto p : creatures) {
		if(p && *p)
			p->kill();
	}
}

static void monsters_leave() {
	if(!loc || !opponent)
		return;
	creature* creatures[6]; loc->getmonsters(creatures, opponent->pos);
	for(auto p : creatures) {
		if(p && *p) {
			drop_unique_loot(p);
			loc->state.monsters_killed++;
			p->clear();
		}
	}
}

static void animal_hunt() {
	monsters_kill();
	dungeon_drop(Ration);
}

static bool party_move_interact(pointc v) {
	auto t = loc->get(v);
	switch(t) {
	case CellStairsUp:
		if(find_dungeon(loc->level - 1)) {
			enter_dungeon(loc->level - 1, CellStairsDown);
			consolen(getnm(PartyGoingUp));
		} else if(confirm(getnm(ConfirmReturnCity)))
			leave_dungeon();
		break;
	case CellStairsDown:
		if(find_dungeon(loc->level + 1)) {
			enter_dungeon(loc->level + 1, CellStairsUp);
			consolen(getnm(PartyGoingDown));
		} else if(confirm(getnm(ConfirmReturnCity)))
			leave_dungeon();
		break;
	case CellPit:
		party_set(v);
		loc = find_dungeon(loc->level + 1);
		all_party(pit_fall_down, true);
		consolen(getnm(PartyFallPit));
		enter_dungeon(loc->level, CellUnknown);
		pass_time();
		break;
	case CellOverlay1:
	case CellOverlay2:
	case CellOverlay3:
		// apply_script(getid<celli>(t), "Use", 0);
		break;
	default:
		return false;
	}
	return true;
}

static void get_opponents(creature** result) {
	loc->getmonsters(result, to(party.pos, party.d));
}

static item* party_valuable() {
	collection<item> result;
	for(auto p : adventurers) {
		if(!p)
			continue;
		for(auto& e : p->backpack()) {
			if(e && is_valuable(e))
				result.add(&e);
		}
	}
	return result.random();
}

void reaction_check(int bonus) {
	if(!loc)
		return;
	auto push_opponent = opponent;
	creature* opponents[party_size];
	get_opponents(opponents);
	check_reaction(opponents, bonus);
	while(true) {
		get_opponents(opponents);
		opponent = get_leader(opponents);
		if(!opponent)
			break;
		auto reaction = opponent->reaction;
		valuable_item = party_valuable();
		party_set(opponents, Moved);
		party_set(opponents, to(party.d, Down));
		if(reaction == Careful) {
			party_set(opponents, Surprised, false);
			party_set(adventurers, Surprised, false);
			monsters_talk(talk_carefully, WhoIsYou, HowYouGetHere);
		} else if(reaction == Friendly) {
			party_set(opponents, Surprised, false);
			party_set(adventurers, Surprised, false);
			monsters_talk(talk_friendly, WelcomeFriends);
		} else if(reaction == Hostile) {
			make_attacks(true);
			break;
		} else
			break;
	}
	opponent = push_opponent;
}

void move_party(pointc v) {
	if(!is_passable(v))
		return;
	if(loc->ismonster(v)) {
		turnto(v, to(party.d, Down), true);
		reaction_check(0);
		pass_time();
		return;
	}
	if(party_move_interact(v))
		return;
	party_set(v);
	pass_time();
	explore_area();
}

static bool change_overlay(pointc v, directionn d) {
	auto p = loc->get(v, d);
	if(!p)
		return false;
	auto x = to(v, d);
	if(!x)
		return false;
	auto n = cells[p->type].activate;
	if(!n)
		return false;
	loc->set(x, n);
	loc->removeov(x);
	return true;
}

static void toggle(pointc v) {
	if(!v)
		return;
	if(!loc->is(v, CellActive))
		loc->set(v, CellActive);
	else
		loc->remove(v, CellActive);
}

static bool is(classn classes, classn type) {
	return get_class_index(classes, type) != -1;
}

static messagen get_skilled(resn dungeon, celln type, racen race, classn classes) {
	if(dungeon == BRICK && type == CellDecor1 && is(classes, Theif))
		return LookTheifSign;
	return (messagen)0;
}

static void examine(creature* player, resn dungeon, celln type) {
	static messagen unskilled_examine[XANATHA - BRICK + 1][CellDecor3 - CellDecor1 + 1] = {
		{LookUnknownTheifSign, LookDrainageGate, LookUnknown}, // BRICK
	};
	auto m = get_skilled(dungeon, type, player->race, player->type);
	if(!m)
		m = unskilled_examine[dungeon - BRICK][type - CellDecor1];
	if(!m)
		return;
	player->say(m);
}

static wellmsgn get_miss(wellmsgn type) {
	switch(type) {
	case MessageMagicWeapons: return MessageMagicWeaponsFail;
	case MessageMagicRings: return MessageMagicRingsFail;
	case MessageSecrets: return MessageSecretsFail;
	case MessageTraps: return MessageTrapsFail;
	case MessageLocked: return MessageLockedFail;
	case MessageAtifacts: return MessageAtifactsFail;
	case MessageCursedItems: return MessageCursedItemsFail;
	case MessageSpecialItem: return MessageSpecialItemFail;
	case MessageBoss: return MessageBossFail;
	default: return type;
	}
}

static void read_wall_messages(creature* player, dungeoni::overlayi* p) {
	if(!player->isunderstand(loc->language)) {
		player->say(CantRead, language_names[loc->language]);
		return;
	}
	if(p->subtype < MessageHabbits) {
		if(loc->state.variables[p->subtype] > 0)
			player->say(wallmsg_names[p->subtype], loc->state.variables[p->subtype]);
		else {
			auto n = get_miss((wellmsgn)p->subtype);
			player->say(wallmsg_names[n]);
		}
	} else
		player->say(wallmsg_names[p->subtype]);
}

static bool manipulate_overlay() {
	auto p = loc->get(player->pos, player->d);
	if(!p)
		return false;
	auto v = to(player->pos, player->d);
	auto pi = (item*)get_item((void*)current_focus);
	switch(p->type) {
	case CellDoorButton:
		toggle(v);
		break;
	case CellDecor1:
	case CellDecor2:
	case CellDecor3:
		examine(player, loc->type, p->type);
		break;
	case CellSecretButton:
		if(change_overlay(player->pos, player->d)) {
			party_addexp(400);
			player->say(SecrectButtonFound);
			loc->state.secrets_found++;
		}
		break;
	case CellMessage:
		read_wall_messages(player, p);
		break;
	case CellCellar:
		if(!pi)
			break;
		if(*pi) {
			// Put item to cellar
			if(!is_small(pi->type))
				player->say(ItemNotFit);
			else
				loc->add(p, *pi);
		} else {
			// Get item from cellar
			item* items[1];
			if(loc->getitems(items, lengthof(items), p)) {
				*pi = *items[0];
				items[0]->clear();
			} else
				player->say(NothingToGrab);
		}
		break;
	case CellTrapLauncher:
		player->say(LookStrangeDevice);
		break;
	default:
		break;
	}
	return true;
}

static bool manipulate_cell() {
	auto v = to(player->pos, player->d);
	auto t = loc->get(v);
	switch(t) {
	case CellPortal:
		if(player->is(Mage)) {
			if(player->get(Mage) > 5) {
				// TODO: Use portal
			} else
				player->say(ThisIsUndefinedObject, getnm(Portal));
		} else
			player->say(ThisIsUndefinedObject, getnm(MagicDevice));
		break;
	case CellBarel:
	case CellWeb:
	case CellCocon:
		//player->speak(id, "About");
		//apply_script(id, "Use", 0);
		break;
	case CellGrave:
		//broke_cell(v);
		break;
	default:
		return false;
	}
	return true;
}

void player_manipulate() {
	if(!player)
		return;
	if(!player->isactable())
		return;
	if(manipulate_overlay() || manipulate_cell())
		make_action();
}

static bool dungeon_use() {
	if(!loc) {
		player->say(CantUseInSettlement);
		return false;
	}
	return true;
}

static dungeoni::overlayi* get_overlay() {
	if(!loc)
		return 0;
	return loc->get(party.pos, party.d);
}

static bool use_key(creature* player, item* last_item) {
	auto po = get_overlay();
	if(!po || po->type != CellKeyHole) {
		player->say(WhereIsKeyhole);
		return false;
	}
	if(!last_item->is(loc->getkey())) {
		player->say(ThisIsWrongKey);
		return false;
	}
	if(po->link) {
		consolen(getnm(DoorOpened));
		loc->set(po->link, CellActive);
		loc->state.locks_open++;
	}
	last_item->clear();
	return true;
}

static void sleep_party(int cure_hits) {
}

static bool allow_use(const creature* player, const item* pi) {
	return player->allow(pi->type, CantUseItem);
}

static bool monsters_nearbe() {
	return false;
}

static void ambush_enemy() {
	creature* opponents[party_size]; get_opponents(opponents);
	party_addexp_evil(25);
	party_set(opponents, Surprised);
	make_attacks(true);
}

static void attack_enemy() {
	make_attacks(true);
}

bool make_object_attack(pointc v);

void use_item(creature* player, item* last_item, wearn wear) {
	if(!player->isactable())
		return;
	switch(last_item->geti().wear) {
	case LeftHand: case RightHand:
		if(!dungeon_use())
			break;
		if(wear != LeftHand && wear != RightHand)
			player->say(MustBeUseInHand);
		else if(last_item->isweapon()) {
			if(!make_object_attack(to(party.pos, party.d)))
				make_attacks(false);
			pass_time();
		}
		break;
	case Body: case Neck: case Elbow: case Legs: case Head:
	case LeftRing: case RightRing:
		player->say(MustBeWearing);
		break;
	case Quiver:
		player->say(MustBeQuver);
		break;
	case Drinkable:
		if(!allow_use(player, last_item))
			break;
		//		drink_effect(pn, last_item->getpower(), xrand(5, 20) * 10, last_item->iscursed() ? -1 : 1);
		//		consolen(getnm("DrinkPotionAct"));
		last_item->clear();
		pass_time();
		break;
	case Edible:
		if(!allow_use(player, last_item))
			break;
		if(!dungeon_use())
			break;
		if(last_item->isdamaged()) {
			//			player->say("MakeCamp", "RottenFood");
			break;
		}
		if(monsters_nearbe())
			break;
		if(confirm(getnm(ConfirmMakeCamp))) {
			if(last_item->iscursed())
				sleep_party(-2);
			else
				sleep_party(last_item->geti().combat.damage.roll());
			last_item->clear();
		}
		break;
	case Readable:
		if(!allow_use(player, last_item))
			break;
		if(!player->canread())
			player->say(CantRead);
		else {
			//			if(read_effect(pn, last_item->getpower(), 50, xrand(5, 20) * 10))
			//				last_item->clear();
		}
		break;
	case Rod:
		if(!allow_use(player, last_item))
			break;
		if(wear != LeftHand) {
			//			player->speak("MustBeWearing", "LeftHand");
			break;
		}
		//		if(use_rod(pn, last_item, last_item->getpower()))
		//			pass_round();
		break;
	case Faithable:
		if(!allow_use(player, last_item))
			break;
		if(wear != LeftHand) {
			//			player->speak("MustBeWearing", "LeftHand");
			break;
		}
		//if(faith_effect(last_item->getmagic())) {
		//	last_item->usecharge("ToolCrumbleToDust");
		//	pass_round();
		//}
		break;
	case Key:
		if(!dungeon_use())
			break;
		use_key(player, last_item);
		break;
	default:
		if(!allow_use(player, last_item))
			break;
		// apply_script(last_item->geti().id, "Use", last_item->getmagic());
		break;
	}
}

static void party_set(celln v) {
	switch(v) {
	case CellStairsUp: party_set(loc->state.up); break;
	case CellStairsDown: party_set(loc->state.down); break;
	case CellPortal: party_set(loc->state.portal); break;
	default: return;
	}
}

void enter_dungeon(int level, celln location) {
	auto p = active_quest();
	if(!p)
		return;
	loc = find_dungeon(p->index(), level);
	if(!loc)
		return; // Dungeon not found. Stay in city.
	current_music = quests[loc->quest].music;
	party_set(location);
	make_action();
	set_dungeon_tiles(loc->type);
	next_scene(play_dungeon);
}

static bool is_animal() {
	return opponent->is(Animal);
}

static bool is_personality() {
	return !is_animal();
}

static bool is_bribeable() {
	if(is_animal())
		return false;
	return valuable_item != 0;
}

static void lie() {
	if(!party_median(adventurers, Charisma)) {
		monsters_talk(YouLiers);
		party_set(adventurers, Surprised);
		make_attacks(true);
	} else {
		creature* opponents[party_size]; get_opponents(opponents);
		party_set(opponents, Friendly);
	}
}

static bool talk_rumor() {
	auto p = active_quest();
	if(!p || p->identify())
		return false;
	monsters_talk(quest_rumor[p->rumor++]);
	monsters_leave();
	return true;
}

static bool is_magical(const void* object) {
	auto p = (item*)object;
	return !p->identified && p->power;
}

static bool talk_identify_item() {
	collection<item> source;
	party_items(source, is_magical, true);
	if(!source)
		return false;
	return true;
}

static bool talk_cursed_item() {
	return false;
}

static bool talk_gift_item() {
	return false;
}

static void talk_help() {
	// 1 - There is a big chance to speak about rumor
	if(chance(60) && talk_rumor())
		return;
	// 2 - Random chance to get help (include rumor)
	fncondition source[] = {
		talk_gift_item,
		talk_cursed_item, talk_identify_item, talk_rumor,
	};
	zshuffle(source, lengthof(source));
	for(auto proc : source) {
		if(proc())
			return;
	}
	// 3 - After all give general advise
	
}

actioni talk_carefully[] = {
	{Lie, {}, {}, lie, is_personality},
	{CalmDown, {}, Ranger, monsters_leave, is_animal},
	{Bribe, {}, {}, 0, is_bribeable},
	{Attack, {}, {}, attack_enemy},
	{}};

actioni talk_friendly[] = {
	{Talk, {}, {}, talk_help, is_personality},
	{Hunt, {}, Ranger, monsters_kill, is_animal},
	{Ambush, {}, {}, ambush_enemy},
	{}};