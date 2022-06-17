#include "mpch.h"
#include "Time.h"

#include "GLFW/glfw3.h"

namespace MoonEngine
{
	float Time::m_Begin;
	float Time::m_End;
	float Time::m_DeltaTime;

	void Time::Init()
	{
		m_End = (float)glfwGetTime();
		m_Begin = 0.0f;
		m_End = 0.0f;
	}

	void Time::Calculate()
	{
		m_Begin = (float)glfwGetTime();
		m_DeltaTime = m_Begin - m_End;
		m_End = m_Begin;
	}
}