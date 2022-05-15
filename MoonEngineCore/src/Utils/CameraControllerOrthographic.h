#pragma once
#include "OrthographicCamera.h"

namespace MoonEngine
{
	class CameraControllerOrthographic
	{
	private:
		OrthographicCamera& m_Camera;
		float m_MoveSpeed = 1.0f;
		float m_RotateSpeed = 100.0f;
		float m_ZoomSpeed = 10.0f;
		glm::vec2 m_Position;
		float m_Rotation;
		float m_ZoomLevel;
		bool OnMouseScroll(Event& event);
	public:
		CameraControllerOrthographic(OrthographicCamera& camera);
		~CameraControllerOrthographic() = default;
		void OnUpdate();
		void OnEvent(Event& event);
	};
}