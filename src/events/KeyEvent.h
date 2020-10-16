#pragma once

#include "Event.h"

namespace Fractal
{
	enum class KeyCode
	{
		SPACE = 32, ESCAPE = 256,
		RIGHT = 262, LEFT = 263, DOWN = 264, UP = 265
	};

	class KeyEvent : public Event
	{
	public:
		constexpr KeyCode GetKeyCode() const { return m_KeyCode; };

	protected:
		explicit constexpr KeyEvent(const KeyCode& key) : m_KeyCode(key) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		explicit constexpr KeyPressedEvent(const KeyCode& key) : KeyEvent(key) {}

		constexpr static EventType GetStaticType() { return EventType::KeyPressed; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << static_cast<int>(m_KeyCode);
			return ss.str();
		}
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		explicit constexpr KeyReleasedEvent(const KeyCode& key) : KeyEvent(key) {}

		constexpr static EventType GetStaticType() { return EventType::KeyReleased; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << static_cast<int>(m_KeyCode);
			return ss.str();
		}
	};

	class KeyHeldEvent : public KeyEvent
	{
	public:
		explicit constexpr KeyHeldEvent(const KeyCode& key) : KeyEvent(key) {}

		constexpr static EventType GetStaticType() { return EventType::KeyHeld; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyHeldEvent: " << static_cast<int>(m_KeyCode);
			return ss.str();
		}
	};
}

