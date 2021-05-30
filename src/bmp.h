#ifndef BMP_H
#define BMP_H

#include "bmp_header.h"

namespace fractal
{
class Bmp {
public:
	Bmp(uint64_t w, uint64_t h);

	void set_pixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b);
	bool write(std::string_view path);

private:
	static BmpFileHeader s_file_header;
	static BmpInfoHeader s_info_header;

private:
	const uint64_t width, height;
	std::unique_ptr<uint8_t[]> pixels;
};
} // namespace fractal

#endif // BMP_H
