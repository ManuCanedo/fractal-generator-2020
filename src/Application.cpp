// Precompiled header
#include "fpch.h"

#include "tools/Timer.h"
#include "tools/AllocationTracker.h"
#include "Application.h"

using namespace olc;

namespace Fractal
{
	Application::Application() : m_Futures(m_Threads)
	{
		sAppName = "Fractal Explorer";
	}

	Application* Application::CreateHeapApplication()
	{
		return new Application;
	}

	void Application::DestroyHeapApplication(Application* app)
	{
		delete app;
	}

	bool Application::OnUserCreate()
	{
		m_pFractal = std::make_unique<int[]>(static_cast<int64_t>(ScreenHeight()) * ScreenWidth());
		LOG_WARN("Memory: {0}", s_AllocationTracker.CurrentUsage());
		return true;
	}

	bool Application::OnUserDestroy()
	{
		m_pFractal.reset(nullptr);
		return true;
	}

	bool Application::OnUserUpdate(float fElapsedTime)
	{
		olc::vi2d pixTopLeft = { 0,0 };
		olc::vi2d pixBottomRight = { ScreenWidth(), ScreenHeight() };
		olc::vd2d fractTopLeft = { -2.0, -1.0 };
		olc::vd2d fractBottomRight = { 1.0, 1.0 };

		// Get Pan and Zoom input
		HandleInput(pixTopLeft, pixBottomRight, fractTopLeft, fractBottomRight);

		int screenSectionWidth{ (pixBottomRight.x - pixTopLeft.x) / m_Threads };
		double fractalSectionWidth{ (fractBottomRight.x - fractTopLeft.x) / static_cast<double>(m_Threads) };

		for (size_t i = 0; i < m_Threads; i++)
			m_Futures[i] = std::async(std::launch::async, &Application::GenerateFractalFrame, this,
				olc::vi2d(pixTopLeft.x + screenSectionWidth * i, pixTopLeft.y),
				olc::vi2d(pixTopLeft.x + screenSectionWidth * (i + 1), pixBottomRight.y),
				olc::vd2d(fractTopLeft.x + fractalSectionWidth * static_cast<double>(i), fractTopLeft.y),
				olc::vd2d(fractTopLeft.x + fractalSectionWidth * static_cast<double>(i + 1), fractBottomRight.y),
				m_Iterations);

		for (auto& f : m_Futures)
			f.wait();

		// Render frame
		RenderFrame();

		return !(GetKey(olc::Key::ESCAPE).bPressed);
	}

	void Application::ScreenShot()
	{
		//LOG_TRACE("Application running");
		//Bitmap image(WIDTH, HEIGHT);
		//LOG_TRACE("Bitmap created");

		//std::unique_ptr<int[]> pHistogram{ std::make_unique<int[]>(Mandelbrot::MAX_ITERATIONS) };
		//std::unique_ptr<int[]> pFractal{ std::make_unique<int[]>(WIDTH * HEIGHT) };

		//ZoomList zoomList(WIDTH, HEIGHT);
		//zoomList.Add(Zoom(WIDTH / 2, HEIGHT / 2, 4.0 / WIDTH));
		//zoomList.Add(Zoom(295, HEIGHT - 202, 0.1));
		//zoomList.Add(Zoom(312, HEIGHT - 304, 0.1));

		//LOG_WARN("Generating Fractal");
		//Mandelbrot::GenerateFractalNaive(WIDTH, HEIGHT, zoomList, pHistogram, pFractal);
		//LOG_INFO("Fractal Generated");

		//LOG_WARN("Colouring Fractal");
		//Mandelbrot::ColorFractal(WIDTH, HEIGHT, image, pHistogram, pFractal);
		//LOG_INFO("Fractal Coloured");

		//image.Write(name);
		//LOG_INFO("Bitmap generation successful");
		//LOG_TRACE("Bitmap saved as {0}", name);
	}

