#pragma once

namespace MoonEngine
{
	class Camera
	{
	private:
		float m_AspectRatio = 1.0f;
		float m_NearClip = -1.0f;
		float m_FarClip = 1.0f;
		float m_Size = 1.0f;
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_View = glm::mat4(1.0f);
		glm::mat4 m_ViewProjection = glm::mat4(1.0f);
	public:
		Camera() = default;
		~Camera() = default;
		const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetProjection() const { return m_Projection; }
		const float GetSize() const { return m_Size; }

		void Resize(float aspectRatio);
		void Move(const glm::vec3& position);
		void Zoom(float amount);
	};
}