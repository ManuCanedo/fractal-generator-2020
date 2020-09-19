#include "Application.h"

int main()
{
	auto app = Application::CreateApplication();
	app->Application::Run();

	delete app;
	return 0;
}