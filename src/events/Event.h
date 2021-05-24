#ifndef EVENT_H
#define EVENT_H

namespace fractal
{
enum class EventType {
	NONE = 0,
	WIN_CLOSE,
	WIN_RESIZE,
	KEY_PRESS,
	KEY_RELEASE,
	KEY_HOLD,
	MOUSE_PRESS,
	MOUSE_RELEASE,
	MOUSE_HOLD,
	MOUSE_MOVE,
	MOUSE_SCROLL,
};

// Abstract parent class
class Event {
	friend class EventDispatcher;

public:
	virtual EventType type() const = 0;
	virtual std::string to_string() const = 0;

	friend std::ostream &operator<<(std::ostream os, const Event &e)
	{
		return os << e.to_string();
	}

protected:
	bool is_handled{ false };
};

// Dispatcher
class EventDispatcher {
	template <typename T> using event_func = std::is_function<bool(T &)>;

public:
	explicit EventDispatcher(Event &e) : event(e)
	{
	}

	template <typename T, typename F> bool dispatch(const F &func)
	{
		if (event.type() == T::Get()) {
			event.is_handled = func(static_cast<T &>(event));
			return true;
		}
		return false;
	}

private:
	Event &event;
};
}

#endif // EVENT_H