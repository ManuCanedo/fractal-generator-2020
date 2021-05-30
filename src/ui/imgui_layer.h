#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <imgui.h>

namespace fractal
{
class ImGuiLayer {
public:
	ImGuiLayer(unsigned int window_width, unsigned int window_height);
	~ImGuiLayer();

	void on_update();
	void on_event(Event& e);

private:
	void define_appui();
	void init();
	void shutdown();

private:
	unsigned int window_w, window_h;
};
} // namespace fractal

#endif // IMGUI_LAYER_H