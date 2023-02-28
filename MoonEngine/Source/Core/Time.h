#pragma once

namespace MoonEngine
{
	class Time
	{
	private:
		inline static float s_DeltaTime;
		float m_LastTime;

		//Function controlled by the Application
		void Calculate(float time)
		{
			s_DeltaTime = time - m_LastTime;
			m_LastTime = time;
		}
		//-
		friend class Application;
	public:
		static float DeltaTime() { return s_DeltaTime; }
	};
}