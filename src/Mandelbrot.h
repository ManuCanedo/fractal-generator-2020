#pragma once

#include "Bitmap.h"
#include "ZoomList.h"

namespace Fractal
{
	struct Mandelbrot
	{
		static int GetIterations(double x, double y);
		static void GenerateFractalNaive(const int WIDTH, const int HEIGHT, ZoomList& zoomList, std::unique_ptr<int[]>& pHistogram, std::unique_ptr<int[]>& pFractal);
		static void ColorFractal(const int WIDTH, const int HEIGHT, Bitmap& image, std::unique_ptr<int[]>& pHistogram, std::unique_ptr<int[]>& pFractal);

		static inline int MAX_ITERATIONS = 10000;
	};
}

