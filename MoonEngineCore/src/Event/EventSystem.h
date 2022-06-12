#pragma once
#include "mpch.h"

namespace MoonEngine
{
	enum class EventType
	{
		NONE = 0,
		MouseScroll,
		KeyPress,
		WindowResize, 
	};

	class Event
	{
	private:
		EventType m_Type = EventType::NONE;
	public:
		bool Handled = false;
		EventType& GetType() { return m_Type; }
	protected:
		void SetType(const EventType& type) { m_Type = type; }
	};

	class EventHandler
	{
	private:
		Event& m_Event;
	public:
		EventHandler(Event& event)
			:m_Event(event)
		{	
		}

		void HandleEvent(const EventType& eventType, const std::function<bool(Event&)>& func)
		{
			if (m_Event.Handled)
				return;

			if (m_Event.GetType() == eventType)
				m_Event.Handled = func(m_Event);
		}

		//For Debug
		void HandleEvent(const EventType& eventType, const std::function<bool(Event&)>& func, const char* name)
		{
			if (m_Event.Handled)
				return;

			if (m_Event.GetType() == eventType)
			{
				DebugNor(name);
				m_Event.Handled = func(m_Event);
			}
		}
	};
}