#include "fpch.h"
#include "Application.h"
#include "Bitmap.h"
#include "tools/AllocationTracker.h"
#include "Tools/CPUFeatures.h"

#include <GLFW/glfw3.h>

namespace Fractal
{
	static std::mutex s_Mutex;
	Application* Application::s_pInstance{ nullptr };

	constexpr unsigned int WIDTH{ 1600 }, HEIGHT{ 900 }, THREADS{ 40 };

	Application::Application()
		: m_Threads(THREADS), m_Futures(m_Threads), m_Scale({ WIDTH / 2, HEIGHT })
	{
		Timer timer;
		s_pInstance = this;
		std::future<int> fut = std::async(std::launch::async, &SupportsIntelVectorExtensions);

		m_Window = std::unique_ptr<Window>(Window::Create({ "Fractal Explorer", WIDTH, HEIGHT }));
		m_Window->SetEventCallback([this](Event& event) { OnEvent(event); });
		m_pFractal = std::make_unique<uint8_t[]>(3 * static_cast<int64_t>(m_Window->GetWidth()) * m_Window->GetHeight());

		m_bSupportedAVX2 = fut.get();
	}

	void Application::Run()
	{
		while (m_bRunning)
			Update();
	}

	void Application::Update()
	{
		ChangeWorldScale(m_ScalingFactor);

		constexpr static Point2D m_PixTopLeft{ 0.0, 0.0 };
		const Point2D m_PixBottomRight{ static_cast<double>(m_Window->GetWidth()), static_cast<double>(m_Window->GetHeight()) };
		Point2D fractTopLeft{ -2.0, -1.0 }, fractBottomRight{ 1.0, 1.0 };

		ScreenToWorld(m_PixTopLeft, fractTopLeft);
		ScreenToWorld(m_PixBottomRight, fractBottomRight);

		const double scrSectionWidth{ (m_PixBottomRight.x - m_PixTopLeft.x) / m_Threads };
		const double fracSectionWidth{ (fractBottomRight.x - fractTopLeft.x) / m_Threads };
		unsigned int iters{ m_Iterations };

		if (m_bScreenshot) SaveFractal();											// Take Screenshot
		if (m_bBinarySearch) iters = static_cast<unsigned int>(log2(iters));		// Binary Search Simulation Mode 

		static bool bFirstFrame{ true };											// Multithreading optimization
		if (!bFirstFrame) 
		{
			for (auto&& f : m_Futures) f.wait();
			glDrawPixels(m_Window->GetWidth(), m_Window->GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, m_pFractal.get());
		}
		bFirstFrame = false;

		if (m_bAVX2 && m_bSupportedAVX2)											// AVX Fractal Calculation Mode
			for (size_t i = 0; i < m_Threads; ++i)
				m_Futures[i] = std::async(std::launch::async, &Application::CalculateFractalSectionAVX, this,
					m_pFractal.get(), m_Window->GetWidth(), iters,
					Point2D(m_PixTopLeft.x + (scrSectionWidth * i), m_PixTopLeft.y),
					Point2D(m_PixTopLeft.x + (scrSectionWidth * (i + 1)), m_PixBottomRight.y),
					Point2D(fractTopLeft.x + (fracSectionWidth * i), fractTopLeft.y),
					Point2D(fractTopLeft.x + (fracSectionWidth * (i + 1)), fractBottomRight.y));
		else																		// Standard Fractal Calculation Mode
			for (size_t i = 0; i < m_Threads; ++i)
				m_Futures[i] = std::async(std::launch::async, &Application::CalculateFractalSection, this,
					m_pFractal.get(), m_Window->GetWidth(), iters,
					Point2D(m_PixTopLeft.x + (scrSectionWidth * i), m_PixTopLeft.y),
					Point2D(m_PixTopLeft.x + (scrSectionWidth * (i + 1)), m_PixBottomRight.y),
					Point2D(fractTopLeft.x + (fracSectionWidth * i), fractTopLeft.y),
					Point2D(fractTopLeft.x + (fracSectionWidth * (i + 1)), fractBottomRight.y));

		m_Window->OnUpdate();
	}

