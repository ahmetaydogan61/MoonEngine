#pragma once
namespace MoonEngine
{
	class Time
	{
	private:
		static float m_DeltaTime;
		static float m_Begin;
		static float m_End;
	public:
		static void Init();
		static void Calculate();
		static float DeltaTime() { return m_DeltaTime; }
	};
}