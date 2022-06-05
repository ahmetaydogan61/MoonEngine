#pragma once
#include "Core.h"

namespace MoonEngine
{
	class Shader;
	class Texture;

	class Renderer
	{
	private:
		struct RenderData
		{
			glm::mat4 ViewProjection;
			int DrawCalls;
		};
		static RenderData* rData;

		static Ref<Shader> m_DefaultShader;
		
		static int m_TextureIndex;
		static int m_TextureIDs[];
		static std::unordered_map<Ref<Texture>, int> m_TextureCache;
		static Ref<Texture> m_WhiteTexture;

		static glm::vec4 m_ClearColor;

		static int CreateTextureCache(const Ref<Texture>& texture);
	public:
		static void Init();
		static void Begin(const glm::mat4& viewProjection);
		static void End();
		static void Clear();
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const glm::vec4& color, const Ref<Texture>& texture);
		static void Render();
		static void SetClearColor(glm::vec4& clearColor);
		static void Destroy();
		static RenderData& GetRenderData() { return *rData; }
	};
}