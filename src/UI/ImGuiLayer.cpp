#include "fpch.h"
#include "Application.h"
#include "ImGuiLayer.h"

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
		ImGui::NewFrame();
		DefineApplicationUI();
		ImGui::Render();
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseMovedEvent>([this](const MouseMovedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MousePos = ImVec2(static_cast<float>(event.GetX()), static_cast<float>(event.GetY()));
				return false;
			});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](const MouseButtonPressedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MouseDown[static_cast<int>(event.GetMouseButton())] = true;
				return false;
			});

		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](const MouseButtonReleasedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MouseDown[static_cast<int>(event.GetMouseButton())] = false;
				return false;
			});
	}

	void ImGuiLayer::DefineApplicationUI()
	{
		ImGui::SetNextWindowPos(ImVec2(1410, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(165, 310), ImGuiCond_Once);
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);

		static float rOffset{ 0.0f }, gOffset{ 2.094f }, bOffset{ 4.188f };
		static bool bAVX{ true }, bBinary{ false };

		ImGui::Begin("Settings", false, ImGuiWindowFlags_NoResize);

		ImGui::Text("Color Settings");
		ImGui::SliderFloat("Red", &rOffset, 0.0f, 6.28f);
		ImGui::SliderFloat("Green", &gOffset, 0.0f, 6.28f);
		ImGui::SliderFloat("Blue", &bOffset, 0.0f, 6.28f);

		static Application& app{ Application::GetApplication() };

		app.SetRedOffset(rOffset);
		app.SetGreenOffset(gOffset);
		app.SetBlueOffset(bOffset);

		if (ImGui::Button("Default"))
		{
			static float rOffsetDefault{ 0.0f }, gOffsetDefault{ 2.094f }, bOffsetDefault{ 4.188f };

			rOffset = rOffsetDefault;
			gOffset = gOffsetDefault;
			bOffset = bOffsetDefault;
		}
		ImGui::SameLine();
		if (ImGui::Button("Screenshot"))
			app.SetScreenshot(true);

		ImGui::Text("");

		ImGui::Separator();
		ImGui::Text("Performance");
		ImGui::Text("%d Iterations", app.GetIterations());
		ImGui::Text("Frametime %.2f ms", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Checkbox(" AVX2 ", &bAVX);
		ImGui::Text("");

		ImGui::Text("Time Complexity");
		ImGui::Checkbox(" Binary Search", &bBinary);

		app.SetModeAVX(bAVX);
		app.SetModeBinary(bBinary);
		ImGui::End();
	}

	void ImGuiLayer::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsClassic();
	}

	void ImGuiLayer::Shutdown()
	{
		ImGui::DestroyContext();
	}
}
