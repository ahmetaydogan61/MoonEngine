#include "mpch.h"
#include "CameraControllerOrthographic.h"
#include "Event/Events.h"
#include "Core/Input.h"
#include "Core/Time.h"

namespace MoonEngine
{
	CameraControllerOrthographic::CameraControllerOrthographic(OrthographicCamera& camera)
		:m_Camera(camera), m_Position(0.0f, 0.0f), m_Rotation(0.0f), m_ZoomLevel(camera.GetZoom())
	{}

	void CameraControllerOrthographic::OnUpdate()
	{
		if (Input::GetKey(KEY_LEFT_SHIFT))
		{
			m_MoveSpeed = 14.0f;
			m_RotateSpeed = 200.0f;
			m_ZoomSpeed = 20.0f;
		}
		else
		{
			m_MoveSpeed = 7.0f;
			m_RotateSpeed = 75.0f;
			m_ZoomSpeed = 10.0f;
		}

		if (Input::GetKey(KEY_Q))
			m_Rotation += m_RotateSpeed * Time::DeltaTime();
		else  if (Input::GetKey(KEY_E))
			m_Rotation -= m_RotateSpeed * Time::DeltaTime();

		if (Input::GetKey(KEY_W))
			m_Position.y += m_MoveSpeed * Time::DeltaTime();
		else if (Input::GetKey(KEY_S))
			m_Position.y -= m_MoveSpeed * Time::DeltaTime();
	
		if (Input::GetKey(KEY_A))
			m_Position.x -= m_MoveSpeed * Time::DeltaTime();
		else if (Input::GetKey(KEY_D))
			m_Position.x += m_MoveSpeed * Time::DeltaTime();

		m_Camera.SetTranslation(m_Position, m_Rotation);
	}

	void CameraControllerOrthographic::OnEvent(Event& event)
	{
		EventHandler handler = { event };
		handler.HandleEvent(EventType::Mouse_Scroll, EVENT_FN_POINTER(OnMouseScroll));
	}

	bool CameraControllerOrthographic::OnMouseScroll(Event& event)
	{
		MouseScrollEvent& e = (MouseScrollEvent&)event;
		if (e.ScrollY() > 0)
			m_ZoomLevel -= m_ZoomSpeed * Time::DeltaTime();
		else
			m_ZoomLevel += m_ZoomSpeed * Time::DeltaTime();
		m_Camera.SetZoom(m_ZoomLevel);
		return false;
	}
}