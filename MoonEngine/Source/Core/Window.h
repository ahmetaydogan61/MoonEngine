#pragma once
struct GLFWwindow;

#include "Event/Action.h"

namespace MoonEngine
{
	class Window
	{
	private:
		GLFWwindow* m_Window;

		void BindWindowCallbacks();
	public:
		bool Init();
		void Update();
		void Terminate();
		GLFWwindow* GetNative() { return m_Window; }

		void SetVsync(bool state);

		inline static Action<float> OnDpiChange;
	};
}