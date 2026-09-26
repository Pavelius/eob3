#include "color.h"
#include "math.h"
#include "stream.h"
#include "slice.h"

#pragma pack(push)
#pragma pack(1)
namespace {
struct header {
	unsigned short	signature; // for bitmap 4D42
	unsigned		size; // size in bytes all file
	unsigned short	reserved[2]; // reserved, must be null
	unsigned		bits; // offset to bitmap bits from top of this structure
};
struct info {
	unsigned		size; // The number of bytes required by the structure.
	int				width; // The width of the bitmap, in pixels. If biCompression is BI_JPEG or BI_PNG, the biWidth member specifies the width of the decompressed JPEG or PNG image file, respectively.
	int				height; // The height of the bitmap, in pixels. If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner. If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner.
	unsigned short	planes; // The number of planes for the target device. This value must be set to 1.
	unsigned short	bpp; // Bits per pixels. Can be 8, 16, 24, 32
	unsigned		compression;
	unsigned		size_image; // The size, in bytes, of the image.This may be set to zero for BI_RGB bitmaps.
	int				pels_per_meter_x; // The horizontal resolution, in pixels-per-meter, of the target device for the bitmap. An application can use this value to select a bitmap from a resource group that best matches the characteristics of the current device.
	int				pels_per_meter_y; // The vertical resolution, in pixels-per-meter, of the target device for the bitmap.
	unsigned		color_used; // Used mainly with 8-bit per pixel image format.
	unsigned		color_important;
};
}
#pragma pack(pop)

static bool format_info(const unsigned char* input, int size, int& width, int& height, int& bpp) {
	if(size < sizeof(header))
		return false;
	if(input[1] != 0x4D || input[0] != 0x42)
		return false;
	auto pi = (info*)(input + sizeof(header));
	width = pi->width;
	height = pi->height;
	bpp = pi->bpp;
	return true;
}

void bitmap_write(const char* url, unsigned char* bits, int width, int height, int bpp, int scanline, color* pallette) {
	header bmf = {0};
	info bmi = {0};
	//
	bmf.size = sizeof(bmf);
	bmf.signature = 0x4D42;
	bmf.bits = sizeof(header) + sizeof(info);
	//
	bmi.size = sizeof(bmi);
	bmi.width = width;
	bmi.height = height;
	bmi.planes = 1;
	bmi.bpp = bpp;
	bmi.pels_per_meter_x = 96;
	bmi.pels_per_meter_y = 96;
	switch(bpp) {
	case 8:
		bmi.color_used = 256;
		if(!pallette)
			return;
		break;
	default:
		bmi.color_used = 0;
		break;
	}
	if(!scanline)
		scanline = color_scanline(bmi.width, bmi.bpp);
	int wscn = color_scanline(bmi.width, bmi.bpp);
	io::file file(url, StreamWrite);
	if(!file)
		return;
	file.write(&bmf, sizeof(bmf));
	file.write(&bmi, sizeof(bmi));
	if(pallette && bmi.color_used)
		file.write(pallette, 256 * sizeof(color));
	// bits
	int pixbytes = bmi.bpp / 8;
	for(int y = 0; y < height; y++) {
		int w = pixbytes * width;
		unsigned char* p0 = (unsigned char*)bits + (height - y - 1) * scanline;
		file.write(p0, w);
		if(w < wscn)
			file.write(p0, wscn - w);
	}
}

bool bitmap_read(const char* url, unsigned char* output, int output_width, int output_height, int output_bpp, color* pallette) {
	int size = 0, width = 0, height = 0, input_bpp = 0;
	if(!output_width || !output_height)
		return false;
	auto output_scanline = color_scanline(output_width, output_bpp);
	memset(output, 0, output_scanline * output_height);
	auto input = (unsigned char*)loadb(url, &size);
	if(!input)
		return false;
	if(format_info(input, size, width, height, input_bpp)) {
		auto ph = (header*)input;
		auto pi = (info*)(input + sizeof(header));
		unsigned char* ppal = (unsigned char*)pi + sizeof(info);
		unsigned char* pb = (unsigned char*)input + ph->bits;
		auto input_scanline = color_scanline(width, input_bpp);
		if(pi->color_used > 0) {
			auto color_used = imin(pi->color_used, 256);
			memcpy(pallette, ppal, color_used * sizeof(color));
		}
		color e = {};
		if(height > output_height)
			height = output_height;
		if(width > output_width)
			width = output_width;
		for(int y = 0; y < height; y++) {
			unsigned char* d = output + y * output_scanline;
			unsigned char* s = pb + ((pi->height < 0) ? y : (pi->height - y - 1)) * input_scanline;
			if(input_bpp == output_bpp && input_bpp == 8)
				memcpy(d, s, width); // Optimization
			else {
				for(int x = 0; x < width; x++) {
					e.read(s, x, input_bpp, ppal);
					e.write(d, x, output_bpp, 0);
				}
			}
		}
	}
	free(input);
	return true;
}