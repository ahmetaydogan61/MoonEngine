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
		Renderer() = delete;
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		~Renderer() = delete;
		
		static bool s_RendererInitialized;

		static uint32_t s_VertexArray;
		static uint32_t s_VertexBuffer;
		static uint32_t s_IndexBuffer;
		static uint32_t s_VertexIndex;
		static Vertex* s_Vertices;

		static Shared<Shader> s_Shader;
		static Shared<Texture> s_DefaultTexture;
		static RendererData s_RendererData;

		static uint32_t s_TextureIndex;
		static int32_t s_TextureIds[32];
		static std::unordered_map<Shared<Texture>, int32_t> s_TextureCache;

		static int32_t GetTextureFromCache(const Shared<Texture>& texture);
	public:
		static void Init();
		static void Terminate();

		static void SetRenderData(const glm::mat4& viewProjection);
		static void SetRenderMode(RenderMode renderMode);
		static void SetClearColor(const glm::vec3& color);

		static void Clear();
		static void Begin();
		static void Render();
		static void End();

		static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f),
					  const Shared<Texture>& texture = 0, const glm::vec4& color = glm::vec4(1.0f), const glm::vec2& tiling = glm::vec2(1.0f));
		static void DrawQuad(const glm::mat4& transform, const Shared<Texture>& texture, const glm::vec4& color = glm::vec4(1.0f),
					  const glm::vec2& tiling = glm::vec2(1.0f));

		static void DrawEntity(const TransformComponent& transformComponent, const SpriteComponent& spriteComponent, int entityId = -1);
		static void DrawEntity(const glm::mat4& transform, const SpriteComponent& spriteComponent, int entityId = -1);
		static void DrawEntity(const glm::mat4& transform, const Shared<Texture>& texture, const glm::vec4& color,
					  const glm::vec2& tiling, int entityId);

		static RendererData& GetRendererData(){ return s_RendererData; }
		static const glm::vec3& GetClearColor() { return s_RendererData.ClearColor; }
	};
}