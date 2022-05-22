#include "mpch.h"
#include "SceneCamera.h"

namespace MoonEngine
{
	void SceneCamera::SetProjection(float aspectRatio)
	{
		float left = -aspectRatio * m_Distance;
		float right = aspectRatio * m_Distance;
		float bottom = -m_Distance;
		float top = m_Distance;
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}

	void SceneCamera::Resize(float width, float height, float distance)
	{
		m_Distance = distance;
		SetProjection(width / height);
	}
}