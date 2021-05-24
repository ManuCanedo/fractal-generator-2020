#ifndef WINDOW_H
#define WINDOW_H

namespace fractal
{
struct Point2D {
	double x{ 0.0 }, y{ 0.0 };

	constexpr Point2D(double x = 0.0, double y = 0.0) : x(x), y(y)
	{
	}

	constexpr static Point2D inv_y(const Point2D &other)
	{
		return { other.x, -other.y };
	}

	constexpr static Point2D inv_x(const Point2D &other)
	{
		return { -other.x, other.y };
	}

	constexpr Point2D operator+(const Point2D &other) const
	{
		return { x + other.x, y + other.y };
	}

	constexpr Point2D operator-(const Point2D &other) const
	{
		return { x - other.x, y - other.y };
	}

	constexpr Point2D operator/(const Point2D &other) const
	{
		return { x / other.y, y / other.y };
	}

	constexpr void operator+=(const Point2D &other)
	{
		x += other.x;
		y += other.y;
	}

	constexpr void operator-=(const Point2D &other)
	{
		x -= other.x;
		y -= other.y;
	}

	constexpr void operator*=(const double other)
	{
		x *= other;
		y *= other;
	}
};

struct WindowProps {
	std::string name{ "Fractal Explorer" };
	unsigned int width{ 1600 };
	unsigned int height{ 900 };
};

class Window {
public:
	virtual void on_update() = 0;

	virtual unsigned int width() const = 0;
	virtual unsigned int height() const = 0;
	virtual void set_event_callback(const std::function<void(Event &)>) = 0;
	virtual void set_vsync(const bool enabled) = 0;

	static Window *Get(const WindowProps &props = WindowProps());
};
}

#endif // WINDOW_H