	bool Application::CalculateFractalSection(uint8_t* pMemory, const unsigned int width, const unsigned int iterations,
		const Point2D&& pixTopLeft, const Point2D&& pixBottomRight, const Point2D&& fractTopLeft, const Point2D&& fractBottomRight)
	{
		const double xScale{ (fractBottomRight.x - fractTopLeft.x) / (pixBottomRight.x - pixTopLeft.x) };
		const double yScale{ (fractBottomRight.y - fractTopLeft.y) / (pixBottomRight.y - pixTopLeft.y) };

		double yPos{ fractTopLeft.y };
		unsigned int rowSize{ width }, yOffset{ 0 };

		for (int y = static_cast<int>(pixTopLeft.y); y < pixBottomRight.y; ++y)
		{
			double xPos{ fractTopLeft.x };
			for (int x = static_cast<int>(pixTopLeft.x); x < pixBottomRight.x; ++x)
			{
				std::complex<double> z{ 0.0, 0.0 };
				std::complex<double> c{ xPos, yPos };
				unsigned int n{ 0 };

				while (z.real() * z.real() + z.imag() * z.imag() < 4 && n < iterations)
				{
					z = { z.real() * z.real() - z.imag() * z.imag() + c.real(),
						2 * z.real() * z.imag() + c.imag() };
					++n;
				}

				uint8_t* pPixels{ pMemory + 3 * (static_cast<int64_t>(yOffset) + x) };
				uint8_t red{ 0 }, green{ 0 }, blue{ 0 };

				if (n < iterations)
				{
					red = static_cast<uint8_t>	(256 * (0.5 * sin(0.1 * static_cast<float>(n) + m_RGBOffset.x) + 0.5));
					green = static_cast<uint8_t>	(256 * (0.2 * sin(0.1 * static_cast<float>(n) + m_RGBOffset.y) + 0.5));
					blue = static_cast<uint8_t>	(256 * (0.5 * sin(0.1 * static_cast<float>(n) + m_RGBOffset.z) + 0.5));
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

	bool Application::CalculateFractalSectionAVX(uint8_t* pMemory, const unsigned int width, const unsigned int iterations,
		const Point2D&& pixTopLeft, const Point2D&& pixBottomRight, const Point2D&& fractTopLeft, const Point2D&& fractBottomRight)
	{
		const double xScale{ (fractBottomRight.x - fractTopLeft.x) / (pixBottomRight.x - pixTopLeft.x) };
		const double yScale{ (fractBottomRight.y - fractTopLeft.y) / (pixBottomRight.y - pixTopLeft.y) };

		double yPos{ fractTopLeft.y };
		unsigned int rowSize{ width }, yOffset{ 0 };
		unsigned int x, y;

		/* ----------     ----------     ----------     ---------- */
		/* ----------            Intrinsics             ---------- */
		/* ----------     ----------     ----------     ---------- */

		// Double Registers
		__m256d _zr, _zi, _zr2, _zi2, _cr, _ci, _auxDouble1, _auxDouble2, _maskDouble;
		__m256d _two{ _mm256_set1_pd(2.0) }, _four{ _mm256_set1_pd(4.0) };
		__m256d _xScale{ _mm256_set1_pd(xScale) }, _xSkip{ _mm256_set1_pd(xScale * 4) };
		__m256d _xPos, _xPosOffset{ _mm256_mul_pd(_mm256_set_pd(0, 1, 2, 3), _xScale) };

		// Int Registers
		__m256i _n, _maskInt, _auxInt;
		__m256i _one{ _mm256_set1_epi64x(1) }, _iters{ _mm256_set1_epi64x(iterations) };

		for (y = static_cast<unsigned int>(pixTopLeft.y); y < pixBottomRight.y; y++)
		{
			_auxDouble1 = _mm256_set1_pd(fractTopLeft.x);
			_xPos = _mm256_add_pd(_auxDouble1, _xPosOffset);
			_ci = _mm256_set1_pd(yPos);

			for (x = static_cast<unsigned int>(pixTopLeft.x); x < pixBottomRight.x; x += 4)
			{
				//std::complex<double> z{ 0.0, 0.0 };
				_zr = _mm256_setzero_pd();
				_zi = _mm256_setzero_pd();

				//std::complex<double> c{ xPos, yPos };
				_cr = _xPos;

				//int n{ 0 };
				_n = _mm256_setzero_si256();

				//while ()
			repeat:

				// z.real() = z.real() * z.real() - z.imag() * z.imag() + c.real()
				_zr2 = _mm256_mul_pd(_zr, _zr);
				_zi2 = _mm256_mul_pd(_zi, _zi);
				_auxDouble1 = _mm256_sub_pd(_zr2, _zi2);
				_auxDouble2 = _mm256_mul_pd(_zr, _zi);
				_zr = _mm256_add_pd(_auxDouble1, _cr);
				_zi = _mm256_fmadd_pd(_auxDouble2, _two, _ci);

				//  z.real() * z.real() + z.imag() * z.imag() < 4
				_auxDouble1 = _mm256_add_pd(_zr2, _zi2);
				_maskDouble = _mm256_cmp_pd(_auxDouble1, _four, _CMP_LT_OQ);

				// iterations > n
				_maskInt = _mm256_cmpgt_epi64(_iters, _n);

				// (z.real() * z.real() + z.imag() * z.imag() < 4 && n < iterations)
				_maskInt = _mm256_and_si256(_maskInt, _mm256_castpd_si256(_maskDouble));

				// ++n
				_auxInt = _mm256_and_si256(_one, _maskInt);
				_n = _mm256_add_epi64(_n, _auxInt);

				// if cond true true, keeps iterating
				if (_mm256_movemask_pd(_mm256_castsi256_pd(_maskInt)) > 0)
					goto repeat;

				_xPos = _mm256_add_pd(_xPos, _xSkip);

				/* ----------     ----------     ----------     ---------- */
				/* ----------        End of Intrinsics          ---------- */
				/* ----------     ----------     ----------     ---------- */

				uint8_t* pPixels{ &pMemory[3 * (static_cast<int64_t>(yOffset) + x)] };
				
				for (int i = 3; i >= 0; --i)
				{
					// reg _n contains [pix3 n][pix2 n][pix1 n][pix0 n]
					unsigned int n{ static_cast<unsigned int>(int(_n.m256i_i64[i])) };
					uint8_t red{ 0 }, green{ 0 }, blue{ 0 };

					if (n < iterations)
					{
						red = static_cast<uint8_t>	(256 * (0.5 * sin(0.1 * static_cast<float>(n) + m_RGBOffset.x) + 0.5));
						green = static_cast<uint8_t>	(256 * (0.2 * sin(0.1 * static_cast<float>(n) + m_RGBOffset.y) + 0.5));
						blue = static_cast<uint8_t>	(256 * (0.5 * sin(0.1 * static_cast<float>(n) + m_RGBOffset.z) + 0.5));
					}

					pPixels[0] = red;
					pPixels[1] = green;
					pPixels[2] = blue;
					pPixels += 3;
				}
			}
			yPos += yScale;
			yOffset += rowSize;
		}

		return true;
	}

	void Application::SaveFractal()
	{
		static uint8_t index{ 0 };
		unsigned int width{ m_Window->GetWidth() }, height{ m_Window->GetHeight() };

		Timer timer;
		Bitmap image(width, height);
		{
			std::lock_guard<std::mutex> lock(s_Mutex);
			for (size_t y = 0; y < height; ++y)
				for (size_t x = 0; x < width; ++x)
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

		m_bScreenshot = false;
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>([this](const WindowCloseEvent& event)
			{
				m_bRunning = false;
				return true;
			});

		dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent& event)
			{
				m_MouseCoords = { event.GetX(), event.GetY() };
				if (m_bPanning)
				{
					m_Offset -= Point2D::InvertY((m_MouseCoords - m_StartPan) / m_Scale);
					m_StartPan = m_MouseCoords;
				}
				return true;
			});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& event)
			{
				if (event.GetMouseButton() == MouseButtonCode::WHEEL)
				{
					m_StartPan = m_MouseCoords;
					m_bPanning = true;
				}
				return m_bPanning;
			});

		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent& event)
			{
				if (event.GetMouseButton() == MouseButtonCode::WHEEL)
					m_bPanning = false;
				return true;
			});

