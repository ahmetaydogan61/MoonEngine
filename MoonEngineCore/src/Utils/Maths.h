#pragma once
#include "glm/glm.hpp"

namespace MoonEngine
{
	struct Maths
	{
		static int Clamp(int value, int min, int max)
		{
			if (value < min)
				return min;
			else if (value > max)
				return max;
			else
				return value;
		}

		static float Clamp(float value, float min, float max)
		{
			if (value < min)
				return min;
			else if (value > max)
				return max;
			else
				return value;
		}

		static int RandomInt(int from, int to)
		{
			int dif = to - from;
			return (rand() % dif) + from;
		}

		static float RandomFloat(float from, float to)
		{
			return from + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (to - from)));
		}

		static int Lerp(int from, int to, int time)
		{
			return from + time * (to - from);
		}

		static float Lerp(float from, float to, float time)
		{
			return from + time * (to - from);
		}

		static glm::vec2 Lerp(glm::vec2 from, glm::vec2 to, float time)
		{
			return from + time * (to - from);
		}
	};
}