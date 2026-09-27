#include "draw.h"
#include "stream.h"
#include "math.h"
#include "game.h"
#include "slice.h"
#include "stringbuilder.h"
#include "view_focus.h"

extern int answer_origin, answer_per_page, answer_index;

void correct_answers(int maximum);
void draw_font_save(const char* url, int size, unsigned char* data);

static point size, curpix;
static unsigned char font_glyphs[256 * 8];
static unsigned char font_copy[8];
static unsigned char* last_data;

static void read_font(const char* url) {
	struct fnt {
		short int		filesize; // the size of the file
		short int		charoffset[128]; // the offset of the pixel data from the beginning of the file, the index is the ascii value
		unsigned char	height; // the height of a character in pixel
		unsigned char	width; // the width of a character in pixel
		unsigned char	data[1]; // the pixel data, one byte per line 
	};
	auto ph = (fnt*)loadb(url);
	auto p = ph->data;
	size.x = ph->width;
	size.y = ph->height;
	memset(font_glyphs, 0, sizeof(font_glyphs));
	for(auto i = 0; i < 128; i++)
		memcpy(font_glyphs + 8 * i, (unsigned char*)ph + ph->charoffset[i], size.y);
}

static void glyph_normal(unsigned char* data) {
	for(int h = 0; h < size.y; h++) {
		unsigned char line = data[h];
		unsigned char bit = 0x80;
		for(int w = 0; w < size.x; w++) {
			if((line & bit) == bit)
				pixel(caret.x + w, caret.y + h);
			bit = bit >> 1;
		}
	}
}

static void draw_glyph_zoomed(unsigned char* start_line, int zoom, int fw, int fh) {
	pushrect push;
	pushfore push_fore;
	width = zoom - 1;
	height = zoom - 1;
	auto back = fore.mix(colors::black, 32);
	for(int h = 0; h < fh; h++) {
		unsigned char line = start_line[h];
		unsigned char bit = 0x80;
		for(int w = 0; w < fw; w++) {
			auto x0 = push.caret.x + w * zoom;
			auto y0 = push.caret.y + h * zoom;
			caret.x = x0; caret.y = y0;
			fore = back; rectf();
			fore = push_fore.fore;
			if((line & bit) == bit)
				fore = push_fore.fore;
			else
				fore = back;
			rectf();
			bit = bit >> 1;
		}
	}
}

static bool vert_empty(unsigned char* p, int n) {
	auto f = (0x80 >> n);
	if(n >= 8)
		return true;
	for(auto y = 0; y < size.y; y++) {
		auto line = p[y];
		if((f & line) != 0)
			return false;
	}
	return true;
}

static void move_up(unsigned char* p) {
	auto v1 = p[0];
	memmove(p, p + 1, 7);
	p[7] = v1;
}

static void move_down(unsigned char* p) {
	auto v1 = p[7];
	memmove(p + 1, p, 7);
	p[0] = v1;
}

static void move_right(unsigned char* p) {
	for(auto i = 0; i < 8; i++) {
		unsigned char v2 = p[i];
		if(v2 & 1)
			p[i] = (p[i] >> 1) | 0x80;
		else
			p[i] = (p[i] >> 1);
	}
}

static void move_left(unsigned char* p) {
	for(auto i = 0; i < 8; i++) {
		unsigned char v2 = p[i];
		if(v2 & 0x80)
			p[i] = (p[i] << 1) | 1;
		else
			p[i] = (p[i] << 1);
	}
}

static void mirror_horiz(unsigned char* p) {
	for(auto i = 0; i < 8; i++) {
		unsigned char v1 = 0;
		unsigned char v2 = p[i];
		for(auto n = 0; n < 8; n++) {
			if(v2 & (1 << n))
				v1 |= (0x80 >> n);
		}
		p[i] = v1;
	}
}

static void mirror_vert(unsigned char* p) {
	for(auto i = 0; i < 8; i++) {
		auto j = 7 - i;
		if(j <= i)
			break;
		iswap(p[i], p[j]);
	}
}

static void copy_glyph(int n1, int n2) {
	const auto s = 8;
	last_data = font_glyphs + n1 * s;
	memcpy(last_data, font_glyphs + n2 * s, s);
}

