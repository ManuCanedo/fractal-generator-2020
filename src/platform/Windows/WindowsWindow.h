#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Fractal
{
	class WindowsWindow : public Window
	{
	public:
		explicit WindowsWindow(const WindowProperties& props);
		~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		void SetEventCallback(const std::function<void(Event&)>) override;
		void SetVSync(const bool enabled) override;

	private:
		void Init(const WindowProperties& props);
		void Shutdown();

	private:
		GLFWwindow* m_Window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer; 

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;

			std::function<void(Event&)> fEventCallback;
		} m_Data;
	};
}