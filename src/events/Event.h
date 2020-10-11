#pragma once

namespace Fractal
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize,
		KeyPressed, KeyReleased, KeyHeld,
		MouseButtonPressed, MouseButtonReleased, MouseButtonHeld, MouseMoved, MouseScrolled,
	};

	class Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual std::string ToString() const = 0;

	protected:
		bool m_Handled{ false };
	};

	class EventDispatcher
	{
		template <typename T>
		using fEvent = std::is_function<bool(T&)>;

	public:
		EventDispatcher(Event& event) : m_Event(event) {}

		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream os, const Event& e)
	{
		return os << e.ToString();
	}
}
