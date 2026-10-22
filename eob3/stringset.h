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

#define DSTEXT(N) {#N, N, (sizeof(N)/sizeof(N[0]))}

class stringbuilder;

struct stringset {
	const char*		id;
	const char**	names;
	int				count;
	constexpr explicit operator bool() { return id != 0; }
};

extern stringset stringset_data[];

const stringset* stringset_find(const char* id);
const stringset* stringset_find(const char** names);

bool stringset_identifier(const char* identifier, stringbuilder& sb);

const char* test_stringset();