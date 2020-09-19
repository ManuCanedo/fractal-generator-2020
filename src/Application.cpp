#include "Application.h"

Application::Application() {}
Application::~Application() {}

void Application::Run()
{
	while (true);
}

Application* Application::CreateApplication()
{
	return new Application;
}