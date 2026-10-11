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

#include "archive.h"
#include "stringbuilder.h"

bool archive::signature(const char* id) {
	char temp[4];
	if(writemode) {
		memset(temp, 0, sizeof(temp));
		stringbuilder sb(temp);
		sb.addv(id, 0);
		set(temp, sizeof(temp));
	} else {
		set(temp, sizeof(temp));
		if(szcmpi(temp, id) != 0)
			return false;
	}
	return true;
}

bool archive::checksum(unsigned long value) {
	if(writemode)
		set(value);
	else {
		decltype(value) new_value;
		set(new_value);
		if(new_value != value)
			return false;
	}
	return true;
}

bool archive::version(short major, short minor) {
	short major_reader = major;
	short minor_reader = minor;
	set(major_reader);
	set(minor_reader);
	if(!writemode) {
		if(major_reader < major)
			return false;
		else if(major_reader == major && minor_reader < minor)
			return false;
	}
	return true;
}

void archive::set(void* value, unsigned size) {
	if(value) {
		if(writemode)
			source.write(value, size);
		else
			source.read(value, size);
	} else // Skip same count of bytes if readed object is empthy
		source.seek(size, SeekCur);
}