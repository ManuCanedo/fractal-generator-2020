#pragma once

#include "Event.h"

namespace Fractal
{
	enum class MouseButtonCode
	{
		LEFTCLICK = 0, RIGHTCLICK = 1, WHEEL = 2
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseButtonCode GetMouseButton() const { return m_MouseButtonCode; }

	protected:
		MouseButtonEvent(const MouseButtonCode button) : m_MouseButtonCode(button) {}
		MouseButtonCode m_MouseButtonCode;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseButtonCode code) : MouseButtonEvent(code) {}

		static EventType GetStaticType() { return EventType::MouseButtonPressed; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << static_cast<int>(m_MouseButtonCode);
			return ss.str();
		}
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseButtonCode code) : MouseButtonEvent(code) {}

		static EventType GetStaticType() { return EventType::MouseButtonReleased; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << static_cast<int>(m_MouseButtonCode);
			return ss.str();
		}
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		static EventType GetStaticType() { return EventType::MouseMoved; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: (" << m_MouseX << "," << m_MouseY << ")";
			return ss.str();
		}

	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent: public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_OffsetX(xOffset), m_OffsetY(yOffset) {}

		float GetOffsetX() const { return m_OffsetX; }
		float GetOffsetY() const { return m_OffsetY; }

		static EventType GetStaticType() { return EventType::MouseScrolled; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: (" << m_OffsetX << "," << m_OffsetY << ")";
			return ss.str();
		}

	private:
		float m_OffsetX, m_OffsetY;
	};
}

