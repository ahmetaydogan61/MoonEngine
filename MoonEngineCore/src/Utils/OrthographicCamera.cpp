#include "mpch.h"
#include "OrthographicCamera.h"
#include "Event/Events.h"

namespace MoonEngine
{
	OrthographicCamera* OrthographicCamera::MAIN;

	OrthographicCamera::OrthographicCamera(float aspectRatio)		
		:m_View(1.0f), m_AspectRatio(aspectRatio)
	{
		MAIN = this;
		SetProjection(aspectRatio);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::SetProjection()
	{
		float left = -m_AspectRatio * m_ZoomLevel;
		float right = m_AspectRatio * m_ZoomLevel;
		float bottom = -m_ZoomLevel;
		float top = m_ZoomLevel;
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::SetProjection(float aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		float left = -aspectRatio * m_ZoomLevel;
		float right = aspectRatio * m_ZoomLevel;
		float bottom = -m_ZoomLevel;
		float top = m_ZoomLevel;
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_Position.x, m_Position.y, 0.0f)) * 
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_View = glm::inverse(transform);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::OnEvent(Event& event)
	{
		EventHandler handler = { event };
		handler.HandleEvent(EventType::Window_Resize, EVENT_FN_POINTER(OnResize));
	}
	
	bool OrthographicCamera::OnResize(Event& event)
	{
		WindowResizeEvent& e = (WindowResizeEvent&)event;
		SetProjection(e.AspectRatio());
		return false;
	}
}
