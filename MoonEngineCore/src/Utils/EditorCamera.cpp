#include "mpch.h"
#include "EditorCamera.h"

#include "Core/Input.h"
#include "Core/Time.h"

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
	}

	void EditorCamera::SetProjection(float aspectRatio)
	{
		float left = -aspectRatio * m_ZoomLevel;
		float right = aspectRatio * m_ZoomLevel;
		float bottom = -m_ZoomLevel;
		float top = m_ZoomLevel;
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}

	void EditorCamera::Update()
	{
		if (Input::GetKey(KEY_LEFT_SHIFT))
		{
			if (Input::GetKey(KEY_R))
				m_Position = glm::vec2(0.0f, 0.0f);

			m_MoveSpeed = 14.0f;
			m_ZoomSpeed = 20.0f;
		}
		else
		{
			m_MoveSpeed = 7.0f;
			m_ZoomSpeed = 10.0f;
		}

		if (Input::GetKey(KEY_W))
			m_Position.y += m_MoveSpeed * Time::DeltaTime();
		else if (Input::GetKey(KEY_S))
			m_Position.y -= m_MoveSpeed * Time::DeltaTime();

		if (Input::GetKey(KEY_A))
			m_Position.x -= m_MoveSpeed * Time::DeltaTime();
		else if (Input::GetKey(KEY_D))
			m_Position.x += m_MoveSpeed * Time::DeltaTime();
	}

	void EditorCamera::Resize(float width, float height)
	{
		SetProjection(width / height);
		SetView();
		m_ViewProjection = m_Projection * m_View;
	}

	void EditorCamera::OnEvent(Event & event)
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
		return false;
	}

	EditorCamera::~EditorCamera()
	{}
}