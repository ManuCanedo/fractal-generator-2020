#include "fpch.h"

#include "tools/AllocationTracker.h"
#include "Fractal.h"
#include "Window.h"
#include "Bitmap.h"
#include "Application.h"

namespace Fractal
{
	Application::Application() : m_Futures(m_Threads)
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback([this](Event& event) { OnEvent(event); });
		m_pFractal = std::make_unique<unsigned char[]>(3 * m_Window->GetWidth() * m_Window->GetHeight());
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
		while (m_Running)
		{
			Update();
		}
	}

	void Application::Update()
	{
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		Point2D pixTopLeft = { 0,0 };
		Point2D pixBottomRight = { static_cast<double>(m_Window->GetWidth()), static_cast<double>(m_Window->GetHeight()) };
		Point2D fractTopLeft = { -2.0, -1.0 };
		Point2D fractBottomRight = { 1.0, 1.0 };

		Point2D mouseCoordsBeforeZoom;
		ScreenToWorld(m_MouseCoords, mouseCoordsBeforeZoom);

		Point2D mouseCoordsAfterZoom;
		ScreenToWorld(m_MouseCoords, mouseCoordsAfterZoom);
		m_Offset += (mouseCoordsBeforeZoom - mouseCoordsAfterZoom);

		ScreenToWorld(pixTopLeft, fractTopLeft);
		ScreenToWorld(pixBottomRight, fractBottomRight);

		double screenSectionWidth{ (pixBottomRight.x - pixTopLeft.x) / m_Threads };
		double fractalSectionWidth{ (fractBottomRight.x - fractTopLeft.x) / m_Threads };
		double rWeight{ 0.5 }, gWeight{ 0.2 }, bWeight{ 0.5 };

		for (size_t i = 0; i < m_Threads; i++)
		{
			ScrSection scrSec{ {pixTopLeft.x + screenSectionWidth * i, pixTopLeft.y}, {pixTopLeft.x + screenSectionWidth * (i + 1), pixBottomRight.y} };
			ScrSection fracSec{ {fractTopLeft.x + fractalSectionWidth * i, fractTopLeft.y}, {fractTopLeft.x + fractalSectionWidth * (i + 1), fractBottomRight.y} };

			//LOG_ERROR("scrSec: {0},{1} - {2},{3}", scrSec.topLeft.x, scrSec.topLeft.y, scrSec.bottomRight.x, scrSec.bottomRight.y);
			//LOG_ERROR("fracSec: {0},{1} - {2},{3}", fracSec.topLeft.x, fracSec.topLeft.y, fracSec.bottomRight.x, fracSec.bottomRight.y);

			m_Futures[i] = std::async(std::launch::async, &Fractal::GenerateFractalFrame,
				m_pFractal.get(), scrSec, fracSec, m_Iterations, rWeight, gWeight, bWeight);
		}

		for (auto& f : m_Futures)
			f.wait();

		glDrawPixels(m_Window->GetWidth(), m_Window->GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, m_pFractal.get());

		m_Window->OnUpdate();
	}

	void Application::ScreenShot()
	{
		Timer timer;
		static int index{ 0 };
		const int width{ m_Window->GetWidth() }, height{ m_Window->GetHeight() };

		LOG_TRACE("Taking Screenshot");
		Bitmap image(width, height);

		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				uint8_t red =	m_pFractal[3 * (y * width + x) + 0];
				uint8_t green =	m_pFractal[3 * (y * width + x) + 1];
				uint8_t blue =	m_pFractal[3 * (y * width + x) + 2];

				image.SetPixel(x, y, red, green, blue);
			}

		std::string fileName("Fractal_Screenshot_");
		fileName.append(std::to_string(index++));
		fileName.append(".bmp");

		image.Write(fileName);
		LOG_INFO("Bitmap generation successful");
		LOG_INFO("Image saved as {0}", fileName);
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) { return OnWindowClose(event); });
		dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& event)
			{
				m_MouseCoords = { event.GetX(), event.GetY() };

				return true;
			});
		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event)
			{
				if (event.GetMouseButton() == MouseButtonCode::LEFTCLICK)
					m_StartPan = m_MouseCoords;

				return true;
			});
		dispatcher.Dispatch<MouseButtonHeldEvent>([this](MouseButtonHeldEvent& event)
			{
				if (event.GetMouseButton() == MouseButtonCode::LEFTCLICK)
				{
					m_Offset -= (m_MouseCoords - m_StartPan) / m_Scale;
					m_StartPan = m_MouseCoords;
				}

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
					m_Running = false;
					break;
				}
				case KeyCode::SPACE:
				{
					ScreenShot();
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
				{
					m_Scale *= 1.1;
					break;
				}
				case KeyCode::DOWN:
				{
					m_Scale *= 0.9;
					break;
				}
				default:
					return true;
				}

				Point2D mouseCoordsAfterZoom, mouseCoordsBeforeZoom;
				ScreenToWorld(m_MouseCoords, mouseCoordsBeforeZoom);
				ScreenToWorld(m_MouseCoords, mouseCoordsAfterZoom);
				m_Offset += (mouseCoordsBeforeZoom - mouseCoordsAfterZoom);

				return true;
			});
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	inline void Application::ScreenToWorld(const Point2D& n, Point2D& v)
	{
		v.x = static_cast<double>(n.x) / m_Scale.x + m_Offset.x;
		v.y = static_cast<double>(n.y) / m_Scale.y + m_Offset.y;
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
