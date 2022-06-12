#pragma once
namespace MoonEngine
{
	class MouseScrollEvent : public Event
	{
	private:
		float x, y;
	public:
		MouseScrollEvent(float x, float y)
			:x(x), y(y)
		{
			SetType(EventType::MouseScroll);
		}

		float ScrollX() { return x; }
		float ScrollY() { return y; }
	};
}