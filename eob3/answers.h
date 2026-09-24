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

#pragma once

#include "adat.h"
#include "stringbuilder.h"

typedef void(*fnevent)();
typedef bool(*fnuctest)(int v);
typedef void(*fnabutton)(int index, long value, const char* text);

enum picturen : unsigned char;

extern picturen answer_picture;
extern const char* answer_header;

struct answers {
	struct element {
		fnevent		proc;
		void*		object;
		long		value;
		const char* text;
		unsigned	key;
	};
	char buffer[2048];
	stringbuilder sc;
	adat<element, 36> elements;
	static bool	interactive;
	static const char* string;
	static bool show_tips;
	answers() : sc(buffer) {}
	constexpr operator bool() const { return elements.count != 0; }
	const element* begin() const { return elements.data; }
	const element* end() const { return elements.end(); }
	const element* find(long value) const;
	element* begin() { return elements.data; }
	static int compare(const void* v1, const void* v2);
	const char* getname(long v);
	int	getcount() const { return elements.getcount(); }
	int	indexof(const void* v) const { return elements.indexof(v); }
	void add(long value, const char* name, ...);
	void addp(fnevent proc, long value, void* object, const char* name, ...);
	void addv(fnevent proc, long value, void* object, const char* name, unsigned key, const char* format);
	void checkkeys();
	void clear();
	long random() const;
	void remove(int index) { elements.remove(index); }
	void sort();
};
extern answers an;

unsigned anhotkey(int index);

int answer_columns_def();

const char* find_separator(const char* p);

void answers_paint(fnabutton paintcell, int columns, const char* cancel_text);
void buttonparam();
long choose_answers(const char* title = 0, const char* cancel_text = 0, int columns = 1);
long choose_answers_random();
long choose_value(long t1, long t2, fnuctest condition, const char** names, const char* title, const char* cancel_text = 0, bool need_sort = true, int columns = 1);
