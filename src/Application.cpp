// Precompiled header
#include "fpch.h"

#include "tools/AllocationTracker.h"

#include "Application.h"
#include "Bitmap.h"

#include "GLFW/glfw3.h"

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
		for (auto& f : m_Futures)
			f.wait();

		m_pFractal.reset();
		return true;
	}

	bool Application::OnUserUpdate(float fElapsedTime)
	{
		using namespace std::chrono_literals;

		olc::vi2d pixTopLeft = { 0,0 };
		olc::vi2d pixBottomRight = { ScreenWidth(), ScreenHeight() };
		olc::vd2d fractTopLeft = { -2.0, -1.0 };
		olc::vd2d fractBottomRight = { 1.0, 1.0 };

		HandleInput(pixTopLeft, pixBottomRight, fractTopLeft, fractBottomRight);

		int screenSectionWidth{ (pixBottomRight.x - pixTopLeft.x) / m_Threads };
		double fractalSectionWidth{ (fractBottomRight.x - fractTopLeft.x) / static_cast<double>(m_Threads) };

		auto tp1 = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < m_Threads; i++)
			m_Futures[i] = std::async(std::launch::async, &Application::GenerateFractalFrame, this,
				olc::vi2d(pixTopLeft.x + screenSectionWidth * i, pixTopLeft.y),
				olc::vi2d(pixTopLeft.x + screenSectionWidth * (i + 1), pixBottomRight.y),
				olc::vd2d(fractTopLeft.x + fractalSectionWidth * static_cast<double>(i), fractTopLeft.y),
				olc::vd2d(fractTopLeft.x + fractalSectionWidth * static_cast<double>(i + 1), fractBottomRight.y),
				m_Iterations);

		for (auto& f : m_Futures)
			f.wait();

		RenderFrame();

		auto tp2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsedTime = tp2 - tp1;

		//DrawString(0, 30, "Time: " + std::to_string(elapsedTime.count()) + "s", olc::WHITE, 1);
		//DrawString(0, 60, "Iterations: " + std::to_string(m_Iterations), olc::WHITE, 1);

		return !(GetKey(olc::Key::ESCAPE).bPressed);
	}

	bool Application::GenerateFractalFrame(const olc::vi2d pixTopLeft, const olc::vi2d pixBottomRight, const olc::vd2d fractTopLeft,
		const olc::vd2d fractBottomRight, int iterations)
	{
		double xScale{ (fractBottomRight.x - fractTopLeft.x) / (double(pixBottomRight.x) - double(pixTopLeft.x)) };
		double yScale{ (fractBottomRight.y - fractTopLeft.y) / (double(pixBottomRight.y) - double(pixTopLeft.y)) };

		double xPos{ fractTopLeft.x }, yPos{ fractTopLeft.y };
		int rowSize{ ScreenWidth() }, yOffset{ 0 };

		for (int y = pixTopLeft.y; y < pixBottomRight.y; ++y)
		{
			xPos = fractTopLeft.x;
			for (int x = pixTopLeft.x; x < pixBottomRight.x; ++x)
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
				m_pFractal[static_cast<int64_t>(x) + yOffset] = n;
				xPos += xScale;
			}

			yPos += yScale;
			yOffset += rowSize;
		}

		return true;
	}

	void Application::TakeScreenShot()
	{
		Timer timer;
		static int index{ 0 };

		LOG_TRACE("Taking Screenshot");
		Bitmap image(ScreenWidth(), ScreenHeight());
		LOG_TRACE("Bitmap created");

		LOG_WARN("Colouring Image");
		for (int y = 0; y < ScreenHeight(); ++y)
			for (int x = 0; x < ScreenWidth(); ++x)
			{
				int i{ m_pFractal[y * ScreenWidth() + x] };

				float n(i), a{ 0.1f };
				uint8_t red(0), green(0), blue(0);

				if (i != m_Iterations)
				{
					red = static_cast<uint8_t>(256 * static_cast<double>(0.5f * sin(a * n) + 0.5f));
					green = static_cast<uint8_t>(256 * static_cast<double>(0.2f * sin(a * n + 2.094f) + 0.5f));
					blue = static_cast<uint8_t>(256 * static_cast<double>(0.5f * sin(a * n + 4.188f) + 0.5f));
				}
				image.SetPixel(x, y, red, green, blue);
			}

		LOG_INFO("Image Coloured");

		std::string fileName("Fractal_Screenshot_");
		fileName.append(std::to_string(index++));
		fileName.append(".bmp");

		image.Write(fileName);
		LOG_INFO("Bitmap generation successful");
		LOG_TRACE("Bitmap saved as {0}", fileName);
	}

	inline void Application::HandleInput(olc::vi2d& pixTopLeft, olc::vi2d& pixBottomRight, olc::vd2d& fractTopLeft, olc::vd2d& fractBottomRight)
	{
		olc::vd2d mouseCoords = { static_cast<double>(GetMouseX()), static_cast<double>(GetMouseY()) };

		if (GetMouse(2).bPressed)
			m_StartPan = mouseCoords;

		if (GetMouse(2).bHeld)
		{
			m_Offset -= (mouseCoords - m_StartPan) / m_Scale;
			m_StartPan = mouseCoords;
		}

		olc::vd2d mouseCoordsBeforeZoom;
		ScreenToWorld(mouseCoords, mouseCoordsBeforeZoom);

		if (GetKey(olc::Key::SPACE).bPressed)						TakeScreenShot();
		if (GetKey(olc::Key::UP).bHeld || GetMouseWheel() > 0)		m_Scale *= 1.1;
		if (GetKey(olc::Key::DOWN).bHeld || GetMouseWheel() < 0)	m_Scale *= 0.9;
		if (GetKey(olc::Key::RIGHT).bPressed)						m_Iterations += 64;
		if (GetKey(olc::Key::LEFT).bPressed && m_Iterations > 64)	m_Iterations -= 64;

		olc::vd2d mouseCoordsAfterZoom;
		ScreenToWorld(mouseCoords, mouseCoordsAfterZoom);
		m_Offset += (mouseCoordsBeforeZoom - mouseCoordsAfterZoom);

		ScreenToWorld(pixTopLeft, fractTopLeft);
		ScreenToWorld(pixBottomRight, fractBottomRight);
	}

	inline void Application::RenderFrame()
	{
		for (int y = 0; y < ScreenHeight(); y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{
				int i{ m_pFractal[static_cast<int64_t>(y) * ScreenWidth() + x] };
				float n(i), a{ 0.1f };

				if (i == m_Iterations)
					Draw(x, y, olc::BLACK);
				else
					Draw(x, y, olc::PixelF(0.5f * sin(a * n) + 0.5f, 0.2f * sin(a * n + 2.094f) + 0.5f, 0.5f * sin(a * n + 4.188f) + 0.5f));
			}
	}

	inline void Application::ScreenToWorld(const olc::vi2d& n, olc::vd2d& v)
	{
		v.x = static_cast<double>(n.x) / m_Scale.x + m_Offset.x;
		v.y = static_cast<double>(n.y) / m_Scale.y + m_Offset.y;
	}
}


int main()
{
	//// TEST
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
	////

	//Fractal::Log::Init();
	//LOG_TRACE("System logger initialized");

	//auto app = Fractal::Application::CreateHeapApplication();
	//LOG_INFO("Application Launched");
	//LOG_WARN("Memory: {0}", s_AllocationTracker.CurrentUsage());
	//if (app->Construct(1280, 720, 1, 1, false, false))
	//	app->Start();

	//Fractal::Application::DestroyHeapApplication(app);
	//LOG_TRACE("Resources freed");
	//LOG_WARN("Memory: {0}", s_AllocationTracker.CurrentUsage());

	//return 0;
}
