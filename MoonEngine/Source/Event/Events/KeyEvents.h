#pragma once
namespace MoonEngine
{
	enum class Keycode;

	class KeyPressEvent
	{
	private:
		Keycode m_Key;
		bool m_Repeat;
	public:
		KeyPressEvent(Keycode key, bool repeat)
			:m_Key(key), m_Repeat(repeat)
		{
		}

		Keycode Key() { return m_Key; }
		bool IsRepeat() { return m_Repeat; }
	};
}