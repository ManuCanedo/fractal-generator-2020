#include "fpch.h"

#include "Fractal.h"

namespace Fractal
{
	bool Fractal::GenerateFractalFrame(unsigned char* pBuffer, const ScrSection scrSec, const ScrSection fracSec,
		const int iterations, const double rWeight, const double gWeight, const double bWeight)
	{
		double xScale{ (fracSec.bottomRight.x - fracSec.topLeft.x) / (scrSec.bottomRight.x - scrSec.topLeft.x) };
		double yScale{ (fracSec.bottomRight.y - fracSec.topLeft.y) / (scrSec.bottomRight.y - scrSec.topLeft.y) };

		double xPos{ fracSec.topLeft.x }, yPos{ fracSec.topLeft.y };
		int rowSize{ scrSec.scrWidth }, yOffset{ 0 };

		for (int y = scrSec.topLeft.y; y < scrSec.bottomRight.y; ++y)
		{
			xPos = scrSec.topLeft.x;
			for (int x = scrSec.topLeft.x; x < scrSec.bottomRight.x; ++x)
			{
				std::complex<double> c{ xPos, yPos };
				std::complex<double> z{ 0.0, 0.0 };

				double zRe2{ z.real() * z.real() };
				double zIm2{ z.imag() * z.imag() };
				int n{ 0 };

				while (zRe2 + zIm2 < 4 && n < iterations)
				{
					z = { zRe2 - zIm2 + c.real(), 2 * z.real() * z.imag() + c.imag() };
					zRe2 = z.real() * z.real();
					zIm2 = z.imag() * z.imag();
					++n;
				}

				unsigned char red{ 0 }, green{ 0 }, blue{ 0 };
				if (n < iterations)
				{
					red = static_cast<unsigned char>(256 * (rWeight * sin(0.1 * static_cast<double>(n)) + 0.5));
					green = static_cast<unsigned char>(256 * (gWeight * sin(0.1 * static_cast<double>(n) + 2.094) + 0.5));
					blue = static_cast<unsigned char>(256 * (bWeight * sin(0.1 * static_cast<double>(n) + 4.188) + 0.5));
				}

				pBuffer[3 * (static_cast<int>(yOffset + x)) + 0] = red;
				pBuffer[3 * (static_cast<int>(yOffset + x)) + 1] = green;
				pBuffer[3 * (static_cast<int>(yOffset + x)) + 2] = blue;

				xPos += xScale;
			}
			yPos += yScale;
			yOffset += rowSize;
		}

		return true;
	}
}