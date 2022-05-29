#pragma once
#include "Core.h"

namespace MoonEngine
{
	class Shader;
	class Texture;

	class Renderer
	{
	private:
		static const int MAX_INSTANCES = 1000;
		static float m_RenderBuffer[];
		static int m_Index;

		static Ref<Shader> m_DefaultShader;
		static unsigned int m_VertexArray;
		static unsigned int m_VertexBuffer;
		static unsigned int m_ElementBufffer;
		static unsigned int m_InstanceVertexBuffer;
		static float m_Vertices[];
		static unsigned int m_Indices[];

		static int m_TexureIds[32];
		static int CreateTextureCache(Ref<Texture> texture);
		static std::unordered_map<Ref<Texture>, int> m_TextureCache;
		static Ref<Texture> m_WhiteTexture;
		static int m_TextureID;

		static glm::vec4 m_ClearColor;
	public:
		static void SetClearColor(glm::vec4& clearColor);
		static void Init();
		static void Clear();
		static void Render(const glm::mat4& viewProjection);
		static void DrawQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, Ref<Texture> texture);
		static void Destroy();
	};
}