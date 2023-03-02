#pragma once

namespace MoonEngine
{
	class Shader;
	class Texture;
	struct SpriteComponent;
	struct TransformComponent;

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoord;
		int32_t TextureId;
		glm::vec2 Tiling;
		int EntityId;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		int EntityId;
	};

	enum class RenderMode
	{
		Solid,
		Wireframe
	};

	struct RendererStats
	{
		uint32_t DrawCalls;
		uint32_t VertexCount;

		uint32_t QuadCount;
		uint32_t LineCount;
	};

	struct RendererData
	{
		//Configs
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertexCount = MaxQuads * 4;
		const uint32_t MaxIndexCount = MaxQuads * 6;

		//Data
		glm::mat4 ViewProjection;

		//Other
		glm::vec3 ClearColor;

		//Quad Renderer
		uint32_t QuadVertexArray;
		uint32_t QuadVertexBuffer;
		uint32_t QuadIndexBuffer;

		uint32_t QuadVertexIndex;
		QuadVertex* QuadVertices;

		Shared<Shader> QuadShader;
		Shared<Texture> DefaultTexture;

		//Line Renderer
		uint32_t LineVertexArray;
		uint32_t LineVertexBuffer;

		uint32_t LineVertexIndex;
		LineVertex* LineVertices;
		
		Shared<Shader> LineShader;
		float LineWidth = 3.0f;
	};

	class Renderer
	{
	private:
		Renderer() = delete;
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		~Renderer() = delete;
		
		static bool s_RendererInitialized;
	
		static RendererStats s_Stats;
		static RendererData s_Data;

		static uint32_t s_TextureIndex;
		static int32_t s_TextureIds[32];
		static std::unordered_map<Shared<Texture>, int32_t> s_TextureCache;

		static int32_t GetTextureFromCache(const Shared<Texture>& texture);
	public:
		static void Init();
		static void Terminate();

		static void SetClearColor(const glm::vec3& color);
		static const glm::vec3& GetClearColor() { return s_Data.ClearColor; }

		static void SetLineWidth(float width);
		static void SetRenderMode(RenderMode renderMode);

		static void Clear();
		static void SetRenderData(const glm::mat4& viewProjection);
		static void Begin();
		static void RenderIndexed();
		static void RenderLines();
		static void End();

		static const RendererStats& GetStats() { return s_Stats; }
		static const RendererData& GetData() { return s_Data; }
		
		static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f),
					  const Shared<Texture>& texture = 0, const glm::vec4& color = glm::vec4(1.0f), const glm::vec2& tiling = glm::vec2(1.0f));
		static void DrawQuad(const glm::mat4& transform, const Shared<Texture>& texture, const glm::vec4& color = glm::vec4(1.0f),
					  const glm::vec2& tiling = glm::vec2(1.0f));

		static void DrawEntity(const TransformComponent& transformComponent, const SpriteComponent& spriteComponent, int entityId = -1);
		static void DrawEntity(const glm::mat4& transform, const SpriteComponent& spriteComponent, int entityId = -1);
		static void DrawEntity(const glm::mat4& transform, const Shared<Texture>& texture, const glm::vec4& color,
					  const glm::vec2& tiling, int entityId);

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityId = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, int entityId = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityId = -1);
	};
}