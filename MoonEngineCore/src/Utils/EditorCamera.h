#pragma once
#include "Camera.h"
#include "Event/Events.h"

namespace MoonEngine
{
	class EditorCamera : public Camera
	{
	private:
		glm::mat4 m_View;
		glm::mat4 m_ViewProjection;
		glm::vec2 m_Position = { 0.0f, 0.0f };
		float m_AspectRatio = 1.0f;
		float m_ZoomLevel = 5.0f;
		float m_MoveSpeed = 7.0f;
		float m_ZoomSpeed = 10.0f;

		float m_PanSpeed = 125.0f;
		float m_MouseLastX = 0.0f;
		float m_MouseLastY = 0.0f;

		void SetView();
		void SetProjection();
		void SetProjection(float aspectRatio);

		bool OnMouseScroll(Event& event);
	public:
		EditorCamera();
		~EditorCamera();
		void UpdateFocused();
		void UpdateHovered();
		void Update();
		void Resize(float width, float height);
		void OnEvent(Event& event);
		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
	};
}