static void copy_glyph(int n1) {
	memcpy(font_copy, font_glyphs + n1 * 8, sizeof(font_copy));
}

static void paste_glyph(int n1) {
	memcpy(font_glyphs + n1 * 8, font_copy, sizeof(font_copy));
}

static void create_cyrillyc_font() {
	copy_glyph(0x8B, 0x5E); mirror_vert(last_data); move_up(last_data); // Down arrow
	copy_glyph(0x9B, 0x5F); mirror_horiz(last_data); move_left(last_data); // Right arrow
	copy_glyph(0xA8, 'E');
	copy_glyph(0xAA, 'C');
	copy_glyph(0xAF, 'I');
	copy_glyph(0xB2, 'I');
	copy_glyph(0xB3, 'i');
	copy_glyph(0xB8, 'e');
	copy_glyph(0xBA, 'c');
	copy_glyph(0xBF, 'i');
	// Capital letter
	copy_glyph(0xC0, 'A');
	copy_glyph(0xC1, 'B');
	copy_glyph(0xC2, 'B');
	copy_glyph(0xC3, 'R');
	copy_glyph(0xC4, 'A');
	copy_glyph(0xC5, 'E');
	copy_glyph(0xC6, 'H');
	copy_glyph(0xC7, '3');
	copy_glyph(0xC8, 'H');
	copy_glyph(0xC9, 'H');
	copy_glyph(0xCA, 'K');
	copy_glyph(0xCB, 'K');
	copy_glyph(0xCC, 'M');
	copy_glyph(0xCD, 'H');
	copy_glyph(0xCE, 'O');
	copy_glyph(0xCF, 'H');
	copy_glyph(0xD0, 'P');
	copy_glyph(0xD1, 'C');
	copy_glyph(0xD2, 'T');
	copy_glyph(0xD3, 'Y');
	copy_glyph(0xD4, 'F');
	copy_glyph(0xD5, 'X');
	copy_glyph(0xD6, 'U');
	copy_glyph(0xD7, 'R');
	copy_glyph(0xD8, 'U');
	copy_glyph(0xD9, 'U');
	copy_glyph(0xDA, 'B');
	copy_glyph(0xDB, 'B');
	copy_glyph(0xDC, 'B');
	copy_glyph(0xDD, 'C');
	copy_glyph(0xDE, 'O');
	copy_glyph(0xDF, 'R');
	// Small letter
	copy_glyph(0xE0, 'a');
	copy_glyph(0xE1, 'b');
	copy_glyph(0xE2, 'b');
	copy_glyph(0xE3, 'r');
	copy_glyph(0xE4, 'a');
	copy_glyph(0xE5, 'e');
	copy_glyph(0xE6, 'h');
	copy_glyph(0xE7, '3');
	copy_glyph(0xE8, 'u');
	copy_glyph(0xE9, 'u');
	copy_glyph(0xEA, 'k');
	copy_glyph(0xEB, 'k');
	copy_glyph(0xEC, 'm');
	copy_glyph(0xED, 'h');
	copy_glyph(0xEE, 'o');
	copy_glyph(0xEF, 'p');
	copy_glyph(0xF0, 'p');
	copy_glyph(0xF1, 'c');
	copy_glyph(0xF2, 't');
	copy_glyph(0xF3, 'y');
	copy_glyph(0xF4, 'f');
	copy_glyph(0xF5, 'x');
	copy_glyph(0xF6, 'u');
	copy_glyph(0xF7, 'r');
	copy_glyph(0xF8, 'u');
	copy_glyph(0xF9, 'u');
	copy_glyph(0xFA, 'b');
	copy_glyph(0xFB, 'b');
	copy_glyph(0xFC, 'b');
	copy_glyph(0xFD, 'c');
	copy_glyph(0xFE, 'o');
	copy_glyph(0xFF, 'r');
}

static void correct_left(unsigned char* p) {
	for(auto i = 0; i < 8; i++) {
		if(!vert_empty(p, 0))
			break;
		move_left(p);
	}
}

