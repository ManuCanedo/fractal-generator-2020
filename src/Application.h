#pragma once
namespace Fractal 
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
		static Application* CreateApplication();
	};
}

