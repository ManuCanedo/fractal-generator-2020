#ifndef BMP_HEADER_H
#define BMP_HEADER_H

#pragma pack(push, 2)
struct BmpInfoHeader {
	constexpr BmpInfoHeader(const int32_t w, const int32_t h) : width(w), height(h)
	{
	}

	const int32_t header_size{ 40 };
	int32_t width;
	int32_t height;
	const int16_t planes{ 1 };
	const int16_t bits_pp{ 24 };
	const int32_t compression{ 0 };
	const int32_t data_len{ 0 };
	const int32_t horizontal_res{ 2400 };
	const int32_t vertical_res{ 2400 };
	const int32_t colors{ 0 };
	const int32_t important_colors{ 0 };
};

struct BmpFileHeader {
	constexpr BmpFileHeader(const int32_t size) : size(size)
	{
	}

	const char header[2]{ 'B', 'M' };
	int32_t size;
	const int32_t reserverd{ 0 };
	const int32_t data_offset{ sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) };
};
#pragma pack(pop)

#endif // BMP_HEADER_H
