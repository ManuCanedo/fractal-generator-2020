#ifndef WIN_WINDOW_H
#define WIN_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace fractal
{
class WindowsWindow : public Window {
public:
	explicit WindowsWindow(const WindowProps &props);
	~WindowsWindow();

	void on_update() override;

	unsigned int width() const override
	{
		return data.width;
	}

	unsigned int height() const override
	{
		return data.height;
	}

	void set_event_callback(const std::function<void(Event &)>) override;
	void set_vsync(const bool enabled) override;

private:
	void init(const WindowProps &props);
	void shutdown();

private:
	GLFWwindow *window;
	std::unique_ptr<ImGuiLayer> imgui_layer;

	struct WindowData {
		std::string name{ "" };
		unsigned int width{}, height{};
		std::function<void(Event &)> func_eventcallback;
	} data;
};
}

#endif // WIN_WINDOW_H
