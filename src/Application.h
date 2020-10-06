#pragma once

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "Window.h"

namespace Fractal
{
	class Application : public olc::PixelGameEngine
	{
	public:
		Application();
		~Application() = default;
		static Application* CreateHeapApplication();
		static void DestroyHeapApplication(Application* app);

		void OnEvent(Event& event);

		void Run();
		void Update();
		void TakeScreenShot();


		// Pixel Engine
		bool OnUserCreate() override;
		bool OnUserDestroy() override;
		bool OnUserUpdate(float fElapsedTime) override;

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		bool GenerateFractalFrame(const olc::vi2d pixTopLeft, const olc::vi2d pixBottomRight,
			const olc::vd2d fractTopLeft, const olc::vd2d fractBottomRight, int iterations);

		void RenderFrame();
		void HandleInput(olc::vi2d& pixTopLeft, olc::vi2d& pixBottomRight,
			olc::vd2d& fractTopLeft, olc::vd2d& fractBottomRight);

		inline void ScreenToWorld(const olc::vi2d& n, olc::vd2d& v);

	private:
		std::unique_ptr<int[]> m_pFractal{ nullptr };
		std::unique_ptr<Window> m_Window{ nullptr };

		int m_Iterations{ 256 }, m_Threads{ 32 };
		std::vector<std::future<bool>> m_Futures;

		bool m_Running{ true };

		olc::vd2d m_Offset{ 0.0, 0.0 };
		olc::vd2d m_StartPan{ 0.0, 0.0 };
		olc::vd2d m_Scale{ 1280.0 / 2.0, 720.0 };
	};
}

