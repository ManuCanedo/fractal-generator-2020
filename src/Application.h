#pragma once

namespace Fractal
{
	class Application
	{
	public:
		~Application() = default;
		Application();

		static Application* CreateHeapApplication();
		static void DestroyHeapApplication(Application* app);

		void Run();
		void Update();
		void OnEvent(Event& event);

	private:
		bool CalculateFractalSection(const int width, const Point2D& pixTopLeft, const Point2D& pixBottomRight, const Point2D& fractTopLeft, Point2D& fractBottomRight);
		bool SaveFractal(const int width, const int height);
		void ChangeWorldScale(double scalingFactor);
		
		inline void ScreenToWorld(const Point2D& n, Point2D& v);

	private:
		std::unique_ptr<uint8_t[]> m_pFractal{ nullptr };
		std::unique_ptr<Window> m_pWindow{ nullptr };
		
		int m_Iterations{ 128 }, m_Threads{ 32 };
		std::vector<std::future<bool>> m_Futures;

		Point2D m_Offset{ 0, 0 }, m_StartPan{ 0, 0 }, m_MouseCoords{ 0.0, 0.0 };
		Point2D m_Scale;

		double m_ScalingFactor{ 1.0 };
		bool m_bRunning{ true }, m_bPanning{ false }, m_bScreenshot{ false };
	};
}

