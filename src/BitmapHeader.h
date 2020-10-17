#pragma once

struct BitmapInfoHeader;

#pragma pack(push, 2)

struct BitmapInfoHeader
{
	constexpr BitmapInfoHeader(const int32_t width, const int32_t height)
		: width(width), height(height) {}

	const int32_t headerSize{ 40 };
	int32_t width;
	int32_t height;
	const int16_t planes{ 1 };
	const int16_t bitsPerPixel{ 24 };
	const int32_t compression{ 0 };
	const int32_t dataSize{ 0 };
	const int32_t horizontalResolution{ 2400 };
	const int32_t verticalResolution{ 2400 };
	const int32_t colors{ 0 };
	const int32_t importantColors{ 0 };
};

struct BitmapFileHeader
{
	constexpr BitmapFileHeader(const int32_t fileSize) 
		: fileSize(fileSize) {}

	const char header[2]{ 'B', 'M' };
	int32_t fileSize;
	const int32_t reserverd{ 0 };
	const int32_t dataOffset{ sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) };
};

#pragma pack(pop)