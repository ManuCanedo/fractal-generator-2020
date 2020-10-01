#pragma once

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

namespace Fractal 
{
	class Application : public olc::PixelGameEngine
	{
	public:
		static inline Application* CreateApplication();
		~Application() = default;

		bool OnUserCreate() override;
		bool OnUserUpdate(float fElapsedTime) override;

		void ScreenShot();

	private:
		Application();

		void GenerateFractalFrame(const olc::vi2d& pix_tl, const olc::vi2d& pix_br, const olc::vd2d& frac_tl, const olc::vd2d& frac_br);
		inline void WorldToScreen(const olc::vd2d& v, olc::vi2d& n);
		inline void ScreenToWorld(const olc::vi2d& n, olc::vd2d& v);

	private:
		std::unique_ptr<int[]> m_pFractal{ nullptr };
		int m_Iterations{ 1024 };

		// Pan & Zoom variables
		olc::vd2d m_vOffset { 0.0, 0.0 };
		olc::vd2d m_vStartPan { 0.0, 0.0 };
		olc::vd2d m_vScale { 1280.0 / 2.0, 720.0 };
	};
}