static void read_font_v2(const char* url) {
	auto ph = (sprite*)loadb(url);
	size.x = ph->width;
	size.y = ph->height;
	memset(font_glyphs, 0, sizeof(font_glyphs));
	for(auto i = 0; i < 256; i++) {
		auto p = font_glyphs + 8 * i;
		memcpy(p, ph->ptr(sizeof(sprite) + size.y * i), 8 * size.y);
		correct_left(p);
	}
}

static void initialize_font(resn fid) {
	font = res_data[fid];
	size.x = font->width;
	size.y = font->height;
	auto base = font->ptr(sizeof(sprite) + 256);
	memset(font_glyphs, 0, sizeof(font_glyphs));
	for(auto i = 0; i < 256; i++) {
		auto p = font_glyphs + 8 * i;
		memcpy(p, base + size.y * i, size.y);
	}
}

static void edit_glyph(int current_glyph) {
	const int zoom = 12;
	pushrect push;
	pushfocus push_focus;
	auto data = font_glyphs + 8 * current_glyph;
	while(ismodal()) {
		if(curpix.x < 0)
			curpix.x = 0;
		else if(curpix.x >= size.x)
			curpix.x = size.x - 1;
		if(curpix.y < 0)
			curpix.y = 0;
		else if(curpix.y >= size.y)
			curpix.y = size.y - 1;
		fore = colors::black;
		rectf();
		fore = colors::white;
		caret.x = 4; caret.y = 4;
		draw_glyph_zoomed(data, zoom, size.x, size.y);
		caret.x += curpix.x * zoom;
		caret.y += curpix.y * zoom;
		width = zoom - 1;
		height = zoom - 1;
		fore = colors::green;
		rectb();
		domodal();
		switch(hkey) {
		case '1': data[curpix.y] &= ~(0x80 >> curpix.x); break;
		case '2': data[curpix.y] |= (0x80 >> curpix.x); break;
		case 'W': move_up(data); break;
		case 'Z': move_down(data); break;
		case 'A': move_left(data); break;
		case 'S': move_right(data); break;
		case 'H': mirror_horiz(data); break;
		case 'V': mirror_vert(data); break;
		case KeyRight: curpix.x++; break;
		case KeyLeft: curpix.x--; break;
		case KeyDown: curpix.y++; break;
		case KeyUp: curpix.y--; break;
		case KeyEscape: breakmodal(0); break;
		}
		focus_input();
	}
}

static void draw_font_save() {
	//auto p = findresid(font);
	//if(!p)
	//	return;
	//char temp[260]; stringbuilder sb(temp);
	//sb.add("%1/%2.fnt", p->folder, p->id);
	//draw_font_save(temp, size.y, font_glyphs);
}

void show_scene_font() {
	auto push_font = font;
	answer_index = 0;
	answer_per_page = 256;
	answer_origin = 0;
	auto sx = 10;
	initialize_font(FONT6);
	while(ismodal()) {
		if(answer_index < 0)
			answer_index = 0;
		else if(answer_index > 255)
			answer_index = 255;
		fore = colors::black;
		rectf();
		width = sx; height = sx;
		correct_answers(256);
		for(auto y = 0; y < 16; y++) {
			for(auto x = 0; x < 16; x++) {
				caret.x = x * width;
				caret.y = y * height;
				auto index = y * 16 + x;
				if(answer_index == index) {
					fore = colors::gray.mix(colors::black, 128);
					rectf();
				}
				fore = colors::white;
				caret.x += (width - size.x) / 2;
				caret.y += (height - size.y) / 2;
				glyph_normal(font_glyphs + 8 * index);
			}
		}
		caret.x = 176;
		caret.y = 16;
		draw_glyph_zoomed(font_glyphs + 8 * answer_index, 4, 7, 7);
		domodal();
		switch(hkey) {
		case KeyRight: answer_index++; break;
		case KeyLeft: answer_index--; break;
		case KeyDown: answer_index += 16; break;
		case KeyUp: answer_index -= 16; break;
		case KeyEnter: edit_glyph(answer_index); break;
		case KeyEscape: breakmodal(0); break;
		case 'K': create_cyrillyc_font(); break;
		case Ctrl + 'C': copy_glyph(answer_index); break;
		case Ctrl + 'V': paste_glyph(answer_index); break;
		case Ctrl + 'S': draw_font_save(); break;
		}
		focus_input();
	}
	font = push_font;
	draw_font_save("art/core/fontac.fnt", size.y, font_glyphs); // Autosave font
}

