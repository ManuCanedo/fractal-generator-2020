#include "fpch.h"

#include "bmp.h"

namespace fractal
{
BmpFileHeader Bmp::s_file_header{ 0 };
BmpInfoHeader Bmp::s_info_header{ 0, 0 };

Bmp::Bmp(int w, int h)
	: width(w), height(h),
	  pixels(std::make_unique<uint8_t[]>(3 * static_cast<int64_t>(w) * static_cast<int64_t>(h)))
{
	s_file_header.size = s_file_header.data_offset + 3 * width * height;
	s_info_header.width = width;
	s_info_header.height = height;
}

void Bmp::set_pixel(const uint64_t x, const uint64_t y, const uint8_t r, const uint8_t g, const uint8_t b)
{
	auto pix = pixels.get();
	pix += 3 * y * width + 3 * x;
	pix[0] = b;
	pix[1] = g;
	pix[2] = r;
}

bool Bmp::write(std::string_view path)
{
	static const char *fh = reinterpret_cast<char *>(&s_file_header);
	static const char *ih = reinterpret_cast<char *>(&s_info_header);

	std::ofstream file(path.data(), std::ios::out | std::ios::binary);
	if (file)
		return false;

	file.write(fh, sizeof(s_file_header));
	file.write(ih, sizeof(s_info_header));
	file.write(reinterpret_cast<char *>(pixels.get()),
		   3 * static_cast<int64_t>(width) * static_cast<int64_t>(height));
	file.close();

	return true;
}
} // namespace fractal
