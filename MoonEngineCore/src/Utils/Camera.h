#pragma once

namespace MoonEngine
{
	class Camera
	{
	protected:
		glm::mat4 m_Projection;
	public:
		Camera() = default;
		virtual ~Camera() = default;
		const glm::mat4& GetProjection() const { return m_Projection; }
	};
}