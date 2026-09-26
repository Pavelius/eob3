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

enum directionn : unsigned char;

const int mpx = 38;
const int mpy = 23;

struct pointc {
	char x = -1, y = -1;
	constexpr bool operator==(pointc v) const { return x == v.x && y == v.y; }
	constexpr bool operator!=(pointc v) const { return x != v.x || y != v.y; }
	explicit operator bool() const { return x >= 0 && y >= 0 && x < mpx && y < mpy; }
	pointc operator+(const pointc& v) const { return {(char)(x + v.x), (char)(y + v.y)}; }
	pointc operator+(int i) const { pointc v; v.set(x + i, y + i); return v; }
	pointc operator-(int i) const { pointc v; v.set(x - i, y - i); return v; }
	void clear() { x = y = -1; }
	int	distance(pointc v) const;
	void set(int nx, int ny) { x = nx; y = ny; }
	pointc to(int dx, int dy) const { return {(char)(x + dx), (char)(y + dy)}; }
};

struct posable : pointc {
	char		side = 0;
	directionn	d = (directionn)0;
	constexpr posable() = default;
	constexpr posable(pointc v) : pointc(v), side(0), d() {}
	constexpr posable(pointc v, directionn d) : pointc(v), side(0), d(d) {}
	void clear() { pointc::clear(); side = 0; d = (directionn)0; }
	void set(pointc v, directionn d) { x = v.x; y = v.y; d = d; }
};
extern posable party;

typedef bool (*fnpointc)(pointc v);
typedef void (*fncorridor)(pointc v, directionn d);