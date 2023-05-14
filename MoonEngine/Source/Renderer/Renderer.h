#pragma once

namespace MoonEngine
{
	class Texture;
	class TextureSheet;

	struct RenderData;
	struct TransformComponent;
	struct SpriteComponent;

	struct RendererStats
	{
		uint32_t MaxLayers;
		uint32_t DrawCalls;
	};

	class Renderer
	{
	public:
		//Application initializes this you dont need to call this. If you want a custom call, remove the call from Application.cpp
		static void Init();
		//Application initializes this you dont need to call this. If you want a custom call, remove the call from Application.cpp
		static void Terminate();

		static void DrawEntity(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation, const glm::vec4& color, const Shared<Texture>& texture = 0, int layer = 0, const glm::vec2& tiling = { 1.0f, 1.0f }, int entityId = -1);
		static void DrawEntity(const TransformComponent& tC, const SpriteComponent& sC, int entityId = -1);

		static void DrawEntity(const glm::mat4& transform, const glm::vec4& color, const Shared<Texture>& texture, int layer, const glm::vec2& tiling, int entityId);
		static void DrawEntity(const glm::mat4& transform, const glm::vec4& color, const Shared<TextureSheet>& spriteSheet, int layer, const glm::vec2& tiling, int entityId);

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityId = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, int entityId = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityId = -1);

		static void Begin(const glm::mat4& viewProjection);
		static void End();
		static void RenderIndexed(int layer = -1);
		static void RenderLines();

		static void SetClearColor(const glm::vec3& color);
		static const RendererStats& GetStats() { return *s_Stats; }
		static void SetLineWidth(float width);
	private:
		static RendererStats* s_Stats;
	};
}