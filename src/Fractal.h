#pragma once

#include "Window.h"

namespace Fractal
{
	class Fractal
	{
	public:
		static bool GenerateFractalFrame(unsigned char* pBuffer, const ScrSection screenSection, const ScrSection fractalSection,
			const int iterations, const double rWeight, const double gWeight, const double bWeight);
	};
}

