#include "fpch.h"

#include "tools/AllocationTracker.h"
#include "Window.h"
#include "Bitmap.h"
#include "Application.h"

namespace Fractal
{
	static std::mutex s_Mutex;

	// Ez-tweak globals, only used in constructor below
	constexpr char* NAME = "Fractal Explorer";
	constexpr int WIDTH{ 1600 }, HEIGHT{ 900 };

	Application::Application() : m_Futures(m_Threads), m_Scale({ WIDTH / 2, HEIGHT })
	{
		m_pWindow = std::unique_ptr<Window>(Window::Create({ NAME, WIDTH, HEIGHT }));
		m_pWindow->SetEventCallback([this](Event& event) { OnEvent(event); });
		m_pFractal = std::make_unique<uint8_t[]>(3 * static_cast<int64_t>(m_pWindow->GetWidth()) * m_pWindow->GetHeight());
	}

	Application* Application::CreateHeapApplication()
	{
		return new Application;
	}

	void Application::DestroyHeapApplication(Application* app)
	{
		delete app;
	}

	void Application::Run()
	{
		while (m_bRunning)
		{
			Update();
		}
	}

	void Application::Update()
	{
		int width{ m_pWindow->GetWidth() }, height{ m_pWindow->GetHeight() };

		if (m_bScreenshot) m_bScreenshot = !SaveFractal(width, height);

		Point2D pixTopLeft{ 0, 0 }, pixBottomRight(width, height);
		Point2D fractTopLeft{ -2.0, -1.0 }, fractBottomRight{ 1.0, 1.0 };

		ScreenToWorld(pixTopLeft, fractTopLeft);
		ScreenToWorld(pixBottomRight, fractBottomRight);

		double screenSectionWidth{ (pixBottomRight.x - pixTopLeft.x) / m_Threads };
		double fractalSectionWidth{ (fractBottomRight.x - fractTopLeft.x) / m_Threads };

		for (size_t i = 0; i < m_Threads; ++i)
			m_Futures[i] = std::async(std::launch::async, &Application::CalculateFractalSection, this, width,
				Point2D(pixTopLeft.x + (screenSectionWidth * i), pixTopLeft.y),
				Point2D(pixTopLeft.x + (screenSectionWidth * (i + 1)), pixBottomRight.y),
				Point2D(fractTopLeft.x + (fractalSectionWidth * i), fractTopLeft.y),
				Point2D(fractTopLeft.x + (fractalSectionWidth * (i + 1)), fractBottomRight.y));


		glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, m_pFractal.get());
		m_pWindow->OnUpdate();
	}

	bool Application::CalculateFractalSection(const int width, const Point2D& pixTopLeft, const Point2D& pixBottomRight, const Point2D& fractTopLeft, Point2D& fractBottomRight)
	{
		double xScale{ (fractBottomRight.x - fractTopLeft.x) / (pixBottomRight.x - pixTopLeft.x) };
		double yScale{ (fractBottomRight.y - fractTopLeft.y) / (pixBottomRight.y - pixTopLeft.y) };

		double xPos{ fractTopLeft.x }, yPos{ fractTopLeft.y };
		int rowSize{ width }, yOffset{ 0 };

		for (size_t y = pixTopLeft.y; y < pixBottomRight.y; ++y)
		{
			xPos = fractTopLeft.x;
			for (size_t x = pixTopLeft.x; x < pixBottomRight.x; ++x)
			{
				std::complex<double> c{ xPos, yPos };
				std::complex<double> z{ 0.0, 0.0 };

				auto zRe2{ z.real() * z.real() };
				auto zIm2{ z.imag() * z.imag() };
				int n{ 0 };

				while (zRe2 + zIm2 < 4 && n < m_Iterations)
				{
					z = { zRe2 - zIm2 + c.real(), 2 * z.real() * z.imag() + c.imag() };
					zRe2 = z.real() * z.real();
					zIm2 = z.imag() * z.imag();
					++n;
				}

				uint8_t red{ 0 }, green{ 0 }, blue{ 0 };
				if (n < m_Iterations)
				{
					red = static_cast<uint8_t>(256 * (0.5 * sin(0.1 * static_cast<double>(n)) + 0.5));
					green = static_cast<uint8_t>(256 * (0.2 * sin(0.1 * static_cast<double>(n) + 2.094) + 0.5));
					blue = static_cast<uint8_t>(256 * (0.5 * sin(0.1 * static_cast<double>(n) + 4.188) + 0.5));
				}

				m_pFractal[3 * static_cast<int64_t>(yOffset + x) + 0] = red;
				m_pFractal[3 * static_cast<int64_t>(yOffset + x) + 1] = green;
				m_pFractal[3 * static_cast<int64_t>(yOffset + x) + 2] = blue;

				xPos += xScale;
			}
			yPos += yScale;
			yOffset += rowSize;
		}

		return true;
	}

	bool Application::SaveFractal(const int width, const int height)
	{
		static int index{ 0 };

		Timer timer;
		Bitmap image(width, height);

		{
			std::lock_guard<std::mutex> lock(s_Mutex);
			for (int y = 0; y < height; ++y)
				for (int x = 0; x < width; ++x)
				{
					uint8_t* pPixel = &m_pFractal[3 * (y * static_cast<int64_t>(width) + x)];
					image.SetPixel(x, y, pPixel[0], pPixel[1], pPixel[2]);
				}
		}

		std::string fileName("Fractal_Screenshot_");
		fileName.append(std::to_string(index++));
		fileName.append(".bmp");

		image.Write(fileName);
		LOG_INFO("Screenshot saved as {0}", fileName);
		return true;
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) 
			{ 
				m_bRunning = false;
				return true;
			});

		dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& event)
			{
				m_MouseCoords = { event.GetX(), event.GetY() };
				if (m_bPanning)
				{
					m_Offset -= Point2D::InvertY((m_MouseCoords - m_StartPan) / m_Scale);
					m_StartPan = m_MouseCoords;
				}
				return true;
			});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event)
			{
				if (event.GetMouseButton() == MouseButtonCode::WHEEL)
				{
					m_StartPan = m_MouseCoords;
					m_bPanning = true;
				}
				return m_bPanning;
			});

		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& event)
			{
				if (event.GetMouseButton() == MouseButtonCode::WHEEL)
				{
					m_bPanning = false;
				}
				return true;
			});

		dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event)
			{
				Point2D mouseCoordsBeforeZoom, mouseCoordsAfterZoom;

				ScreenToWorld(m_MouseCoords, mouseCoordsBeforeZoom);
				m_Scale *= (event.GetOffsetY() > 0) ? 1.1 : 0.9;
				ScreenToWorld(m_MouseCoords, mouseCoordsAfterZoom);
				m_Offset += (mouseCoordsBeforeZoom - mouseCoordsAfterZoom);

				return true;
			});

		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event)
			{
				switch (event.GetKeyCode())
				{
				case KeyCode::LEFT:
				{
					if (m_Iterations > 64)
						m_Iterations -= 64;
					break;
				}
				case KeyCode::RIGHT:
				{
					m_Iterations += 64;
					break;
				}
				case KeyCode::ESCAPE:
				{
					m_bRunning = false;
					break;
				}
				case KeyCode::SPACE:
				{
					m_bScreenshot = true;
					break;
				}
				default:
					return true;
				}

				return true;
			});

		dispatcher.Dispatch<KeyHeldEvent>([this](KeyHeldEvent& event)
			{
				switch (event.GetKeyCode())
				{
				case KeyCode::UP:
				case KeyCode::DOWN:
					Point2D mouseCoordsBeforeZoom, mouseCoordsAfterZoom;
					ScreenToWorld(m_MouseCoords, mouseCoordsBeforeZoom);
					m_Scale *= (event.GetKeyCode() == KeyCode::UP) ? 1.1 : 0.9;
					ScreenToWorld(m_MouseCoords, mouseCoordsAfterZoom);
					m_Offset += (mouseCoordsBeforeZoom - mouseCoordsAfterZoom);
					break;
				}

				return true;
			});
	}

	inline void Application::ScreenToWorld(const Point2D& n, Point2D& v)
	{
		v.x = n.x / m_Scale.x + m_Offset.x;
		v.y = n.y / m_Scale.y + m_Offset.y;
	}
}


int main()
{
	Fractal::Log::Init();
	LOG_TRACE("System logger initialized");

	LOG_INFO("Application Launched");
	auto app = Fractal::Application::CreateHeapApplication();
	LOG_WARN("Memory: {0}KB", s_AllocationTracker.CurrentUsage());
	app->Run();

	Fractal::Application::DestroyHeapApplication(app);
	LOG_TRACE("Resources freed");
	LOG_WARN("Memory: {0}KB", s_AllocationTracker.CurrentUsage());

	return 0;
}
