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

#include "stringvar.h"

bool stringvar_identifier(stringbuilder& sb, const char* identifier) {
	for(auto p = stringvars; p->id; p++) {
		if(!equal(p->id, identifier))
			continue;
		p->proc(sb);
		return true;
	}
	return false;
}