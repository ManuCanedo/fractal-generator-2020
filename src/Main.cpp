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
	LOG_TRACE("Application quit");
	return 0;
}