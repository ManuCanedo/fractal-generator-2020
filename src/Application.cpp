// Precompiled header
#include "fpch.h"

#include "tools/Timer.h"
#include "tools/AllocationTracker.h"

#include "Application.h"

using namespace olc;

namespace Fractal
{
	Application::Application()
	{
		sAppName = "Fractal Explorer";
	}

	Application* Application::CreateApplication()
	{
		return new Application;
	}

	bool Application::OnUserCreate()
	{
		m_pFractal = std::make_unique<int[]>(ScreenHeight() * ScreenWidth());
		return true;
	}

	bool Application::OnUserUpdate(float fElapsedTime)
	{
		// Get mouse location this frame
		olc::vd2d vMouse = { (double)GetMouseX(), (double)GetMouseY() };

		// Handle Pan & Zoom
		if (GetMouse(2).bPressed)
			m_vStartPan = vMouse;

		if (GetMouse(2).bHeld)
		{
			m_vOffset -= (vMouse - m_vStartPan) / m_vScale;
			m_vStartPan = vMouse;
		}

		olc::vd2d vMouseBeforeZoom;
		ScreenToWorld(vMouse, vMouseBeforeZoom);

		if (GetKey(olc::Key::W).bHeld || GetKey(olc::Key::UP).bHeld || GetMouseWheel() > 0) m_vScale *= 1.1;
		if (GetKey(olc::Key::S).bHeld || GetKey(olc::Key::DOWN).bHeld || GetMouseWheel() < 0) m_vScale *= 0.9;

		olc::vd2d vMouseAfterZoom;
		ScreenToWorld(vMouse, vMouseAfterZoom);
		m_vOffset += (vMouseBeforeZoom - vMouseAfterZoom);

		olc::vi2d pix_tl = { 0,0 };
		olc::vi2d pix_br = { ScreenWidth(), ScreenHeight() };
		olc::vd2d frac_tl = { -2.0, -1.0 };
		olc::vd2d frac_br = { 1.0, 1.0 };

		ScreenToWorld(pix_tl, frac_tl);
		ScreenToWorld(pix_br, frac_br);

		// Calculate Fractal
		GenerateFractalFrame(pix_tl, pix_br, frac_tl, frac_br);

		// Render frame
		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{
				int i = m_pFractal[y * ScreenWidth() + x];
				float n = (float)i;
				float a = 0.1f;

				if (i == m_Iterations)
					Draw(x, y, olc::BLACK);
				else
					Draw(x, y, olc::PixelF(0.5f * sin(a * n) + 0.5f, 0.2f * sin(a * n + 2.094f) + 0.5f, 0.5f * sin(a * n + 4.188f) + 0.5f));
			}

		return true;
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

	void Application::GenerateFractalFrame(const olc::vi2d& pix_tl, const olc::vi2d& pix_br, const olc::vd2d& frac_tl, const olc::vd2d& frac_br)
	{
		double xScale = (frac_br.x - frac_tl.x) / (double(pix_br.x) - double(pix_tl.x));
		double yScale = (frac_br.y - frac_tl.y) / (double(pix_br.y) - double(pix_tl.y));

		for (int y = pix_tl.y; y < pix_br.y; y++)
			for (int x = pix_tl.x; x < pix_br.x; x++)
			{
				std::complex<double> z{ 0, 0 };
				std::complex<double> c{ x * xScale + frac_tl.x, y * yScale + frac_tl.y };

				double zRe2{ 0.0 };
				double zIm2{ 0.0 };
				int n{ 0 };

				while ((zRe2 + zIm2 < 4) && n < m_Iterations)
				{
					z = { zRe2 - zIm2 + c.real(), 2 * z.real() * z.imag() + c.imag() };
					zRe2 = z.real() * z.real();
					zIm2 = z.imag() * z.imag();
					++n;
				}
				m_pFractal[y * ScreenWidth() + x] = n;
			}
	}

	// Convert coordinates from World Space --> Screen Space
	void Application::WorldToScreen(const olc::vd2d& v, olc::vi2d& n)
	{
		n.x = static_cast<int>((v.x - m_vOffset.x) * m_vScale.x);
		n.y = static_cast<int>((v.y - m_vOffset.y) * m_vScale.y);
	}

	// Convert coordinates from Screen Space --> World Space
	void Application::ScreenToWorld(const olc::vi2d& n, olc::vd2d& v)
	{
		v.x = static_cast<double>(n.x) / m_vScale.x + m_vOffset.x;
		v.y = static_cast<double>(n.y) / m_vScale.y + m_vOffset.y;
	}
}

// Entry Point
int main()
{
	Fractal::Log::Init();
	LOG_TRACE("System logger initialized");

	auto app = Fractal::Application::CreateApplication();
	if (app->Construct(1280, 720, 1, 1))
		app->Start();

	delete app;
	LOG_TRACE("Resources freed");

	LOG_WARN("Execution finished\n\nPress ENTER to quit...");
	std::cin.get();
	return 0;
}
