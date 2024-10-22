#pragma once

namespace MoonEngine
{
	struct Maths
	{
		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
		static int Clamp(int value, int min, int max);
		static float Clamp(float value, float min, float max);
		static float Normalize(float value, float min, float max);
		static int RandomInt(int from, int to);
		static float RandomFloat(float from, float to);
		static int Lerp(int from, int to, int time);
		static float Lerp(float from, float to, float time);
		static glm::vec2 Lerp(const glm::vec2& from, const glm::vec2& to, float time);
		static glm::vec3 Lerp(const glm::vec3& from, const glm::vec3& to, float time);
		static glm::vec4 Lerp(const glm::vec4& from, const glm::vec4& to, float time);
	};
}