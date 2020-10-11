#include "fpch.h"

#include "Window.h"
#include "UI/ImGuiLayer.h"
#include "WindowsWindow.h"

namespace Fractal
{
	static bool s_GLFWInitialized = false;
	WindowsWindow* WindowsWindow::s_pInstance{ nullptr };

	Window* Window::Create(const WindowProperties& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& props)
	{
		s_pInstance = this;
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		m_pImGuiLayer->OnUpdate();
		glfwPollEvents();
		glfwSwapBuffers(m_pWindow);
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

		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		m_pWindow = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_pWindow);
		glfwSetWindowUserPointer(m_pWindow, &m_Data);
		SetVSync(true);

		if (glewInit() != GLEW_OK) LOG_ERROR("GlewInit error");

		// Set up ImGui
		m_pImGuiLayer = std::make_unique<ImGuiLayer>();

		// GLFW Callbacks
		glfwSetWindowSizeCallback(m_pWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event{ width, height };
				data.fEventCallback(event);
			});

		glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				WindowCloseEvent event;
				data.fEventCallback(event);
			});

		glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

		glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods)
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
				}
			});

		glfwSetScrollCallback(m_pWindow, [](GLFWwindow* window, double offsetX, double offsetY)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				MouseScrolledEvent event(static_cast<float>(offsetX), static_cast<float>(offsetY));
				data.fEventCallback(event);
			});

		glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double positionX, double positionY)
			{
				WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				MouseMovedEvent event(static_cast<float>(positionX), static_cast<float>(positionY));
				data.fEventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_pWindow);
	}
}