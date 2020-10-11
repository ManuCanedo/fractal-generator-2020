#pragma once

#include "Event.h"

namespace Fractal
{
	enum class KeyCode
	{
		// glfw
		SPACE = 32, ESCAPE = 256,
		RIGHT = 262, LEFT = 263, DOWN = 264, UP = 265
	};

	class KeyEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; };

	protected:
		KeyEvent(const KeyCode key) : m_KeyCode(key) {}
		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode key) : KeyEvent(key) {}

		static EventType GetStaticType() { return EventType::KeyPressed; }

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
		KeyReleasedEvent(const KeyCode key) : KeyEvent(key) {}

		static EventType GetStaticType() { return EventType::KeyReleased; }

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
		KeyHeldEvent(const KeyCode key) : KeyEvent(key) {}

		static EventType GetStaticType() { return EventType::KeyHeld; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyHeldEvent: " << static_cast<int>(m_KeyCode);
			return ss.str();
		}
	};
}

