#include "Application.h"
#include "Log.h"

int main()
{
	Fractal::Log::Init();
	LOG_TRACE("System Log initialized");

	auto app = Fractal::Application::CreateApplication();
	app->Application::Run();

	delete app;
	LOG_WARN("Application quit");
	return 0;
}