	void Application::HandleInput(olc::vi2d& pixTopLeft, olc::vi2d& pixBottomRight, olc::vd2d& fractTopLeft, olc::vd2d& fractBottomRight)
	{
		olc::vd2d mouseCoord = { static_cast<double>(GetMouseX()), static_cast<double>(GetMouseY()) };

		if (GetMouse(2).bPressed)
			m_StartPan = mouseCoord;
		if (GetMouse(2).bHeld)
		{
			m_Offset -= (mouseCoord - m_StartPan) / m_Scale;
			m_StartPan = mouseCoord;
		}

		olc::vd2d mouseCoordBeforeZoom;
		ScreenToWorld(mouseCoord, mouseCoordBeforeZoom);

		if (GetKey(olc::Key::UP).bHeld || GetMouseWheel() > 0) m_Scale *= 1.1;
		if (GetKey(olc::Key::DOWN).bHeld || GetMouseWheel() < 0) m_Scale *= 0.9;
		if (GetKey(olc::Key::RIGHT).bPressed) m_Iterations += 64;
		if (GetKey(olc::Key::LEFT).bPressed && m_Iterations > 64) m_Iterations -= 64;

		olc::vd2d mouseCoordAfterZoom;
		ScreenToWorld(mouseCoord, mouseCoordAfterZoom);
		m_Offset += (mouseCoordBeforeZoom - mouseCoordAfterZoom);

		ScreenToWorld(pixTopLeft, fractTopLeft);
		ScreenToWorld(pixBottomRight, fractBottomRight);
	}

	bool Application::GenerateFractalFrame(const olc::vi2d pixTopLeft, const olc::vi2d pixBottomRight, const olc::vd2d fractTopLeft, const olc::vd2d fractBottomRight, const int iterations)
	{
		double xScale{ (fractBottomRight.x - fractTopLeft.x) / (double(pixBottomRight.x) - double(pixTopLeft.x)) };
		double yScale{ (fractBottomRight.y - fractTopLeft.y) / (double(pixBottomRight.y) - double(pixTopLeft.y)) };
		
		double xPos{ fractTopLeft.x }, yPos{ fractTopLeft.y };
		int rowSize{ ScreenWidth() }, yOffset{ 0 };

		for (int y = pixTopLeft.y; y < pixBottomRight.y; y++)
		{
			xPos = fractTopLeft.x;
			for (int x = pixTopLeft.x; x < pixBottomRight.x; x++)
			{
				std::complex<double> c{ xPos, yPos };
				std::complex<double> z{ 0.0, 0.0 };
				int n{ 0 };

				while (z.real() * z.real() + z.imag() * z.imag() < 4 && n < iterations)
				{
					z = { z.real() * z.real() - z.imag() * z.imag() + c.real(),
						2 * z.real() * z.imag() + c.imag() };
					++n;
				}

				m_pFractal[yOffset + x] = n;
				xPos += xScale;
			}

			yPos += yScale;
			yOffset += rowSize;
		}

		return true;
	}

	void Application::RenderFrame()
	{
		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{
				int i{ m_pFractal[static_cast<int64_t>(y) * ScreenWidth() + x] };
				float n(i);
				float a{ 0.1f };

				if (i == m_Iterations)
					Draw(x, y, olc::BLACK);
				else
					Draw(x, y, olc::PixelF(0.5f * sin(a * n) + 0.5f, 0.2f * sin(a * n + 2.094f) + 0.5f, 0.5f * sin(a * n + 4.188f) + 0.5f));
			}
	}

	// Convert coordinates from World Space --> Screen Space
	void Application::WorldToScreen(const olc::vd2d& v, olc::vi2d& n)
	{
		n.x = static_cast<int>((v.x - m_Offset.x) * m_Scale.x);
		n.y = static_cast<int>((v.y - m_Offset.y) * m_Scale.y);
	}

	// Convert coordinates from Screen Space --> World Space
	void Application::ScreenToWorld(const olc::vi2d& n, olc::vd2d& v)
	{
		v.x = static_cast<double>(n.x) / m_Scale.x + m_Offset.x;
		v.y = static_cast<double>(n.y) / m_Scale.y + m_Offset.y;
	}
}

// Entry Point
int main()
{
	Fractal::Log::Init();
	LOG_TRACE("System logger initialized");

	auto app = Fractal::Application::CreateHeapApplication();
	LOG_INFO("Application Launched");
	LOG_WARN("Memory: {0}", s_AllocationTracker.CurrentUsage());
	if (app->Construct(1280, 720, 1, 1, false, false))
		app->Start();

	Fractal::Application::DestroyHeapApplication(app);
	LOG_TRACE("Resources freed");
	LOG_WARN("Memory: {0}", s_AllocationTracker.CurrentUsage());

	return 0;
}
