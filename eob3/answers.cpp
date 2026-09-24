/*
	Copyright 2026 by Pavel Chistyakov

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.77
*/

#include "answers.h"
#include "draw.h"
#include "pushvalue.h"
#include "rand.h"

picturen answer_picture;
const char* answer_header;

const char* answers::string;
bool answers::show_tips = true;
bool answers::interactive = true;

answers an;

static bool allow(const answers& an, size_t max_width) {
	for(auto& e : an) {
		if(zlen(e.text) > max_width)
			return false;
	}
	return true;
}

int answer_columns_def() {
	auto count = an.getcount();
	if(!count)
		return 1;
	auto result = 1;
	if(count > 15 && allow(an, 13))
		result = 3;
	else if(count > 8)
		result = 2;
	return result;
}

unsigned anhotkey(int index) {
	static char hotkeys[] = {
		'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
		'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
		'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
		'V', 'W', 'X', 'Y', 'Z'
	};
	if((size_t)index > sizeof(hotkeys) / sizeof(hotkeys[0]) - 1)
		index = sizeof(hotkeys) / sizeof(hotkeys[0]) - 1;
	return hotkeys[index];
}

const char* find_separator(const char* pb) {
	auto p = pb;
	while(*p) {
		if(*p == '-' && p[1] == '+' && p[2] == '-' && (p[3] == 10 || p[3] == 13) && p > pb && (p[-1] == 10 || p[-1] == 13))
			return p;
		p++;
	}
	return 0;
}

void answers_paint(fnabutton paintcell, int columns, const char* cancel_text) {
	auto column_width = width;
	if(columns > 1)
		column_width = (column_width - (metrics::border * 2 + metrics::padding) * (columns - 1)) / columns + 1;
	auto index = 0;
	auto y1 = (int)caret.y, x1 = (int)caret.x;
	auto y2 = (int)caret.y;
	auto push_width_normal = width;
	auto push_x2 = caret.x + width;
	width = column_width;
	pushfore push;
	for(auto& e : an.elements) {
		fore = push.fore;
		if(e.value < 0)
			fore = fore.mix(colors::header, 128);
		paintcell(index, e.value, e.text);
		caret.y += height + metrics::padding;
		fire(e.proc, (long)e.value, e.object);
		index++;
		if(caret.y > y2)
			y2 = caret.y;
		if(columns > 1) {
			auto current_column = index % columns;
			width = column_width;
			if(current_column == 0) {
				y1 = caret.y;
				caret.x = x1;
			} else {
				caret.y = y1;
				caret.x += width + metrics::border * 2 + metrics::padding;
				if(current_column == columns - 1)
					width = push_x2 - caret.x;
			}
		}
	}
	caret.x = x1; caret.y = y2;
	width = push_width_normal;
	if(cancel_text) {
		fore = fore.mix(colors::header, 128);
		paintcell(-1, 0, cancel_text);
		fire(buttonparam, 0);
		caret.y += height + metrics::padding;
	}
}

int answers::compare(const void* v1, const void* v2) {
	return szcmp(((answers::element*)v1)->text, ((answers::element*)v2)->text);
}

void answers::addv(fnevent proc, long value, void* object, const char* text, unsigned key, const char* format) {
	auto p = elements.add();
	p->proc = proc;
	p->object = object;
	p->value = value;
	p->text = sc.get();
	p->key = key;
	sc.addv(text, format);
	sc.addsz();
}

void answers::add(long value, const char* name, ...) {
	XVA_FORMAT(name);
	addv(buttonparam, value, 0, name, 0, format_param);
}

void answers::addp(fnevent proc, long value, void* object, const char* name, ...) {
	XVA_FORMAT(name);
	addv(proc, value, object, name, 0, format_param);
}

void answers::sort() {
	qsort(elements.data, elements.count, sizeof(elements.data[0]), compare);
}

long answers::random() const {
	if(!elements.count)
		return 0;
	return elements.data[rand() % elements.count].value;
}

const char* answers::getname(long v) {
	for(auto& e : elements) {
		if(e.value == v)
			return e.text;
	}
	return 0;
}

void answers::clear() {
	elements.clear();
	sc.clear();
}

const answers::element* answers::find(long value) const {
	for(auto& e : elements) {
		if(e.value==value)
			return &e;
	}
	return 0;
}

void answers::checkkeys() {
	auto index = 0;
	for(auto& e : elements) {
		if(!e.key)
			e.key = anhotkey(index);
		index++;
	}
}