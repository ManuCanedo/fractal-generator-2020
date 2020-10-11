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

		void SetBinarySimulation(bool bBinary) { m_bBinarySearch = bBinary; }
		void SetAVX(bool bAVX) { m_bAVX = bAVX; }
		void SetRGBOffset(float r, float g, float b) { m_RGBOffset = { r, g, b }; }

		Window& GetWindow() { return *m_pWindow; }
		int GetIterations() { return m_Iterations; }

		static Application& GetApplication() { return *s_pInstance; }

	private:
		bool CalculateFractalSection(const Point2D&& pixTopLeft, const Point2D&& pixBottomRight, const Point2D&& fractTopLeft, const Point2D&& fractBottomRight);
		bool SaveFractal(const int width, const int height);
		void ChangeWorldScale(float scalingFactor);

		inline void ScreenToWorld(const Point2D& n, Point2D& v);

	private:
		static Application* s_pInstance;

	private:
		std::unique_ptr<uint8_t[]> m_pFractal{ nullptr };
		std::unique_ptr<Window> m_pWindow{ nullptr };

		int m_Iterations{ 128 }, m_Threads{ 32 };
		std::vector<std::future<bool>> m_Futures;

		Point2D m_Offset{ 0, 0 }, m_StartPan{ 0, 0 }, m_MouseCoords{ 0.0, 0.0 };
		Point2D m_Scale;

		// ImGui
		bool m_bBinarySearch{ false }, m_bAVX{ false }; // Not yet implemented
		
		struct Vec3f
		{
			float x, y, z;
		} m_DynamicRGB{ 0.5f, 0.2f, 0.5f }, m_StaticRGB{ 0.5f, 0.5f, 0.5f }, m_RGBOffset{ 0.0f, 2.094f, 4.188f };


		// Event
		bool m_bRunning{ true }, m_bPanning{ false }, m_bScreenshot{ false };
		float m_ScalingFactor{ 1.0 };

	};
}

