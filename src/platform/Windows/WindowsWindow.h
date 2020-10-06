#pragma once



namespace Fractal
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline int GetWidth() const override { return m_Data.Width; }
		inline int GetHeight() const override { return m_Data.Height; }
		inline GLFWwindow* GetGLFWwindow() { return m_Window; }

		void SetEventCallback(std::function<void(Event&)>) override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

	private:
		virtual void Init(const WindowProperties& props);
		virtual void Shutdown();

	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title{ "Fractal World" };
			int Width{ 1280 }, Height{ 720 };
			bool VSync{ false };

			std::function<void(Event&)> fEventCallback;
		} m_Data;
	};

}