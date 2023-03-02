#include "mpch.h"
#include "Utils/EditorCamera.h"

#include <Core/Application.h>
#include <Core/Time.h>

namespace MoonEngine
{
	void CameraController::Update(bool focused, bool hovered)
	{
		if (!m_Camera)
			return;

		m_Focused = focused;
		m_Hovered = hovered;

		if (Input::GetKey(Keycode::LeftShift))
		{
			m_CurrentPanSpeed = PanSpeed * SpeedMultiplier;
			m_CurrentSpeed = Speed * SpeedMultiplier * SpeedFactor();
			m_CurrentZoomSpeed = ZoomSpeed * SpeedMultiplier;
		}
		else
		{
			m_CurrentPanSpeed = PanSpeed;
			m_CurrentSpeed = Speed * SpeedFactor();
			m_CurrentZoomSpeed = ZoomSpeed;
		}

		glm::vec2 cursorPos = Input::GetMouseScreenPos();
		float deltaX = cursorPos.x - m_MouseLastPos.x;
		float deltaY = cursorPos.y - m_MouseLastPos.y;
		m_MouseLastPos.x = cursorPos.x;
		m_MouseLastPos.y = cursorPos.y;

		const Resolution& res = Application::GetResoultion();
		float normalX = deltaX * 2.0f / res.Width;
		float normalY = deltaY * 2.0f / res.Height;

		float fovRads = 60.0f * 3.14f / 180.0f;
		float fovTan = tan(fovRads / 2.0f);

		if (Input::GetMouseButtonDown(2) && hovered && !m_IsPanning)
			m_IsPanning = true;
		else if (!Input::GetMouseButton(2))
			m_IsPanning = false;

		if (m_IsPanning)
		{
			float cameraSize = m_Camera->GetSize();
			m_Position.x -= normalX * cameraSize * m_CurrentPanSpeed * ((float)(res.Width) / res.Height);
			m_Position.y += normalY * cameraSize * m_CurrentPanSpeed;
		}

		if (focused && Input::GetKey(Keycode::LeftShift))
		{
			if (Input::GetKey(Keycode::W))
				m_Position += glm::vec3(0.0f, m_CurrentSpeed * Time::DeltaTime(), 0.0f);
			else if (Input::GetKey(Keycode::S))
				m_Position -= glm::vec3(0.0f, m_CurrentSpeed * Time::DeltaTime(), 0.0f);

			if (Input::GetKey(Keycode::D))
				m_Position += glm::vec3(m_CurrentSpeed * Time::DeltaTime(), 0.0f, 0.0f);
			else if (Input::GetKey(Keycode::A))
				m_Position -= glm::vec3(m_CurrentSpeed * Time::DeltaTime(), 0.0f, 0.0f);
		}

		m_Camera->Move(m_Position);
	}

	void CameraController::Zoom(float amount)
	{
		if (!m_Camera)
			return;

		if (!m_Hovered)
			return;

		float zoomSpeed = SpeedFactor();
		zoomSpeed = std::min(zoomSpeed, 50.0f);
		float zoomAmount = -amount * m_CurrentZoomSpeed * zoomSpeed;
		m_Camera->Zoom(zoomAmount);
	}
}