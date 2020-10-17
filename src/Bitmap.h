#pragma once

#include "BitmapHeader.h"

namespace Fractal
{
	class Bitmap
	{
	public:
		Bitmap(const int width, const int height);
		~Bitmap() = default;

		void SetPixel(uint64_t x, uint64_t y, uint8_t red, uint8_t green, uint8_t blue);
		bool Write(std::string filename);

	private:
		static BitmapFileHeader m_sFileHeader;
		static BitmapInfoHeader m_sInfoHeader;

	private:
		const int m_Width, m_Height;
		std::unique_ptr<uint8_t[]> m_pPixels{ nullptr };
	};
}