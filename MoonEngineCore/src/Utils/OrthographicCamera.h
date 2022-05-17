#pragma once
namespace MoonEngine
{
	class Event;

	class OrthographicCamera
	{
	private:
		glm::mat4 m_Projection;
		glm::mat4 m_View;
		glm::mat4 m_ViewProjection;
		float m_AspectRatio = 1.0f;

		glm::vec2 m_Position = { 0.0f, 0.0f };
		float m_ZoomLevel = 5.0f;
		float m_Rotation = 0.0f;

		void RecalculateViewMatrix();
		bool OnResize(Event& event);
	public:
		static OrthographicCamera* MAIN;
		OrthographicCamera(float aspectRatio);

		void SetProjection();
		void SetProjection(float aspectRatio);

		void SetTranslation(glm::vec2& position, float rotation) { m_Position = position; m_Rotation = rotation; RecalculateViewMatrix(); }
		const glm::vec2& GetPosition() const { return m_Position; }
		float GetRotation() const { return m_Rotation; }

		void SetZoom(float zoomLevel) { m_ZoomLevel = zoomLevel; SetProjection(); }
		float GetZoom() { return m_ZoomLevel; }
		
		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetViewProjection() const { return m_ViewProjection; }

		void OnEvent(Event& event);
	};
}