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
		constexpr MouseButtonCode GetMouseButton() const { return m_MouseButtonCode; }

	protected:
		explicit MouseButtonEvent(const MouseButtonCode& button) : m_MouseButtonCode(button) {}

		MouseButtonCode m_MouseButtonCode;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		explicit MouseButtonPressedEvent(const MouseButtonCode& code) : MouseButtonEvent(code) {}

		constexpr static EventType GetStaticType() { return EventType::MouseButtonPressed; }

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
		explicit MouseButtonReleasedEvent(const MouseButtonCode& code) : MouseButtonEvent(code) {}

		constexpr static EventType GetStaticType() { return EventType::MouseButtonReleased; }

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
		MouseMovedEvent(const double x, const double y)
			: m_MouseX(x), m_MouseY(y) {}

		constexpr double GetX() const { return m_MouseX; }
		constexpr double GetY() const { return m_MouseY; }

		constexpr static EventType GetStaticType() { return EventType::MouseMoved; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
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

		constexpr double GetOffsetX() const { return m_OffsetX; }
		constexpr double GetOffsetY() const { return m_OffsetY; }

		constexpr static EventType GetStaticType() { return EventType::MouseScrolled; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: (" << m_OffsetX << "," << m_OffsetY << ")";
			return ss.str();
		}

	private:
		double m_OffsetX, m_OffsetY;
	};
}

