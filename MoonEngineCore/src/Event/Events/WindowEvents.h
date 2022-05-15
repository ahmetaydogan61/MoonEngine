#pragma once
namespace MoonEngine
{
	class WindowResizeEvent : public Event
	{
	private:
		int width, height;
	public:
		WindowResizeEvent(int& width, int& height)
			:width(width), height(height)
		{
			SetType(EventType::Window_Resize);
		}

		int Width()
		{
			return width;
		}

		int Height()
		{
			return height;
		}

		float AspectRatio()
		{
			return (float)width / (float)height;
		}
	};
}