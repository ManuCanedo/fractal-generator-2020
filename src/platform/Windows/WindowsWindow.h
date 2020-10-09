#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Fractal
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		void SetEventCallback(std::function<void(Event&)>) override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline int GetWidth() const override { return m_Data.Width; }
		inline int GetHeight() const override { return m_Data.Height; }
		inline GLFWwindow* GetGLFWwindow() { return m_pWindow; }

	private:
		void Init(const WindowProperties& props);
		void Shutdown();

	private:
		GLFWwindow* m_pWindow;
		std::unique_ptr<ImGuiLayer> m_pImGuiLayer; 

		struct WindowData
		{
			std::string Title{ "Fractal Explorer" };
			int Width{ 1600 }, Height{ 900 };
			bool VSync{ false };

			std::function<void(Event&)> fEventCallback;
		} m_Data;
	};

}