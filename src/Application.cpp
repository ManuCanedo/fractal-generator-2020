#include <memory>
#include <exception>

#include "tools/Log.h"
#include "tools/Timer.h"
#include "tools/AllocationTracker.h"

#include "Application.h"
#include "Bitmap.h"
#include "Mandelbrot.h"

namespace Fractal
{
	const int WIDTH{ 800 }, HEIGHT{ 600 };
	const std::string name{ "fractal.bmp" };;

	void Application::Run()
	{
		LOG_TRACE("Application running");
		Bitmap image(WIDTH, HEIGHT);
		LOG_TRACE("Bitmap created");

		std::unique_ptr<int[]> pHistogram{ nullptr };
		std::unique_ptr<int[]> pFractal{ nullptr };
		try
		{
			pHistogram = std::make_unique<int[]>(Mandelbrot::MAX_ITERATIONS);
			pFractal = std::make_unique<int[]>(WIDTH * HEIGHT);
		}
		catch (std::exception e)
		{
			LOG_ERROR("Error allocating memory for the fractal");
		}

		LOG_WARN("Generating Fractal");
		Mandelbrot::GenerateFractalNaive(WIDTH, HEIGHT, pHistogram, pFractal);
		LOG_INFO("Fractal Generated");

		LOG_WARN("Colouring Fractal");
		Mandelbrot::ColorFractal(WIDTH, HEIGHT, image, pHistogram, pFractal);
		LOG_INFO("Fractal Coloured");

		image.Write(name);
		LOG_INFO("Bitmap generation successful");
		LOG_TRACE("Bitmap saved as {0}", name);
	}

	Application* Application::CreateApplication()
	{
		return new Application;
	}
}