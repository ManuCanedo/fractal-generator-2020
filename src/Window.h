#pragma once

namespace Fractal
{
	struct Point2D
	{
		float x{ 0.0f }, y{ 0.0f };
		Point2D(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

		static Point2D InvertY(const Point2D& other) { return { other.x, -other.y }; }
		static Point2D InvertX(const Point2D& other) { return { -other.x, other.y }; }

		Point2D operator+(const Point2D& other) const { return { x + other.x, y + other.y }; }
		Point2D operator-(const Point2D& other) const { return { x - other.x, y - other.y }; }
		Point2D operator/(const Point2D& other) const { return { x / other.y, y / other.y }; }
		void operator+=(const Point2D& other) { x += other.x; y += other.y; }
		void operator-=(const Point2D& other) { x -= other.x; y -= other.y; }
		void operator*=(float other) { x *= other; y *= other; }
	};

	struct WindowProperties
	{
		std::string title{ "Fractal Explorer" };
		int width{ 1600 };
		int height{ 900 };
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

