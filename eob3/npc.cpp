#include "console.h"
#include "creature.h"
#include "game.h"
#include "rand.h"
#include "stringbuilder.h"

static const char* getnm(speechn id) {
	auto n = rand() % 3;
	switch(n) {
	case 1: return speech_names2[id] ? speech_names2[id] : speech_names1[id];
	case 2: return speech_names3[id] ? speech_names3[id] : speech_names1[id];
	default: return speech_names1[id];
	}
}

void printn(messagen id, ...) {
	XVA_FORMAT(id);
	consolen(getnm(id), format_param);
}

void npci::say(messagen id, ...) const {
	XVA_FORMAT(id);
	sayv(getnm(id), format_param);
}

void npci::say(speechn id, ...) const {
	XVA_FORMAT(id);
	sayv(getnm(id), format_param);
}

void npci::say(const char* foramt, ...) const {
	XVA_FORMAT(foramt);
	sayv(foramt, format_param);
}

void npci::sayv(const char* format, const char* format_param) const {
	consolens();
	console(name());
	console(" \"");
	consolev(format, format_param);
	console("\"");
}
