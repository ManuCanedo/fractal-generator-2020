// Precompiled header
#include "fpch.h"

#include "Bitmap.h"

namespace Fractal
{
	BitmapFileHeader Bitmap::m_sFileHeader{ 0 };
	BitmapInfoHeader Bitmap::m_sInfoHeader{ 0, 0 };

	Bitmap::Bitmap(int width, int height) : m_Width(width), m_Height(height),
		m_pPixels(std::make_unique<uint8_t[]>(3 * static_cast<int64_t>(width) * static_cast<int64_t>(height)))
	{
		m_sFileHeader.fileSize = m_sFileHeader.dataOffset + 3 * m_Width * m_Height;
		m_sInfoHeader.width = m_Width;
		m_sInfoHeader.height = m_Height;
	}

	void Bitmap::SetPixel(uint64_t x, uint64_t y, uint8_t red, uint8_t green, uint8_t blue)
	{
		auto pPixel{ m_pPixels.get() };
		pPixel += 3 * y * m_Width + 3 * x;
		pPixel[0] = blue; pPixel[1] = green; pPixel[2] = red;
	}

	bool Bitmap::Write(const std::string filename)
	{
		static const char* fileHeader{ reinterpret_cast<char*>(&m_sFileHeader) };
		static const char* infoHeader{ reinterpret_cast<char*>(&m_sInfoHeader) };

		std::ofstream file;
		file.open(filename, std::ios::out | std::ios::binary);

		if (!file) return false;

		file.write(fileHeader, sizeof(m_sFileHeader));
		file.write(infoHeader, sizeof(m_sInfoHeader));
		file.write(reinterpret_cast<char*>(m_pPixels.get()),
			3 * static_cast<int64_t>(m_Width) * static_cast<int64_t>(m_Height));
		file.close();

		if (!file) return false;

		return true;
	}
}

