#include "fpch.h"
#include "Application.h"
#include "Bitmap.h"
#include "tools/AllocationTracker.h"

#include <GLFW/glfw3.h>

namespace Fractal
{
	static std::mutex s_Mutex;
	Application* Application::s_pInstance{ nullptr };

	// Globals only used in constructor below
	constexpr char* NAME = "Fractal Explorer";
	constexpr int WIDTH{ 1600 }, HEIGHT{ 900 };

	Application::Application() : m_Futures(m_Threads), m_Scale({ WIDTH / 2, HEIGHT })
	{
		s_pInstance = this;

		m_pWindow = std::unique_ptr<Window>(Window::Create({ NAME, WIDTH, HEIGHT }));
		m_pWindow->SetEventCallback([this](Event& event) { OnEvent(event); });
		m_pFractal = std::make_unique<uint8_t[]>(3 * static_cast<int64_t>(m_pWindow->GetWidth()) * m_pWindow->GetHeight());
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
		Point2D pixTopLeft{ 0.0f, 0.0f }; 
		Point2D pixBottomRight{ static_cast<float>(m_pWindow->GetWidth()), static_cast<float>(m_pWindow->GetHeight()) };
		Point2D fractTopLeft{ -2.0f, -1.0f }, fractBottomRight{ 1.0f, 1.0f };

		ChangeWorldScale(m_ScalingFactor);
		ScreenToWorld(pixTopLeft, fractTopLeft);
		ScreenToWorld(pixBottomRight, fractBottomRight);

		float scrSectionWidth{ (pixBottomRight.x - pixTopLeft.x) / m_Threads };
		float fracSectionWidth{ (fractBottomRight.x - fractTopLeft.x) / m_Threads };

		for (size_t i = 0; i < m_Threads; ++i)
			m_Futures[i] = std::async(std::launch::async, &Application::CalculateFractalSection, this,
				Point2D(pixTopLeft.x + (scrSectionWidth * i), pixTopLeft.y),
				Point2D(pixTopLeft.x + (scrSectionWidth * (i + 1)), pixBottomRight.y),
				Point2D(fractTopLeft.x + (fracSectionWidth * i), fractTopLeft.y),
				Point2D(fractTopLeft.x + (fracSectionWidth * (i + 1)), fractBottomRight.y));

		if (m_bScreenshot) m_bScreenshot = !SaveFractal(m_pWindow->GetWidth(), m_pWindow->GetHeight());

		for (auto& f : m_Futures) f.wait();
		glDrawPixels(m_pWindow->GetWidth(), m_pWindow->GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, m_pFractal.get());

		m_pWindow->OnUpdate();
	}

	bool Application::CalculateFractalSection(const Point2D&& pixTopLeft, const Point2D&& pixBottomRight, const Point2D&& fractTopLeft, const Point2D&& fractBottomRight)
	{
		float xScale{ (fractBottomRight.x - fractTopLeft.x) / (pixBottomRight.x - pixTopLeft.x) };
		float yScale{ (fractBottomRight.y - fractTopLeft.y) / (pixBottomRight.y - pixTopLeft.y) };

		float xPos{ fractTopLeft.x }, yPos{ fractTopLeft.y };
		int rowSize{ m_pWindow->GetWidth() }, yOffset{ 0 };

		for (int y = static_cast<int>(pixTopLeft.y); y < pixBottomRight.y; ++y)
		{
			xPos = fractTopLeft.x;
			for (int x = static_cast<int>(pixTopLeft.x); x < pixBottomRight.x; ++x)
			{
				std::complex<double> c{ xPos, yPos };
				std::complex<double> z{ 0.0, 0.0 };
				int n{ 0 };

				while (z.real() * z.real() + z.imag() * z.imag() < 4 && n < m_Iterations)
				{
					z = { z.real() * z.real() - z.imag() * z.imag() + c.real(),
						2 * z.real() * z.imag() + c.imag() };
					++n;
				}

				uint8_t* pPixels{ &m_pFractal[3 * static_cast<int64_t>(yOffset + x)] };
				uint8_t red{ 0 }, green{ 0 }, blue{ 0 };

				if (n < m_Iterations)
				{
					red = static_cast<uint8_t>	(256 * (m_DynamicRGB.x * sin(0.1 * static_cast<float>(n) + m_RGBOffset.x) + m_StaticRGB.x));
					green = static_cast<uint8_t>	(256 * (m_DynamicRGB.y * sin(0.1 * static_cast<float>(n) + m_RGBOffset.y) + m_StaticRGB.y));
					blue = static_cast<uint8_t>	(256 * (m_DynamicRGB.z * sin(0.1 * static_cast<float>(n) + m_RGBOffset.z) + m_StaticRGB.z));
				}

				pPixels[0] = red;
				pPixels[1] = green;
				pPixels[2] = blue;

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
					m_bPanning = false;
				return true;
			});

		dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event)
			{
				(event.GetOffsetY() > 0) ? m_ScalingFactor = 1.1f : m_ScalingFactor = 0.9f;
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
					break;
				}
				return true;
			});

		dispatcher.Dispatch<KeyHeldEvent>([this](KeyHeldEvent& event)
			{
				switch (event.GetKeyCode())
				{
				case KeyCode::UP:
				case KeyCode::DOWN:
					(event.GetKeyCode() == KeyCode::UP) ? m_ScalingFactor = 1.1f : m_ScalingFactor = 0.9f;
				}
				return true;
			});
	}

	void Application::ChangeWorldScale(float scalingFactor)
	{
		Point2D mouseCoordsBeforeZoom{ 0.0f, 0.0f }, mouseCoordsAfterZoom{ 0.0f, 0.0f };
		ScreenToWorld(m_MouseCoords, mouseCoordsBeforeZoom);
		m_Scale *= scalingFactor;
		ScreenToWorld(m_MouseCoords, mouseCoordsAfterZoom);

		m_Offset += mouseCoordsBeforeZoom - mouseCoordsAfterZoom;
		m_ScalingFactor = 1.0f;
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
	LOG_TRACE("Initializing system logger");

	LOG_TRACE("Initializing application");
	auto app = std::make_unique<Fractal::Application>();

	LOG_WARN("Memory: {0}KB", s_AllocationTracker.CurrentUsage());

	LOG_INFO("Launching application");
	app->Run();

	LOG_WARN("Memory: {0}KB", s_AllocationTracker.CurrentUsage());

	return 0;
}
