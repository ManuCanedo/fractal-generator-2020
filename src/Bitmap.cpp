#include <fstream>
#include "Log.h"
#include "Bitmap.h"
#include "BitmapHeader.h"

namespace Fractal
{
	Bitmap::Bitmap(int width, int height)
		: m_Width(width), m_Height(height),
		m_pPixels(std::make_unique<uint8_t[]>(width* height * 3)) {}

	Bitmap::~Bitmap() {}

	void Bitmap::SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
	{

	}

	bool Bitmap::Write(std::string filename)
	{
		Fractal::BitmapFileHeader fileHeader;
		Fractal::BitmapInfoHeader infoHeader;

		fileHeader.fileSize = sizeof(Fractal::BitmapFileHeader)
			+ sizeof(Fractal::BitmapInfoHeader) + m_Width * m_Height * 3;
		fileHeader.dataOffset = sizeof(Fractal::BitmapFileHeader)
			+ sizeof(Fractal::BitmapInfoHeader);

		infoHeader.width = m_Width;
		infoHeader.height = m_Height;

		std::ofstream file;
		file.open(filename, std::ios::out | std::ios::binary);
		if (!file)
			return false;

		file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
		file.write(reinterpret_cast<char*>(&fileHeader), sizeof(infoHeader));
		file.write(reinterpret_cast<char*>(m_pPixels.get()), m_Width * m_Height * 3);

		file.close();
		if (!file)
			return false;

		return true;
	}
}

