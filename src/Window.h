#pragma once

namespace Fractal
{
	struct Point2D
	{
		double x{ 0.0 }, y{ 0.0 };
		constexpr Point2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}

		constexpr static Point2D InvertY(const Point2D& other) { return { other.x, -other.y }; }
		constexpr static Point2D InvertX(const Point2D& other) { return { -other.x, other.y }; }

		constexpr Point2D operator+(const Point2D& other) const { return { x + other.x, y + other.y }; }
		constexpr Point2D operator-(const Point2D& other) const { return { x - other.x, y - other.y }; }
		constexpr Point2D operator/(const Point2D& other) const { return { x / other.y, y / other.y }; }
		constexpr void operator+=(const Point2D& other) { x += other.x; y += other.y; }
		constexpr void operator-=(const Point2D& other) { x -= other.x; y -= other.y; }
		constexpr void operator*=(double other) { x *= other; y *= other; }
	};

	struct WindowProperties
	{
		std::string title{ "Fractal Explorer" };
		unsigned int width{ 1600 };
		unsigned int height{ 900 };
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual void SetEventCallback(const std::function<void(Event&)>) = 0;
		virtual void SetVSync(const bool enabled) = 0;

		static Window* Create(const WindowProperties& props = WindowProperties());
	};
}

