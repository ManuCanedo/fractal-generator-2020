#pragma once

#include "Event.h"

namespace Fractal
{
	enum class MouseButtonCode
	{
		// glfw
		LEFTCLICK = 0, RIGHTCLICK = 1
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline MouseButtonCode GetMouseButton() const { return m_MouseButtonCode; }

	protected:
		MouseButtonEvent(const MouseButtonCode button) : m_MouseButtonCode(button) {}
		MouseButtonCode m_MouseButtonCode;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseButtonCode code) : MouseButtonEvent(code) {}

		static EventType GetStaticType() { return EventType::MouseButtonPressed; }

		inline virtual EventType GetEventType() const override { return GetStaticType(); }
		inline virtual std::string ToString() const override
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

		inline virtual EventType GetEventType() const override { return GetStaticType(); }
		inline virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << static_cast<int>(m_MouseButtonCode);
			return ss.str();
		}
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const double x, const double y)
			: m_MouseX(x), m_MouseY(y) {}

		inline double GetX() const { return m_MouseX; }
		inline double GetY() const { return m_MouseY; }

		static EventType GetStaticType() { return EventType::MouseMoved; }

		inline virtual EventType GetEventType() const override { return GetStaticType(); }
		inline virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: (" << m_MouseX << "," << m_MouseY << ")";
			return ss.str();
		}

	private:
		double m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent: public Event
	{
	public:
		MouseScrolledEvent(const double xOffset, const double yOffset)
			: m_OffsetX(xOffset), m_OffsetY(yOffset) {}

		inline double GetOffsetX() const { return m_OffsetX; }
		inline double GetOffsetY() const { return m_OffsetY; }

		static EventType GetStaticType() { return EventType::MouseScrolled; }

		inline virtual EventType GetEventType() const override { return GetStaticType(); }
		inline virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: (" << m_OffsetX << "," << m_OffsetY << ")";
			return ss.str();
		}

	private:
		double m_OffsetX, m_OffsetY;
	};
}

