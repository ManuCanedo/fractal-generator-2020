#include <iostream>

#include "Application.h"
#include "Log.h"

int main()
{
	Fractal::Log::Init();
	LOG_TRACE("System logger initialized");

	auto app = Fractal::Application::CreateApplication();
	app->Application::Run();

	delete app;
	LOG_TRACE("Resources freed");

	LOG_WARN("Execution finished\n\nPress ENTER to quit...");
	std::cin.get();
	return 0;
}