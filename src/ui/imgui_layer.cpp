#include "fpch.h"

#include "application.h"
#include "imgui_layer.h"

namespace fractal
{
ImGuiLayer::ImGuiLayer()
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

void ImGuiLayer::on_event(Event &event)
{
	EventDispatcher dispatcher(event);

	dispatcher.dispatch<MouseMovedEvent>([this](const MouseMovedEvent &event) {
		ImGuiIO &io = ImGui::GetIO();
		io.MousePos = ImVec2(static_cast<float>(event.get_x()),
				     static_cast<float>(event.get_y()));
		return false;
	});

	dispatcher.dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent &event) {
		ImGuiIO &io = ImGui::GetIO();
		io.MouseDown[static_cast<int>(event.get_button())] = true;
		return false;
	});

	dispatcher.dispatch<MouseButtonReleasedEvent>(
		[this](const MouseButtonReleasedEvent &event) {
			ImGuiIO &io = ImGui::GetIO();
			io.MouseDown[static_cast<int>(event.get_button())] = false;
			return false;
		});
}

void ImGuiLayer::define_appui()
{
	ImGui::SetNextWindowPos(ImVec2(1410, 20), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(165, 310), ImGuiCond_Once);
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);

	static float r_offset = 4.8f, g_offset = 1.0f, b_offset = 3.6f;
	static bool is_avx = true, is_complex_visualizer = false;

	ImGui::Begin("Settings", false, ImGuiWindowFlags_NoResize);

	ImGui::Text("Color Settings");
	ImGui::SliderFloat("Red", &r_offset, 0.0f, 6.28f);
	ImGui::SliderFloat("Green", &g_offset, 0.0f, 6.28f);
	ImGui::SliderFloat("Blue", &b_offset, 0.0f, 6.28f);

	static Application &app{ Application::Get() };

	app.set_roffset(r_offset);
	app.set_goffset(g_offset);
	app.set_boffset(b_offset);

	if (ImGui::Button("Default")) {
		static float r_offset_def = 4.8f, g_offset_def = 1.0f, b_offset_def = 3.6f;

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

	ImGui::Text("Visualising Complexity");
	ImGui::Checkbox(" nlog(n)", &is_complex_visualizer);

	app.set_avx(is_avx);
	app.set_complex_visualizer(is_complex_visualizer);
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
}
