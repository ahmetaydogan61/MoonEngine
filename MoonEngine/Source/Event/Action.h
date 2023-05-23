#pragma once
#include "Event/Events/KeyEvents.h"

namespace MoonEngine
{
	using EventListener = std::function<void()>;

	class Event
	{
	public:

		void Subscribe(std::function<void()>func) { m_Events.push_back(func); }
		void Unsubscribe(std::function<void()>func)
		{
			for (int i = 0; i < m_Events.size(); i++)
			{
				auto foo = m_Events[i];
				if (foo)
					if (getAddress(foo) == getAddress(func))
						m_Events.erase(m_Events.begin() + i);
			}
		}

		void operator+=(std::function<void()>func) { Subscribe(func); }
		void operator-=(std::function<void()>func) { Unsubscribe(func); }

		void Invoke()
		{
			for (auto& func : m_Events)
				func();
		}

		void Clear()
		{
			m_Events.clear();
		}
	private:
		std::vector<std::function<void()>> m_Events;

		size_t getAddress(std::function<void()> f) {
			typedef void (fnType)();
			fnType** fnPointer = f.template target<fnType*>();
			if (fnPointer)
				return (size_t)*fnPointer;
			else
				return 0;
		}
	};

	template<typename T>
	class Action
	{
	public:

		void Subscribe(std::function<void(T)>func) { m_Actions.push_back(func); }
		void Unsubscribe(std::function<void(T)>func)
		{
			for (int i = 0; i < m_Actions.size(); i++)
			{
				auto foo = m_Actions[i];
				if (foo)
					if (getAddress(foo) == getAddress(func))
						m_Actions.erase(m_Actions.begin() + i);
			}
		}

		void operator+=(std::function<void(T)>func) { Subscribe(func); }
		void operator-=(std::function<void(T)>func) { Unsubscribe(func); }

		void Invoke(T parameter)
		{
			for (auto& func : m_Actions)
				func(parameter);
		}

		void Clear()
		{
			m_Actions.clear();
		}
	private:
		std::vector<std::function<void(T)>> m_Actions;

		size_t getAddress(std::function<void(T)> f) {
			typedef void (fnType)(T);
			fnType** fnPointer = f.template target<fnType*>();
			if (fnPointer)
				return (size_t)*fnPointer;
			else
				return 0;
		}
	};

#define BIND_ACTION(func) std::bind(&func, this, std::placeholders::_1)

}
