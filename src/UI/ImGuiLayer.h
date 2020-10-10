#pragma once

#include "Platform/OpenGL/imgui_impl_glfw.h"
#include "Platform/OpenGL/imgui_impl_opengl3.h"
#include <GL/glew.h>

namespace Fractal
{
	class ImGuiLayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnUpdate();
		void OnEvent(Event& event);

		void Hide(bool bHide) { m_bHidden = bHide; }
		bool IsHidden() { return m_bHidden; }

	private:
		void DefineUI();
		void Init();
		void Shutdown();

	private:
		ImVec4 m_ClearColor{ 0.45f, 0.55f, 0.60f, 1.00f };
		ImVec2 m_MouseCoords{ 0.0f, 0.0f };

		bool m_bHidden{ false };
		float m_Time{ 0.0f };

	};
}