static int get_glyph_width(unsigned char* p) {
	auto m = 0;
	for(auto i = 0; i < 8; i++) {
		if(!vert_empty(p, i)) {
			if(m < i)
				m = i;
		}
	}
	if(m)
		return m + 1 + 1; // Symbols width and spacing
	return 8;
}

static void draw_font_save(const char* url, int size, unsigned char* data) {
	sprite header = {};
	header.name[0] = 'F';
	header.name[1] = 'N';
	header.name[2] = 'T';
	header.width = size;
	header.height = size;
	header.size = sizeof(sprite) - sizeof(pma) + 256 + (256 * size);
	io::file file(url, StreamWrite);
	if(!file)
		return;
	// Calculate width
	char font_width[256] = {};
	for(auto i = 0; i < 256; i++)
		font_width[i] = get_glyph_width(data + 8 * i);
	font_width[' '] = size - 3; // Space symbol
	font_width['\t'] = size - 3; // Space symbol
	// Save font
	file.write(&header, sizeof(header));
	file.write(&font_width, sizeof(font_width));
	for(auto i = 0; i < 256; i++)
		file.write(data + i * 8, size);
}

void convert_fonts_start() {
	read_font("D:/resources/eob2/FONT8.fnt");
	create_cyrillyc_font();
	draw_font_save("art/core/font8c.fnt", size.y, font_glyphs);
	read_font("D:/resources/eob2/FONT6.fnt");
	create_cyrillyc_font();
	draw_font_save("art/core/font6c.fnt", size.y, font_glyphs);
}

void convert_fonts() {
	read_font_v2("art/core/font8co.fnt");
	draw_font_save("art/core/font8c.fnt", size.y, font_glyphs);
	read_font_v2("art/core/font6co.fnt");
	draw_font_save("art/core/font6c.fnt", size.y, font_glyphs);
}

static void paint_sprites(resn id, point offset, int& focus, int per_line) {
	auto p = res_data[id];
	if(!p)
		return;
	auto index = 0;
	auto push_line = caret;
	auto count = per_line;
	while(index < p->count) {
		image(p, index, 0);
		if(focus == index) {
			auto push_caret = caret;
			caret = caret - offset;
			rectb();
			caret = push_caret;
		}
		index++;
		caret.x += width;
		if((--count) == 0) {
			count = per_line;
			caret.y += height;
			caret.x = push_line.x;
		}
		if((caret.y + height) > getheight())
			break;
	}
}

static void show_sprites(resn id, point start, point size) {
	pushrect push;
	pushfore push_fore;
	pushfont push_font(0);
	int focus = 0;
	auto maximum = res_data[id]->count;
	auto per_line = 320 / size.x;
	while(ismodal()) {
		if(focus < 0)
			focus = 0;
		else if(focus > maximum - 1)
			focus = maximum - 1;
		fore = colors::black;
		rectf();
		width = size.x;
		height = size.y;
		caret = start;
		fore = colors::white;
		paint_sprites(id, start, focus, per_line);
		setpos(0, 193); text(str("index %1i of %2i", focus, maximum), -1, TextBold);
		focus_input();
		domodal();
		switch(hkey) {
		case KeyRight: focus++; break;
		case KeyLeft: focus--; break;
		case KeyDown: focus += per_line; break;
		case KeyUp: focus -= per_line; break;
		case KeyEscape: breakmodal(0); break;
		}
	}
}

static void show_sprites(resn id) {
	switch(id) {
	case ITEMS: show_sprites(ITEMS, {8, 8}, {16, 16}); break;
	case ITEMGL: show_sprites(ITEMGL, {32, 24}, {64, 32}); break;
	case ITEMGS: show_sprites(ITEMGS, {16, 16}, {32, 32}); break;
	case PORTM: show_sprites(PORTM, {0, 0}, {32, 32}); break;
	default: break;
	}
}

void show_sprites_command() {
	show_sprites((resn)hparam);
}