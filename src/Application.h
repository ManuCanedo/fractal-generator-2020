#pragma once

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

namespace Fractal 
{
	class Application : public olc::PixelGameEngine
	{
	public:
		Application();
		~Application() = default;
		static Application* CreateHeapApplication();
		static void DestroyHeapApplication(Application* app);

		bool OnUserCreate() override;
		bool OnUserDestroy() override;
		bool OnUserUpdate(float fElapsedTime) override;
		void ScreenShot();

	private:
		bool GenerateFractalFrame(const olc::vi2d pixTopLeft, const olc::vi2d pixBottomRight,
			const olc::vd2d fractTopLeft, const olc::vd2d fractBottomRight, int iterations);

		inline void HandleInput(olc::vi2d& pixTopLeft, olc::vi2d& pixBottomRight,
			olc::vd2d& fractTopLeft, olc::vd2d& fractBottomRight);
		inline void RenderFrame();
		inline void WorldToScreen(const olc::vd2d& v, olc::vi2d& n);
		inline void ScreenToWorld(const olc::vi2d& n, olc::vd2d& v);

	private:
		std::unique_ptr<int[]> m_pFractal{ nullptr };
		int m_Iterations{ 256 };

		int m_Threads{ 32 };
		std::vector<std::future<bool>> m_Futures;

		olc::vd2d m_Offset { 0.0, 0.0 };
		olc::vd2d m_StartPan { 0.0, 0.0 };
		olc::vd2d m_Scale { 1280.0 / 2.0, 720.0 };
	};
}

