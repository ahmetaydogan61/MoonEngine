#include "mpch.h"
#include "EditorCamera.h"

#include "Core/Input.h"
#include "Core/Time.h"

#include "Core/Window.h"

namespace MoonEngine
{
	EditorCamera::EditorCamera()
	{
		m_View = glm::mat4(1.0f);
		SetProjection(1.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void EditorCamera::SetView()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_Position.x, m_Position.y, 0.0f));
		m_View = glm::inverse(transform);
		m_ViewProjection = m_Projection * m_View;
	}

	void EditorCamera::SetProjection()
	{
		float left = -m_AspectRatio * m_ZoomLevel;
		float right = m_AspectRatio * m_ZoomLevel;
		float bottom = -m_ZoomLevel;
		float top = m_ZoomLevel;
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void EditorCamera::SetProjection(float aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		float left = -aspectRatio * m_ZoomLevel;
		float right = aspectRatio * m_ZoomLevel;
		float bottom = -m_ZoomLevel;
		float top = m_ZoomLevel;
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void EditorCamera::UpdateFocused()
	{}

	void EditorCamera::UpdateHovered()
	{
		if (Input::GetKey(KEY_LEFT_CONTROL))
		{
			if (Input::GetKey(KEY_W))
				m_Position.y += m_MoveSpeed * Time::DeltaTime();
			else if (Input::GetKey(KEY_S))
				m_Position.y -= m_MoveSpeed * Time::DeltaTime();

			if (Input::GetKey(KEY_A))
				m_Position.x -= m_MoveSpeed * Time::DeltaTime();
			else if (Input::GetKey(KEY_D))
				m_Position.x += m_MoveSpeed * Time::DeltaTime();
		}
	}

	void EditorCamera::Update()
	{
		//MousePan
		float deltaX = Input::GetX() - m_MouseLastX;
		float deltaY = Input::GetY() - m_MouseLastY;
		m_MouseLastX = Input::GetX();
		m_MouseLastY = Input::GetY();

		float normal_x = deltaX * 2.0f / Window::GetWidth();
		float normal_y = deltaY * 2.0f / Window::GetHeight();

		float fovRads = 60.0f * 3.14f / 180.0f;
		float fovTan = tan(fovRads / 2.0f);

		if (Input::MouseDown(2))
		{
			m_Position.x -= normal_x * fovTan * m_ZoomLevel * m_PanSpeed * Time::DeltaTime() * Window::GetAspectRatio();
			m_Position.y += normal_y * fovTan * m_ZoomLevel * m_PanSpeed * Time::DeltaTime();
		}
		//-----

		SetView();
	}

	void EditorCamera::Resize(float width, float height)
	{
		SetProjection(width / height);
		SetView();
	}

	void EditorCamera::OnEvent(Event& event)
	{
		EventHandler handler = { event };
		handler.HandleEvent(EventType::Mouse_Scroll, EVENT_FN_POINTER(OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(Event& event)
	{
		MouseScrollEvent& e = (MouseScrollEvent&)event;
		if (e.ScrollY() > 0)
			m_ZoomLevel -= m_ZoomSpeed * Time::DeltaTime();
		else
			m_ZoomLevel += m_ZoomSpeed * Time::DeltaTime();
		SetProjection();
		return false;
	}

	EditorCamera::~EditorCamera()
	{}
}