#pragma once

namespace Fractal
{
	struct Point2D
	{
		double x{ 0 }, y{ 0 };

		Point2D operator+(const Point2D& other) const { return { x + other.x, y + other.y }; }
		Point2D operator-(const Point2D& other) const { return { x - other.x, y - other.y }; }
		Point2D operator/(const Point2D& other) const { return { x / other.y, y / other.y }; }

		void operator+=(const Point2D& other) { x += other.x; y += other.y; }
		void operator-=(const Point2D& other) { x -= other.x; y -= other.y; }
		void operator*=(double other) { x *= other; y *= other; }

	};

	struct ScrSection
	{
		Point2D topLeft{ 0 }, bottomRight{ 0 };
		int scrWidth{ 1280 };
	};

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

