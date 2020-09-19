#pragma once

class Application
{
public:
	Application();
	~Application();

	void Run();
	static Application* CreateApplication();
};


