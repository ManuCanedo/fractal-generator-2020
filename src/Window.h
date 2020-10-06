#pragma once

namespace Fractal
{
	struct WindowProperties
	{
		std::string title{ "Fractal World" };
		int width{ 1280 };
		int height{ 720 };
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual void SetEventCallback(const std::function<void(Event&)>) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		static Window* Create(const WindowProperties& props = WindowProperties());
	};
}

