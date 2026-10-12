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

#include "stream.h"

// Fast and simple driver for streaming binary data.
// Allow arrays and simple collections.
struct archive {
	io::stream&	source;
	bool writemode;
	constexpr archive(io::stream& source, bool writemode) : source(source), writemode(writemode) {}
	bool signature(const char* id);
	bool version(short major, short minor);
	bool checksum(unsigned long value);
	void set(void* value, unsigned size);
	void set(void** p, void* source, unsigned size); // Serial `p` unsigned char pointer to object in collection of `source`.
	// Array with fixed count
	template<typename T, unsigned N> void set(T(&value)[N]) {
		for(int i = 0; i < N; i++)
			set(value[i]);
	};
	// All simple types and requisites
	template<class T> void set(T& value) {
		set(&value, sizeof(value));
	}
	// Fixed data collection
	template<typename T> void setv(T* source, unsigned char& count) {
		set(count);
		for(unsigned i = 0; i < count; i++)
			set(source[i]);
	}
};