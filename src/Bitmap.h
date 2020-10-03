#pragma once

namespace Fractal
{
	class Bitmap
	{
	public:
		Bitmap(int width, int height);
		~Bitmap() = default;

		void SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
		bool Write(std::string filename);

	private:
		int m_Width{ 0 }, m_Height{ 0 };
		std::unique_ptr<uint8_t[]> m_pPixels{ nullptr };
	};
}