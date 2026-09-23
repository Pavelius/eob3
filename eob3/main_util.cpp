#include "sound.h"
#include "stream.h"
#include "slice.h"
#include "stringbuilder.h"

static unsigned temp_buffer[100000 * 3];

// Convert midi file to windows based midi sequence.
unsigned convert_to_midi_events(unsigned* output, unsigned output_size,
	const unsigned char* input_midi_file, unsigned input_size,
	unsigned& result_division);

static void pack_bin(io::stream& out, const char* header, const char* id, const unsigned* data, unsigned size) {
	if(header)
		out << header;
	out << id << "[" << size << "] = {";
	for(unsigned i = 0; i < size; i++) {
		if((i % 32) == 0)
			out << "\n";
		out << data[i];
		if((i + 1) != size)
			out << ",";
	}
	out << "};\n";
}

static void pack_bin(io::stream& out, const char* header, const char* id, const unsigned char* data, unsigned size) {
	if(header)
		out << header;
	out << id << "[" << size << "] = {";
	for(unsigned i = 0; i < size; i++) {
		if((i % 64) == 0)
			out << "\n";
		out << data[i];
		if((i + 1) != size)
			out << ",";
	}
	out << "};\n";
}

static void add_midi(io::stream& out, const char* url, const char* id, unsigned* result_size = 0, unsigned short* result_division = 0) {
	unsigned division;
	int input_size = 0;
	auto input = (unsigned char*)loadb(url, &input_size);
	if(!input)
		return;
	auto output_size = convert_to_midi_events(temp_buffer, lenghof(temp_buffer), input, input_size, division);
	if(output_size)
		pack_bin(out, "static unsigned mid_", id, temp_buffer, output_size);
	if(result_size)
		*result_size = output_size;
	if(result_division)
		*result_division = division;
	free(input);
}

static void convert_midi(const char* url, const char* id) {
	io::file file(str("mid_%1.cpp", id), StreamWrite);
	if(!file)
		return;
	add_midi(file, url, id);
}

static void convert_bin(const char* url, const char* id) {
	int size;
	auto p = (unsigned char*)loadb(url, &size);
	if(!p)
		return;
	io::file out(str("bin_%1.cpp", id), StreamWrite);
	if(out)
		pack_bin(out, "unsigned char bin_", id, p, size);
	free(p);
}

static void convert_music(const char* url, const char** files) {
	io::file out("mid_music.cpp", StreamWrite);
	if(!out)
		return;
	out << "#include \"sound.h\"\n\n";
	char temp[260]; stringbuilder sb(temp);
	soundi mt[256] = {};
	// Main files data
	for(auto p = files; *p; p++) {
		auto s = *p;
		if(!s || s[0] == 0)
			continue;
		sb.clear();
		sb.add(url);
		sb.add("%1.mid", s);
		auto i = p - files;
		add_midi(out, temp, s, &mt[i].size, &mt[i].division);
		out << "\n";
	}
	// Add headers
	out << "soundi songs[] = {\n";
	for(auto p = files; *p; p++) {
		auto s = *p;
		if(!s || s[0] == 0) // Empty name must be empty
			out << "{},\n";
		else {
			auto i = p - files;
			out << "{mid_" << *p << "," << mt[i].size << "," << mt[i].division << "},\n";
		}
	}
	out << "};";
}

static void convert_music() {
	static const char* source[] = {
		"",
		"kyra2a_002", "kyra1a_music_3", "kyra1a_music_2",
		"kyra5b_029",
		0};
	convert_music("D:/gamer/lk1/mid/", source);
}

static void convert_fonts() {
	convert_bin("D:/projects/eob2/art/core/font6c.fnt", "font6");
	convert_bin("D:/projects/eob2/art/core/font8c.fnt", "font8");
}

static void convert_screens() {
	convert_bin("D:/projects/eob2/art/core/menu.pma", "menu");
	convert_bin("D:/projects/eob2/art/core/playfld.pma", "playfld");
}

void main_util() {
	// convert_screens();
	// convert_fonts();
	// convert_music();
	// convert_midi("D:/gamer/lk1/mid/kyra2a_002.mid", "mus_forest");
}