#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include "event.h"

namespace fractal
{
enum class MouseButtonCode { LEFTCLICK = 0, RIGHTCLICK = 1, WHEEL = 2 };

class MouseButtonEvent : public Event {
public:
	constexpr MouseButtonCode get_button() const
	{
		return mbutton_code;
	}

protected:
	explicit MouseButtonEvent(const MouseButtonCode &button) : mbutton_code(button)
	{
	}

	MouseButtonCode mbutton_code;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
public:
	explicit MouseButtonPressedEvent(const MouseButtonCode &code) : MouseButtonEvent(code)
	{
	}

	static constexpr EventType Get()
	{
		return EventType::MOUSE_PRESS;
	}

	EventType type() const override
	{
		return Get();
	}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << static_cast<int>(mbutton_code);
		return ss.str();
	}
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
public:
	explicit MouseButtonReleasedEvent(const MouseButtonCode &code) : MouseButtonEvent(code)
	{
	}

	static constexpr EventType Get()
	{
		return EventType::MOUSE_RELEASE;
	}

	EventType type() const override
	{
		return Get();
	}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << static_cast<int>(mbutton_code);
		return ss.str();
	}
};

class MouseMovedEvent : public Event {
public:
	MouseMovedEvent(const double x, const double y) : mouse_x(x), mouse_y(y)
	{
	}

	constexpr double get_x() const
	{
		return mouse_x;
	}

	 constexpr double get_y() const
	{
		return mouse_y;
	}

	static constexpr EventType Get()
	{
		return EventType::MOUSE_MOVE;
	}

	EventType type() const override
	{
		return Get();
	}
	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: (" << mouse_x << "," << mouse_y << ")";
		return ss.str();
	}

private:
	double mouse_x, mouse_y;
};

class MouseScrolledEvent : public Event {
public:
	MouseScrolledEvent(const double x_off, const double y_off)
		: offset_x(x_off), offset_y(y_off)
	{
	}

	constexpr double get_offset_x() const
	{
		return offset_x;
	}

	constexpr double get_offset_y() const
	{
		return offset_y;
	}

	constexpr static EventType Get()
	{
		return EventType::MOUSE_SCROLL;
	}

	EventType type() const override
	{
		return Get();
	}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: (" << offset_x << "," << offset_y << ")";
		return ss.str();
	}

private:
	double offset_x, offset_y;
};
}

#endif // MOUSE_EVET_H