#pragma once
struct GLFWwindow;

#include "Event/Action.h"

namespace MoonEngine
{
	class Window
	{
	public:
		bool Init();
		void Terminate();
		
		void Update();

		void SetVsync(bool state);
		GLFWwindow* GetNative() { return m_Window; }

		inline static Action<float> OnDpiChange;
	private:
		GLFWwindow* m_Window;

		void BindWindowCallbacks();
	};
}