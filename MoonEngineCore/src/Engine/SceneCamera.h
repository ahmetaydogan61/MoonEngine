#pragma once
#include "Utils/Camera.h"

namespace MoonEngine
{
	class SceneCamera : public Camera
	{
	private:
		float m_Distance = 5.0f;
		void SetProjection(float aspectRatio);
	public:
		SceneCamera() = default;
		~SceneCamera() = default;
		void Resize(float width, float height, float distance);
	};
}