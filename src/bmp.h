#ifndef BMP_H
#define BMP_H

#include "bmp_header.h"

namespace fractal
{
class Bmp {
public:
	Bmp(const int w, const int h);

	void set_pixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b);
	bool write(std::string path);

private:
	static BmpFileHeader s_file_header;
	static BmpInfoHeader s_info_header;

private:
	const unsigned int width, height;
	std::unique_ptr<uint8_t[]> pixels;
};
}

#endif // BMP_H