		dispatcher.Dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent& event)
			{
				(event.GetOffsetY() > 0) ? m_ScalingFactor = 1.1f : m_ScalingFactor = 0.9f;
				return true;
			});

		dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent& event)
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

		dispatcher.Dispatch<KeyHeldEvent>([this](const KeyHeldEvent& event)
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

	inline void Application::ChangeWorldScale(const double scalingFactor)
	{
		Point2D mouseCoordsBeforeZoom{ 0.0f, 0.0f }, mouseCoordsAfterZoom{ 0.0f, 0.0f };
		ScreenToWorld(m_MouseCoords, mouseCoordsBeforeZoom);
		m_Scale *= scalingFactor;
		ScreenToWorld(m_MouseCoords, mouseCoordsAfterZoom);

		m_Offset += mouseCoordsBeforeZoom - mouseCoordsAfterZoom;
		m_ScalingFactor = 1.0f;
	}

	inline void Application::ScreenToWorld(const Point2D& n, Point2D& v) const
	{
		v.x = n.x / m_Scale.x + m_Offset.x;
		v.y = n.y / m_Scale.y + m_Offset.y;
	}
}


int main()
{
	Fractal::Log::Init();
	LOG_TRACE("System logger initialized");

	LOG_TRACE("Application launched");
	auto app = std::make_unique<Fractal::Application>();
	LOG_TRACE("Memory: {0}KB", s_AllocationTracker.CurrentUsage());

	Fractal::Timer timer;
	app->Run();

	return 0;
}
