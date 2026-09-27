#include "answers.h"
#include "game.h"
#include "sound.h"

static picturen get_picture(messagen v) {
	switch(v) {
	case VisitTavern: return PicTavern;
	case VisitInn: return PicInn;
	default: return PicCity;
	}
}

static soundn get_music(messagen v) {
	switch(v) {
	case VisitInn: return MusInn;
	default: return MusKvirasim;
	}
}

void play_city_actions() {
	auto city_location = Cancel;
	while(true) {
		answer_picture = get_picture(city_location);
		current_music = get_music(city_location);
		play_city();
		an.add(GoAdventure, getnm(GoAdventure));
		an.add(VisitTavern, getnm(VisitTavern));
		an.add(VisitBlacksmith, getnm(VisitBlacksmith));
		an.add(VisitTemple, getnm(VisitTemple));
		an.add(VisitInn, getnm(VisitInn));
		an.add(VisitWizardTower, getnm(VisitWizardTower));
		auto result = (messagen)choose_large_menu_no_player(getnm(WhichWayToGo), getnm(Cancel));
		if(!result)
			continue;
		city_location = result;
	}
}