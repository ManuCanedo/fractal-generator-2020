#ifndef WINDOW_EVENT_H
#define WINDOW_EVENT_H

#include "event.h"

namespace fractal
{
class WindowEvent : public Event {
public:
	virtual EventType type() const override = 0;
	virtual std::string to_string() const override = 0;
};

class WindowCloseEvent : public WindowEvent {
public:
	WindowCloseEvent() = default;

	static constexpr EventType Get()
	{
		return EventType::WIN_CLOSE;
	}

	EventType type() const override
	{
		return Get();
	}

	std::string to_string() const override
	{
		return "WindowCloseEvent";
	}
};

class WindowResizeEvent : public WindowEvent {
public:
	WindowResizeEvent(const int w, const int h) : width(w), height(h)
	{
	}

	constexpr unsigned int get_width() const
	{
		return width;
	}
	constexpr unsigned int get_height() const
	{
		return height;
	}

	static constexpr EventType Get()
	{
		return EventType::WIN_RESIZE;
	}

	EventType type() const override
	{
		return Get();
	}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: (" << width << "," << height << ")";
		return ss.str();
	}

private:
	unsigned int width, height;
};
} // namespace fractal

#endif // WINDOW_EVENT_H