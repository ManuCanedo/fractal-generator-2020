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

		void SetRedOffset(float val) { m_RGBOffset.x.exchange(val); }
		void SetGreenOffset(float val) { m_RGBOffset.y.exchange(val); }
		void SetBlueOffset(float val) { m_RGBOffset.z.exchange(val); }

		void SetModeAVX(bool val) { m_bAVX = val; }
		void SetModeBinary(bool val) { m_bBinarySearch = val; }
		void SetScreenshot(bool val) { m_bScreenshot = val; }

		Window& GetWindow() { return *m_Window; }
		int GetIterations() { return m_Iterations; }

		static Application& GetApplication() { return *s_pInstance; }

	private:
		bool CalculateFractalSection(uint8_t* pPixels, int width, int iterations, 
			const Point2D&& pixTopLeft, const Point2D&& pixBottomRight, const Point2D&& fractTopLeft, const Point2D&& fractBottomRight);
		bool CalculateFractalSectionAVX(uint8_t* pPixels, int width, int iterations, 
			const Point2D&& pixTopLeft, const Point2D&& pixBottomRight, const Point2D&& fractTopLeft, const Point2D&& fractBottomRight);
		bool SaveFractal(int width, int height);
		void ChangeWorldScale(double scalingFactor);
		inline void ScreenToWorld(const Point2D& n, Point2D& v);

	private:
		static Application* s_pInstance;

	private:
		std::unique_ptr<uint8_t[]> m_pFractal{ nullptr };
		std::unique_ptr<Window> m_Window{ nullptr };

		const int m_Threads{ 16 };
		std::vector<std::future<bool>> m_Futures;

		// Fractal Generation
		int m_Iterations{ 256 };
		Point2D m_Offset{ 0.0, 0.0 }, m_StartPan{ 0.0, 0.0 }, m_MouseCoords{ 0.0, 0.0 }, m_Scale;

		// Fractal Colouring
		struct Vec3f { std::atomic<float> x, y, z; } m_RGBOffset{ 0.3f, 2.4f, 4.0f };

		// Update Flags
		bool m_bAVX{ false }, m_bBinarySearch{ false }, m_bScreenshot{ false };
		bool m_bRunning{ true }, m_bPanning{ false };
		double m_ScalingFactor{ 1.0f };
	};
}

