#pragma once

namespace Fractal
{
	class Application
	{
	public:
		Application();
		~Application() = default;
		static Application* CreateHeapApplication();
		static void DestroyHeapApplication(Application* app);

		void Run();
		void Update();
		void ScreenShot();

		void OnEvent(Event& event);

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		inline void ScreenToWorld(const Point2D& n, Point2D& v);

	private:
		std::unique_ptr<unsigned char[]> m_pFractal{ nullptr };
		std::unique_ptr<Window> m_Window{ nullptr };

		unsigned int m_Iterations{ 256 }, m_Threads{ 3 };
		std::vector<std::future<bool>> m_Futures;
		bool m_Running{ true };

		Point2D m_Offset{ 0.0, 0.0 };
		Point2D m_StartPan{ 0.0, 0.0 };
		Point2D m_Scale{ 1280.0 / 2.0, 720.0 };
		Point2D m_MouseCoords{ 0, 0 };

	};
}

