#pragma once

namespace MoonEngine
{
	class Shader;
	class Texture;
	struct SpriteComponent;
	struct TransformComponent;

	enum class RenderMode
	{
		Solid,
		Wireframe
	};

	struct RendererData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertexCount = MaxQuads * 4;
		const uint32_t MaxIndexCount = MaxQuads * 6;

		//Data
		glm::mat4 ViewProjection;

		//Stats
		uint32_t DrawCalls;
		uint32_t VertexCount;
		uint32_t QuadCount;

		//Other
		glm::vec3 ClearColor;
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoord;
		int32_t TextureId;
		glm::vec2 Tiling;
		int EntityId;
	};

	class Renderer
	{
	private:
		bool m_RendererInitialized = false;
		uint32_t m_VertexArray = 0;
		uint32_t m_VertexBuffer = 0;
		uint32_t m_IndexBuffer = 0;
		uint32_t m_VertexIndex = 0;
		Vertex* m_Vertices = nullptr;

		Shared<Shader> m_Shader = nullptr;
		Shared<Texture> m_DefaultTexture = nullptr;
		RendererData m_RendererData = {};

		uint32_t m_TextureIndex = 0;
		int32_t m_TextureIds[32];
		std::unordered_map<Shared<Texture>, int32_t> m_TextureCache;

		int32_t GetTextureFromCache(const Shared<Texture>& texture);
	public:
		Renderer();
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		~Renderer() = default;

		void Terminate();

		void SetRenderData(const glm::mat4& viewProjection);
		void SetRenderMode(RenderMode renderMode);
		void SetClearColor(const glm::vec3& color);

		void Clear();
		void Begin();
		void Render();
		void End();

		void DrawQuad(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f),
					  const Shared<Texture>& texture = 0, const glm::vec4& color = glm::vec4(1.0f), const glm::vec2& tiling = glm::vec2(1.0f));
		void DrawQuad(const glm::mat4& transform, const Shared<Texture>& texture, const glm::vec4& color = glm::vec4(1.0f),
					  const glm::vec2& tiling = glm::vec2(1.0f));

		void DrawEntity(const TransformComponent& transformComponent, const SpriteComponent& spriteComponent, int entityId = -1);
		void DrawEntity(const glm::mat4& transform, const SpriteComponent& spriteComponent, int entityId = -1);
		void DrawEntity(const glm::mat4& transform, const Shared<Texture>& texture, const glm::vec4& color,
					  const glm::vec2& tiling, int entityId);

		const RendererData& GetRendererData() { return m_RendererData; }
		glm::vec3& GetClearColor() { return m_RendererData.ClearColor; }
	};
}