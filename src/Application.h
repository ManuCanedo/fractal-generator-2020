#pragma once

#include "Window.h"
#include "UI/ImGuiLayer.h"

namespace Fractal
{
	class Application
	{
	public:
		Application();
		~Application() = default;

		void Run();
		void Update();
		void OnEvent(Event& event);

		void SetRedOffset(const float val) { m_RGBOffset.x.exchange(val); }
		void SetGreenOffset(const float val) { m_RGBOffset.y.exchange(val); }
		void SetBlueOffset(const float val) { m_RGBOffset.z.exchange(val); }

		void SetModeAVX(const bool val) { m_bAVX2 = val; }
		void SetModeBinary(const bool val) { m_bBinarySearch = val; }
		void SetScreenshot(const bool val) { m_bScreenshot = val; }

		Window& GetWindow() const { return *m_Window; }
		constexpr int GetIterations() const { return m_Iterations; }

		static Application& GetApplication() { return *s_pInstance; }

	private:
		bool CalculateFractalSection(uint8_t* pMemory, const unsigned int width, const unsigned int iterations,
			const Point2D& pixTopLeft, const Point2D& pixBottomRight, const Point2D& fractTopLeft, const Point2D& fractBottomRight);
		bool CalculateFractalSectionAVX(uint8_t* pMemory, unsigned int width, unsigned int iterations,
			const Point2D& pixTopLeft, const Point2D& pixBottomRight, const Point2D& fractTopLeft, const Point2D& fractBottomRight);
		void SaveFractal();

		inline void ChangeWorldScale(const double scalingFactor);
		inline void ScreenToWorld(const Point2D& n, Point2D& v) const;

	private:
		static Application* s_pInstance;

	private:
		std::unique_ptr<uint8_t[]> m_pFractal;
		std::unique_ptr<Window> m_Window;

		const int m_Threads;
		std::vector<std::future<bool>> m_Futures;

		// Fractal Generation
		unsigned int m_Iterations{ 258 };
		Point2D m_Offset{ 0.0, 0.0 }, m_StartPan{ 0.0, 0.0 }, m_MouseCoords{ 0.0, 0.0 }, m_Scale{ 1.0, 1.0 };

		// Fractal Colouring
		struct Vec3f { std::atomic<float> x, y, z; } m_RGBOffset{ 0.0f, 0.0f, 0.0f };

		// Update Flags
		bool m_bAVX2{ false }, m_bBinarySearch{ false }, m_bScreenshot{ false };
		bool m_bSupportedAVX2{ false }, m_bRunning{ true }, m_bPanning{ false };
		double m_ScalingFactor{ 1.0f };
	};
}

