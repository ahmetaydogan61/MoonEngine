#pragma once
namespace MoonEngine
{
	enum class Keycode;

	class KeyPressEvent : public Event
	{
	private:
		Keycode m_Key;
		bool m_Repeat;
	public:
		KeyPressEvent(Keycode key, bool repeat)
			:m_Key(key), m_Repeat(repeat)
		{
			SetType(EventType::KeyPress);
		}

		Keycode Key() { return m_Key; }
		bool Repeat() { return m_Repeat; }
	};
}