#pragma once
#include "Shader.h"

namespace MoonEngine
{
	class Renderer
	{
	private:
		static const int MAX_INSTANCES = 1000;
		static float m_RenderBuffer[];
		static int m_Index;

		static Shader* m_DefaultShader;
		static unsigned int m_VertexArray;
		static unsigned int m_VertexBuffer;
		static unsigned int m_ElementBufffer;
		static unsigned int m_InstanceVertexBuffer;
		static float m_Vertices[];
		static unsigned int m_Indices[];
		static glm::vec4 m_ClearColor;
	public:
		static void SetClearColor(glm::vec4& clearColor);
		static void Init();
		static void Clear();
		static void Render(const glm::mat4& viewProjection);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color);
		static void Destroy();
		
	};
}