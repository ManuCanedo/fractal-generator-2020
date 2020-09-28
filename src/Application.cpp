// Precompiled header
#include "fpch.h"

#include "tools/Timer.h"
#include "tools/AllocationTracker.h"

#include "Application.h"
#include "Bitmap.h"
#include "Mandelbrot.h"
#include "ZoomList.h"


namespace Fractal
{
	const int WIDTH{ 800 }, HEIGHT{ 600 };
	const std::string name{ "fractal.bmp" };;

	void Application::Run()
	{
		LOG_TRACE("Application running");
		Bitmap image(WIDTH, HEIGHT);
		LOG_TRACE("Bitmap created");

		std::unique_ptr<int[]> pHistogram{ std::make_unique<int[]>(Mandelbrot::MAX_ITERATIONS) };
		std::unique_ptr<int[]> pFractal{ std::make_unique<int[]>(WIDTH * HEIGHT) };

		ZoomList zoomList(WIDTH, HEIGHT);
		zoomList.Add(Zoom(WIDTH/2, HEIGHT/2, 4.0/WIDTH));
		zoomList.Add(Zoom(295, HEIGHT - 202, 0.1));
		zoomList.Add(Zoom(312, HEIGHT - 304, 0.1));

		LOG_WARN("Generating Fractal");
		Mandelbrot::GenerateFractalNaive(WIDTH, HEIGHT, zoomList, pHistogram, pFractal);
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


// Entry Point
int main()
{
	Fractal::Log::Init();
	LOG_TRACE("System logger initialized");

	auto app = Fractal::Application::CreateApplication();
	app->Application::Run();

	delete app;
	LOG_TRACE("Resources freed");

	LOG_WARN("Execution finished\n\nPress ENTER to quit...");
	std::cin.get();
	return 0;
}
