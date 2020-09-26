#pragma once

namespace Fractal 
{
	class Application
	{
	public:
		Application() = default;
		~Application() = default;

		void Run();
		static Application* CreateApplication();
	};
}

