#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <imgui.h>

namespace fractal
{
class ImGuiLayer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	void on_update();
	void on_event(Event &e);

private:
	void define_appui();
	void init();
	void shutdown();
};
} // namespace fractal

#endif // IMGUI_LAYER_H