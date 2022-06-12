#pragma once
namespace MoonEngine
{
	class KeyPressEvent : public Event
	{
	private:
		int m_Key;
		bool m_Repeat;
	public:
		KeyPressEvent(int key, bool repeat)
			:m_Key(key), m_Repeat(repeat)
		{
			SetType(EventType::KeyPress);
		}

		int Key() { return m_Key; }
		bool Repeat() { return m_Repeat; }
	};
}