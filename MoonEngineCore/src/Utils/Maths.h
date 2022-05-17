#pragma once
namespace MoonEngine
{
	struct Maths
	{
		static int Clamp(int value, int min, int max);
		static float Clamp(float value, float min, float max);
		static int RandomInt(int from, int to);
		static float RandomFloat(float from, float to);
		static int Lerp(int from, int to, int time);
		static float Lerp(float from, float to, float time);
		static glm::vec2 Lerp(glm::vec2 from, glm::vec2 to, float time);
	};
}