#include "mpch.h"

#include "Renderer/Camera.h"

namespace MoonEngine
{
	void CameraOrthographic::Resize(float aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		float left = -aspectRatio * m_Size;
		float right = aspectRatio * m_Size;
		float bottom = -m_Size;
		float top = m_Size;
		m_Projection = glm::ortho(left, right, bottom, top, m_NearClip, m_FarClip);

		m_ViewProjection = m_Projection * m_View;
	}

	void CameraOrthographic::Move(const glm::vec3& position)
	{
		m_View = glm::translate(glm::mat4(1.0f), position);
		m_View = glm::inverse(m_View);

		m_ViewProjection = m_Projection * m_View;
	}

	void CameraOrthographic::Zoom(float amount)
	{
		m_Size += amount;
		if (m_Size < 0.1f)
			m_Size = 0.1f;

		float left = -m_AspectRatio * m_Size;
		float right = m_AspectRatio * m_Size;
		float bottom = -m_Size;
		float top = m_Size;

		m_Projection = glm::ortho(left, right, bottom, top, m_NearClip, m_FarClip);
		m_ViewProjection = m_Projection * m_View;
	}
}
