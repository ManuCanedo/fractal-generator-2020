// Precompiled header
#include "fpch.h"

#include "Bitmap.h"
#include "BitmapHeader.h"

namespace Fractal
{
	Bitmap::Bitmap(int width, int height)
		: m_Width(width), m_Height(height),
		m_pPixels(std::make_unique<uint8_t[]>(3 * static_cast<int64_t>(width) * static_cast<int64_t>(height))) {}

	void Bitmap::SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
	{
		uint8_t* pPixel{ m_pPixels.get() };
		pPixel += 3 * static_cast<int64_t>(y) * m_Width + 3 * static_cast<int64_t>(x);
		pPixel[0] = blue; pPixel[1] = green; pPixel[2] = red;
	}

	bool Bitmap::Write(std::string filename)
	{
		BitmapFileHeader fileHeader;
		BitmapInfoHeader infoHeader;

		fileHeader.fileSize = sizeof(BitmapFileHeader)
			+ sizeof(BitmapInfoHeader) + (3 * static_cast<int64_t>(m_Width) * static_cast<int64_t>(m_Height));
		fileHeader.dataOffset = sizeof(BitmapFileHeader)
			+ sizeof(BitmapInfoHeader);

		infoHeader.width = m_Width;
		infoHeader.height = m_Height;

		std::ofstream file;
		file.open(filename, std::ios::out | std::ios::binary);

		if (!file) return false;

		file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
		file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
		file.write(reinterpret_cast<char*>(m_pPixels.get()), 3 * static_cast<int64_t>(m_Width) * static_cast<int64_t>(m_Height));
		file.close();

		if (!file) return false;

		return true;
	}
}

