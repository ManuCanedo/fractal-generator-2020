#include "fpch.h"

#include "imgui_layer.h"
#include "application.h"

namespace fractal
{
ImGuiLayer::ImGuiLayer(unsigned int window_width, unsigned int window_height)
	: window_w(window_width), window_h(window_height)
{
	init();
}

ImGuiLayer::~ImGuiLayer()
{
	shutdown();
}

void ImGuiLayer::on_update()
{
	ImGui::NewFrame();
	define_appui();
	ImGui::Render();
}

void ImGuiLayer::on_event(Event& event)
{
	EventDispatcher dispatcher(event);

	dispatcher.dispatch<MouseMovedEvent>([this](const MouseMovedEvent& event) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(static_cast<float>(event.get_x()),
				     static_cast<float>(event.get_y()));
		return false;
	});

	dispatcher.dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& event) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[static_cast<int>(event.get_button())] = true;
		return false;
	});

	dispatcher.dispatch<MouseButtonReleasedEvent>(
		[this](const MouseButtonReleasedEvent& event) {
			ImGuiIO& io = ImGui::GetIO();
			io.MouseDown[static_cast<int>(event.get_button())] = false;
			return false;
		});
}

void ImGuiLayer::define_appui()
{
	ImGui::SetNextWindowPos(ImVec2(window_w * 0.895, window_h * 0.01), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(window_w * 0.1, window_h * 0.25), ImGuiCond_Once);
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);

	static bool is_avx = true;
	static float r_offset = 0.0f, g_offset = 2.094f, b_offset = 4.188f;

	ImGui::Begin("Settings", false, ImGuiWindowFlags_NoResize);

	ImGui::Text("Color Settings");
	ImGui::SliderFloat("Red", &r_offset, 0.0f, 6.28f);
	ImGui::SliderFloat("Green", &g_offset, 0.0f, 6.28f);
	ImGui::SliderFloat("Blue", &b_offset, 0.0f, 6.28f);

	static Application& app{ Application::Get() };

	app.set_roffset(r_offset);
	app.set_goffset(g_offset);
	app.set_boffset(b_offset);

	if (ImGui::Button("Default")) {
		static float r_offset_def = 0.0f, g_offset_def = 2.094f, b_offset_def = 4.188f;

		r_offset = r_offset_def;
		g_offset = g_offset_def;
		b_offset = b_offset_def;
	}
	ImGui::SameLine();
	if (ImGui::Button("Screenshot"))
		app.set_save(true);

	ImGui::Text("");
	ImGui::Separator();
	ImGui::Text("Performance");
	ImGui::Text("%d Iterations", app.get_iters());
	ImGui::Text("Frametime %.2f ms", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Checkbox(" AVX2 ", &is_avx);
	ImGui::Text("");

	app.set_avx(is_avx);
	ImGui::End();
}

void ImGuiLayer::init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
}

void ImGuiLayer::shutdown()
{
	ImGui::DestroyContext();
}
} // namespace fractal
