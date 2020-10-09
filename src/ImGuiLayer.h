#pragma once

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

namespace Fractal
{
	class ImGuiLayer
	{
	public:
		ImGuiLayer(GLFWwindow* window);
		~ImGuiLayer();

		void OnUpdate();
		void OnEvent(Event& event);

		inline void SetVisibility(bool bVisible) { m_bVisible = bVisible; }
		inline bool IsVisible() { return m_bVisible; }

	private:
		void Init();
		void Shutdown();

		void DefineUI();
		void RenderUI();

	private:
		GLFWwindow* m_pWindow{ nullptr };

		ImVec4 m_ClearColor{ 0.45f, 0.55f, 0.60f, 1.00f };
		bool m_bVisible{ true };
		float m_Time{ 0.0f };
	};
}
