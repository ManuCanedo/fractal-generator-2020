#pragma once

#include <imgui.h>

namespace Fractal
{
	class ImGuiLayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnUpdate();
		void OnEvent(Event& event);

	private:
		void DefineUI();
		void Init();
		void Shutdown();
	};
}
