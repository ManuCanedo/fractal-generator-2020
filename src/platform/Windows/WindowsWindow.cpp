#include "fpch.h"

#include "Window.h"
#include "WindowsWindow.h"

namespace Fractal
{
	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowProperties& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProperties& props)
	{
		m_Data.Title = props.title;
		m_Data.Width = props.width;
		m_Data.Height = props.height;

		LOG_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!s_GLFWInitialized)
		{
			glfwInit();
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// GLEW
		if (glewInit() != GLEW_OK)
			LOG_ERROR("GlewInit error");

		// GLFW Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event{ width, height };
				data.fEventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				WindowCloseEvent event;
				data.fEventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(static_cast<KeyCode>(key));
						data.fEventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(static_cast<KeyCode>(key));
						data.fEventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyHeldEvent event(static_cast<KeyCode>(key));
						data.fEventCallback(event);
						break;
					}
				}
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(static_cast<MouseButtonCode>(button));
						data.fEventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(static_cast<MouseButtonCode>(button));
						data.fEventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						MouseButtonHeldEvent event(static_cast<MouseButtonCode>(button));
						data.fEventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double offsetX, double offsetY)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				MouseScrolledEvent event(offsetX, offsetY);
				data.fEventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double positionX, double positionY)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				MouseMovedEvent event(positionX, positionY);
				data.fEventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetEventCallback(std::function<void(Event&)> onEventFunction)
	{
		m_Data.fEventCallback = onEventFunction;
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}



}