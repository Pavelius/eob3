#include "action.h"
#include "answers.h"
#include "console.h"
#include "creature.h"
#include "dungeon.h"
#include "game.h"
#include "pushvalue.h"
#include "quest.h"
#include "rand.h"
#include "stringbuilder.h"
#include "view_focus.h"

variablei game;
static long save_focus;

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

bool indoor(actionn v) {
	return v >= Tavern && v <= Palace;
}

void addv(variablen v, int i) {
	game.add(v, i);
}

int getv(variablen v) {
	return game.variables[v];
}

int quest_count(questfn v) {
	auto result = 0;
	for(auto& e : quests) {
		if(e.state.is(v))
			result++;
	}
	return result;
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

void pass_time(unsigned minutes) {
	game.variables[Time] += minutes;
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

const actioni* choose_location(const actioni* source) {
	if(!source)
		return 0;
	for(auto p = source; *p; p++) {
		if(p->required && !enough(game, p->required))
			continue; // Can't pay or other reputation
		if(p->allow && !p->allow())
			continue;
		if(p->action >= Tavern && p->action <= Palace)
			an.add((long)p, getnm(VisitBuilding), getnm(p->action));
		else
			an.add((long)p, getnm(p->action));
	}
	return (actioni*)choose_large_menu_no_player(getnm(WhichWayToGo), getnm(Cancel));
}

const actioni* choose_building_action(const actioni* source) {
	if(!source)
		return 0;
	for(auto p = source; *p; p++) {
		if(p->required && !enough(game, p->required))
			continue; // Can't pay or other reputation
		if(!pass_restriction(p->restriction))
			continue; // Not pass restriction
		if(p->allow && !p->allow())
			continue;
		an.add((long)p, getnm(p->action));
	}
	return (actioni*)choose_player_action(getnm(Cancel));
}

static void pass_round() {
	pass_time(1);
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

static dungeoni* find_dungeon(int level) {
	return find_dungeon(loc->quest, level);
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

static void update_party_position() {
	for(auto i = 0; i < lengthof(adventurers); i++) {
		if(!adventurers[i])
			continue;
		adventurers[i]->side = i;
		adventurers[i]->pos = party.pos;
		adventurers[i]->d = party.d;
	}
}

static void set_party(pointc v) {
	party.pos = v;
	update_party_position();
}

static void set_party(posable v) {
	party = v;
	party.pos = to(party.pos, party.d);
	update_party_position();
}

static void set_party(pointc v, directionn d) {
	party.pos = v;
	party.d = d;
	update_party_position();
}

void party_turn_right() {
	party.d = to(party.d, Right);
	update_party_position();
}

void party_turn_left() {
	party.d = to(party.d, Left);
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
		set_party(v, d);
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

static void monsters_talk(messagen id) {
	auto monster = opponent->monster;
	if(!monster)
		return;
	//auto pe = bsdata<listi>::find(ids(last_quest->id, rm));
	//if(!pe && opponent->isanimal())
	//	pe = bsdata<listi>::find(ids("Animal", rm));
	//if(!pe)
	//	pe = bsdata<listi>::find(ids("Negotiation", rm));
	//if(!pe)
	//	return;
	//for(auto v : pe->elements)
	//	add_menu(v, true);
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm(id));
	auto result = show_message(temp, true, 0, 0);
}

static bool monsters_talk() {
	if(!opponent)
		return false;
	auto pm = opponent->monster;
	if(!pm)
		return false;
	//auto pn = speech_get_na(pm->id, last_quest->id);
	//if(!pn)
	//	pn = speech_get_na(pm->id, rm);
	//if(!pn) {
	//	if(opponent->isanimal())
	//		pn = speech_get_na("Animal", rm);
	//	else
	//		pn = speech_get_na("Intellegence", rm);
	//}
	//if(!pn)
	//	return false;
	//picture.clear();
	answer_picture = NoPicture;
	fix_animate();
	animation_update();
	monsters_talk(HowYouGetHere);
	return false;
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
	creature* creatures[6]; loc->getmonsters(creatures, to(party.pos, party.d));
	for(auto p : creatures) {
		if(p) {
			p->pos = v;
			// p->set(Moved);
		}
	}
}

static void monsters_kill(int bonus) {
	if(!loc)
		return;
	creature* creatures[6]; loc->getmonsters(creatures, to(party.pos, party.d));
	for(auto p : creatures) {
		if(p && *p)
			p->kill();
	}
}

static void monsters_leave(int bonus) {
	if(!loc)
		return;
	creature* creatures[6]; loc->getmonsters(creatures, to(party.pos, party.d));
	for(auto p : creatures) {
		if(p && *p) {
			drop_unique_loot(p);
			loc->state.monsters_killed++;
			p->clear();
		}
	}
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
		set_party(v);
		loc = find_dungeon(loc->level + 1);
		animation_update();
		all_party(pit_fall_down, true);
		consolen(getnm(PartyFallPit));
		enter_dungeon(loc->level, CellUnknown);
		animation_update();
		pass_round();
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

void move_party(pointc v) {
	if(!is_passable(v))
		return;
	if(loc->ismonster(v)) {
		turnto(v, to(party.d, Down), true);
	//	reaction_check(0);
		pass_round();
		return;
	}
	if(party_move_interact(v))
		return;
	set_party(v);
	pass_round();
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

static void examine(creature* player, resn dungeon, celln type) {
	switch(dungeon) {
	case BRICK:
		switch(type) {
		case CellDecor1:
		case CellDecor2:
		case CellDecor3:
			player->say(SomeKindOfP1, "Portal");
			break;
		}
		break;
	}
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
	auto pi = (item*)current_focus;
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
		//if(*pi) {
		//	// Put item to cellar
		//	if(!pi->geti().is(Small))
		//		player->speak(getid<celli>(p->type), "NotFit");
		//	else
		//		loc->add(p, *pi);
		//} else {
		//	// Get item from cellar
		//	item* items[1];
		//	if(loc->getitems(items, lenghtof(items), p)) {
		//		*pi = *items[0];
		//		items[0]->clear();
		//	} else
		//		player->speak(getid<celli>(p->type), "Empthy");
		//}
		break;
	default:
		// player->speak(getid<celli>(p->type), "About");
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
//			if(!make_object_attack(to(party.pos, party.d)))
//				make_attacks(false);
			pass_round();
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
		pass_round();
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

static void set_party(celln v) {
	switch(v) {
	case CellStairsUp: set_party(loc->state.up); break;
	case CellStairsDown: set_party(loc->state.down); break;
	case CellPortal: set_party(loc->state.portal); break;
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
	set_party(location);
	make_action();
	set_dungeon_tiles(loc->type);
	next_scene(play_dungeon);
}