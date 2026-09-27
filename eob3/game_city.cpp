#include "answers.h"
#include "game.h"

static picturen get_picture(messagen v) {
	switch(v) {
	case VisitTavern: return PicAdaque;
	default: return PicCity;
	}
}

void play_city_actions() {
	auto city_location = Cancel;
	while(true) {
		answer_picture = get_picture(city_location);
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