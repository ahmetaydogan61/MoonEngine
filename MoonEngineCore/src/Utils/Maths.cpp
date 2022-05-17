#include "mpch.h"
#include "Maths.h"

namespace MoonEngine
{
	int Maths::Clamp(int value, int min, int max)
	{
		if (value < min)
			return min;
		else if (value > max)
			return max;
		else
			return value;
	}

	float Maths::Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		else if (value > max)
			return max;
		else
			return value;
	}

	int Maths::RandomInt(int from, int to)
	{
		int dif = to - from;
		return (rand() % dif) + from;
	}

	float Maths::RandomFloat(float from, float to)
	{
		return from + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (to - from)));
	}

	int Maths::Lerp(int from, int to, int time)
	{
		return from + time * (to - from);
	}

	float Maths::Lerp(float from, float to, float time)
	{
		return from + time * (to - from);
	}

	glm::vec2 Maths::Lerp(glm::vec2 from, glm::vec2 to, float time)
	{
		return from + time * (to - from);
	}
}