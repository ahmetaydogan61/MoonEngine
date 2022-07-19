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

			//Statics
			uint32_t DrawCalls;
			uint32_t QuadCount;
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
		static void Destroy();
		static void Clear();
		static void Begin(const glm::mat4& viewProjection);
		static void Render();
		static void End();

		static RenderData& GetRenderData() { return *rData; }

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0), const Ref<Texture>& texture = nullptr);
		static void DrawQuad(const glm::vec3& position, const glm::vec3& size = glm::vec3(1.0), const glm::vec3& rotation = glm::vec3(0.0), const glm::vec4& color = glm::vec4(1.0), const Ref<Texture>& texture = nullptr);

		static void SetClearColor(glm::vec4& clearColor);
		static glm::vec4& GetClearColor() { return m_ClearColor; }
	};
}