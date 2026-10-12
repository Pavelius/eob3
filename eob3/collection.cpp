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

#include "collection.h"
#include "math.h"
#include "rand.h"
#include "stringbuilder.h"

static bool exist(unsigned char* pb, unsigned char* pe, unsigned char v) {
	while(pb < pe) {
		if(*pb == v)
			return true;
		pb++;
	}
	return false;
}

void collectionu::distinct() {
	auto ps = data;
	for(auto p : *this) {
		if(!exist(data, ps, p))
			*ps++ = p;
	}
	count = ps - data;
}

void collectionu::group(fncgroup proc) {
	for(auto& e : *this)
		e = proc(e);
	distinct();
}

void collectionu::select(unsigned char v1, unsigned char v2) {
	auto ps = data;
	auto pe = endof();
	for(auto v = v1; v <= v2; v++) {
		if(ps >= pe)
			break;
		*ps++ = v;
	}
	count = ps - data;
}

void collectionu::select(unsigned char v1, unsigned char v2, fncfilter proc, bool keep) {
	if(!proc) {
		select(v1, v2);
		return;
	}
	auto ps = data;
	auto pe = endof();
	for(auto v = v1; v <= v2; v++) {
		if(ps >= pe)
			break;
		if(proc(v) != keep)
			continue;
		*ps++ = v;
	}
	count = ps - data;
}

void collectionu::select(unsigned char v1, unsigned char v2, fncfilterparam proc, int param, bool keep) {
	auto ps = data;
	auto pe = endof();
	for(auto v = v1; v <= v2; v++) {
		if(ps >= pe)
			break;
		if(proc(v, param) != keep)
			continue;
		*ps++ = v;
	}
	count = ps - data;
}

void collectionu::match(fncfilter proc, bool keep) {
	auto ps = data;
	for(auto v : *this) {
		if(proc(v) != keep)
			continue;
		*ps++ = v;
	}
	count = ps - data;
}

unsigned char collectionu::random() const {
	if(!count)
		return 0;
	return data[rand() % count];
}

void collectionu::shuffle() {
	zshuffle(data, count);
}

unsigned char collectionu::pick() {
	auto result = (count > 0) ? data[0] : 0;
	remove(0);
	return result;
}

unsigned char collectionu::picklast() {
	if(!count)
		return 0;
	return data[--count];
}

void collectionu::top(int number) {
	if(count > (unsigned)number)
		count = number;
}

void collectionu::removeall(unsigned char v) {
	auto ps = data;
	auto pe = data + count;
	for(auto pb = data; pb < pe; pb++) {
		if(*pb != v)
			*ps++ = *pb;
	}
	count = ps - data;
}