#pragma once
#include "Shader.h"

namespace MoonEngine
{
	class OrthographicCamera;

	class Renderer
	{
	private:
		static const int MAX_INSTANCES = 1000;
		static float m_RenderBuffer[];
		static int m_Index;

		static Shader* m_DefaultShader;
		static unsigned int m_VertexArray;
		static unsigned int m_VertexBuffer;
		static unsigned int m_IndexBuffer;
		static unsigned int m_InstanceVertexBuffer;
		static float m_Vertices[];
		static unsigned int m_Indices[];
	public:
		static void Init();
		static void Clear();
		static void Render(OrthographicCamera& camera);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color);
		static void Destroy();
	};
}