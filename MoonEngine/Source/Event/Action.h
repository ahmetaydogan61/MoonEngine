#pragma once
#include "Event/Events/KeyEvents.h"

namespace MoonEngine
{
	template<typename T>
	class Action
	{
	private:
		size_t getAddress(std::function<void(T)> f) {
			typedef void (fnType)(T);
			fnType** fnPointer = f.template target<fnType*>();
			if (fnPointer)
				return (size_t)*fnPointer;
			else
				return 0;
		}
	public:
		std::vector<std::function<void(T)>> m_Actions;

		void Subscribe(std::function<void(T)>func) { m_Actions.push_back(func); }
		void Unsubscribe(std::function<void(T)>func)
		{
			for (int i = 0; i < m_Actions.size(); i++)
			{
				auto foo = m_Actions[i];
				if(foo)
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
	};

#define BIND_ACTION(func) std::bind(&func, this, std::placeholders::_1)

}
