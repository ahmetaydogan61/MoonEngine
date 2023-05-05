#pragma once
#include <Core/Input.h>
#include <Event/Action.h>
#include <Renderer/Camera.h>

namespace MoonEngine
{
	class EditorCamera : public CameraOrthographic
	{
	private:
		float m_CurrentSpeed = 0.0f;
		float m_CurrentPanSpeed = 0.0f;
		float m_CurrentZoomSpeed = 0.0f;

		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec2 m_MouseLastPos = glm::vec2(0.0f);

		float SpeedFactor() const { return GetSize() * 0.25f; }

		bool m_IsPanning = false, m_Focused = false, m_Hovered = false;
	public:
		EditorCamera()
		{
			ME_LOG("Created");
			Input::OnMouseScroll += BIND_ACTION(EditorCamera::ZoomEvent);
		}
		~EditorCamera()
		{
			ME_LOG("Destroyed");
			Input::OnMouseScroll -= BIND_ACTION(EditorCamera::ZoomEvent);
		}

		float Speed = 7.5f;
		float SpeedMultiplier = 2.5f;
		float PanSpeed = 1.0f;
		float ZoomSpeed = 0.4f;

		void Update(bool focused, bool hovered);
		void ZoomEvent(float amount);
	};
}