#include "fpch.h"
#include "Application.h"
#include "ImGuiLayer.h"
#include "Platform/Windows/WindowsWindow.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <GL/glew.h>

namespace Fractal
{
	ImGuiLayer::ImGuiLayer()
	{
		Init();
	}

	ImGuiLayer::~ImGuiLayer()
	{
		Shutdown();
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::GetApplication();

		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		DefineUI();
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MousePos = ImVec2(event.GetX(), event.GetY());
				return false;
			});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MouseDown[static_cast<int>(event.GetMouseButton())] = true;
				return false;
			});

		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MouseDown[static_cast<int>(event.GetMouseButton())] = false;
				return false;
			});
	}

	void ImGuiLayer::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io{ ImGui::GetIO() }; (void)io;
		WindowsWindow& window{ WindowsWindow::GetWindowsWindow() };

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		ImGui::StyleColorsClassic();

		ImGui_ImplGlfw_InitForOpenGL(window.GetWindowNative(), true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::DefineUI()
	{
		static float rOffset{ 0.0f }, gOffset{ 2.094f }, bOffset{ 4.188f };
		static bool bAVX{ false };
		Application& app{ Application::GetApplication() };

		ImGui::Begin("Fractal Configuration");
		
		ImGui::Text("Color Settings");
		ImGui::SliderFloat("Red", &rOffset, 0.0f, 6.28f);
		ImGui::SliderFloat("Green", &gOffset, 0.0f, 6.28f);
		ImGui::SliderFloat("Blue", &bOffset, 0.0f, 6.28f);

		if (ImGui::Button("Default"))
		{
			rOffset = 0.0f; 
			gOffset = 2.094f; 
			bOffset = 4.188f;
		}

		app.SetRGBOffset(rOffset, gOffset, bOffset);

		ImGui::Text("------------------------------");
		ImGui::Text("Performance");

		ImGui::Text("%d Iterations", app.GetIterations());
		ImGui::Text("Frametime %.2f ms", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Checkbox("AVX Mode", &bAVX);

		app.SetAVX(bAVX);

		ImGui::End();
	}
}
