#include "Application.h"
#include "Log.h"

int main()
{
	Log::Init();
	LOG_WARN("Log Initialized");
	LOG_INFO("System is working fine");
	

	auto app = Application::CreateApplication();
	app->Application::Run();

	delete app;
	return 0;
}