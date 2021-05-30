#include "fpch.h"

#include "window.h"
#include "ui/imgui_layer.h"
#include "win_window.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace fractal
{
static bool s_GLFW_init = false;

Window *Window::Get(const WindowProps &props)
{
	return new WindowsWindow(props);
}

WindowsWindow::WindowsWindow(const WindowProps &props)
{
	init(props);
}

WindowsWindow::~WindowsWindow()
{
	shutdown();
}

void WindowsWindow::on_update()
{
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	imgui_layer->on_update();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}

void WindowsWindow::set_event_callback(std::function<void(Event &)> on_event_func)
{
	data.func_eventcallback = on_event_func;
}

void WindowsWindow::set_vsync(bool enabled)
{
	if (enabled)
		glfwSwapInterval(0);
	else
		glfwSwapInterval(1);
}

void WindowsWindow::init(const WindowProps &props)
{
	// Window Setup
	data.name = props.name;
	data.width = props.width;
	data.height = props.height;

	LOG_INFO("Window created: {0} ({1}, {2})", props.name, props.width, props.height);

	// GLFW Setup
	if (!s_GLFW_init) {
		glfwInit();
		s_GLFW_init = true;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(props.width, props.height, props.name.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &data);
	set_vsync(false);

	// GLEW Setup
	if (glewInit() != GLEW_OK)
		LOG_ERROR("GlewInit error");

	// ImGui Setup
	imgui_layer = std::make_unique<ImGuiLayer>();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	// GLFW Callbacks
	glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
		data.width = width;
		data.height = height;

		WindowResizeEvent event{ width, height };
		data.func_eventcallback(event);
	});

	glfwSetWindowCloseCallback(window, [](GLFWwindow *window) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		WindowCloseEvent event;
		data.func_eventcallback(event);
	});

	glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action,
				      int mods) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		switch (action) {
		case GLFW_PRESS: {
			KeyPressedEvent event(static_cast<KeyCode>(key));
			data.func_eventcallback(event);
			break;
		}
		case GLFW_RELEASE: {
			KeyReleasedEvent event(static_cast<KeyCode>(key));
			data.func_eventcallback(event);
			break;
		}
		case GLFW_REPEAT: {
			KeyHeldEvent event(static_cast<KeyCode>(key));
			data.func_eventcallback(event);
			break;
		}
		}
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action,
					      int mods) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		switch (action) {
		case GLFW_PRESS: {
			MouseButtonPressedEvent event(static_cast<MouseButtonCode>(button));
			data.func_eventcallback(event);
			break;
		}
		case GLFW_RELEASE: {
			MouseButtonReleasedEvent event(static_cast<MouseButtonCode>(button));
			data.func_eventcallback(event);
			break;
		}
		}
	});

	glfwSetScrollCallback(window, [](GLFWwindow *window, double off_x, double off_y) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		MouseScrolledEvent event(off_x, off_y);
		data.func_eventcallback(event);
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow *window, double pos_x, double pos_y) {
		WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

		MouseMovedEvent event(pos_x, pos_y);
		data.func_eventcallback(event);
	});
}

void WindowsWindow::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(window);
}
}
