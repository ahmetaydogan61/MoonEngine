#pragma once
namespace MoonEngine
{
	enum class Keycode;

	class KeyPressEvent
	{
	public:
		KeyPressEvent(Keycode key, bool repeat)
			:m_Key(key), m_Repeat(repeat)
		{
		}

		Keycode Key() { return m_Key; }
		bool IsRepeat() { return m_Repeat; }
	private:
		Keycode m_Key;
		bool m_Repeat;
	};
}