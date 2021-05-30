#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include "event.h"

namespace fractal
{
enum class KeyCode { SPACE = 32, ESCAPE = 256, RIGHT = 262, LEFT = 263, DOWN = 264, UP = 265 };

class KeyEvent : public Event {
public:
	constexpr KeyCode get_keycode() const
	{
		return keycode;
	};

protected:
	explicit KeyEvent(const KeyCode &key) : keycode(key)
	{
	}

	KeyCode keycode;
};

class KeyPressedEvent : public KeyEvent {
public:
	explicit KeyPressedEvent(const KeyCode &key) : KeyEvent(key)
	{
	}

	static constexpr EventType Get()
	{
		return EventType::KEY_PRESS;
	}

	EventType type() const override
	{
		return Get();
	}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << static_cast<int>(keycode);
		return ss.str();
	}
};

class KeyReleasedEvent : public KeyEvent {
public:
	explicit KeyReleasedEvent(const KeyCode &key) : KeyEvent(key)
	{
	}

	static constexpr EventType Get()
	{
		return EventType::KEY_RELEASE;
	}

	EventType type() const override
	{
		return Get();
	}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << static_cast<int>(keycode);
		return ss.str();
	}
};

class KeyHeldEvent : public KeyEvent {
public:
	explicit KeyHeldEvent(const KeyCode &key) : KeyEvent(key)
	{
	}

	constexpr static EventType Get()
	{
		return EventType::KEY_HOLD;
	}

	EventType type() const override
	{
		return Get();
	}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyHeldEvent: " << static_cast<int>(keycode);
		return ss.str();
	}
};
} // namespace fractal

#endif // KEY_EVENT_H