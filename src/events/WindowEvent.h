#pragma once

#include "Event.h"

namespace Fractal
{
	class WindowEvent : public Event
	{
	public:
		virtual EventType GetEventType() const override = 0;
		virtual std::string ToString() const override = 0;
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		constexpr WindowCloseEvent() = default;

		constexpr static EventType GetStaticType() { return EventType::WindowClose; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override { return "WindowCloseEvent"; }
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		constexpr WindowResizeEvent(const int width, const int height)
			: m_Width(width), m_Height(height) {}

		constexpr int GetWidth() { return m_Width; }
		constexpr int GetHeight() { return m_Height; }

		constexpr static EventType GetStaticType() { return EventType::WindowResize; }

		EventType GetEventType() const override { return GetStaticType(); }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: (" << m_Width << "," << m_Height << ")";
			return ss.str();
		}

	private:
		int m_Width, m_Height;
	};
}

