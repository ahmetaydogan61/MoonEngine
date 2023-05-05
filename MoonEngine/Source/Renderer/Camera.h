#pragma once

namespace MoonEngine
{
	class Camera
	{
	public:
		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
	protected:
		glm::mat4 m_View = glm::mat4(1.0f);
		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_ViewProjection = glm::mat4(1.0f);
	};

	class CameraOrthographic : public Camera
	{
	public:
		void Resize(float aspectRatio);
		void Move(const glm::vec3& position);
		void Zoom(float amount);

		const float GetSize() const { return m_Size; }
	private:
		float m_AspectRatio = 1.0f;
		float m_NearClip = -1.0f;
		float m_FarClip = 1.0f;
		float m_Size = 1.0f;
	};
}