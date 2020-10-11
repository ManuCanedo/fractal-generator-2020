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

		int GetWidth() const override { return m_Data.Width; }
		int GetHeight() const override { return m_Data.Height; }
		GLFWwindow* GetWindowNative() { return m_pWindow; }

		void SetEventCallback(std::function<void(Event&)>) override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		static WindowsWindow& GetWindowsWindow() { return *s_pInstance; }

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

	private:
		static WindowsWindow* s_pInstance;
	};
}